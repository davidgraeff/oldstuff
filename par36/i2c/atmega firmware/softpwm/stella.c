/*
* Copyright (c) 2007 by Stefan Siegl <stesie@brokenpipe.de>
* Copyright (c) 2009 by David Gräff <david.graeff@web.de>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
* For more information on the GPL, please go to:
* http://www.gnu.org/copyleft/gpl.html
*/

#include <stdint.h>
#include <avr/io.h>
#include "stella.h"

uint8_t stella_brightness[STELLA_CHANNELS];
uint8_t stella_fade[STELLA_CHANNELS];

uint8_t stella_fade_step = STELLA_FADE_STEP_INIT;
volatile uint8_t stella_fade_counter = 0;

volatile enum stella_update_sync stella_sync;
uint8_t stella_portmask[STELLA_PORT_COUNT];

struct stella_timetable_struct timetable_1, timetable_2;
struct stella_timetable_struct* int_table;
struct stella_timetable_struct* cal_table;


void stella_sort(void);

/* Initialize stella */
void
stella_init (void)
{
	int_table = &timetable_1;
	cal_table = &timetable_2;
	cal_table->head = 0;
	
	stella_sync = NOTHING_NEW;

	/* set stella port pins to output and save the port mask */
	stella_portmask[0] = ((1 << STELLA_CHANNELS) - 1) << STELLA_OFFSET_PORT1;
	STELLA_DDR_PORT1 |= stella_portmask[0];
	cal_table->port[0].port = &STELLA_PORT1;
	cal_table->port[0].mask = 0;


	/* initialise the fade counter. Fading works like this:
	* -> decrement fade_counter
	* -> on zero, fade if neccessary
	* -> reset counter to fade_step
	*/
	stella_fade_counter = stella_fade_step;

	stella_sort();

	/* we need at least 64 ticks for the compare interrupt,
	* therefore choose a prescaler of at least 64. */
	
	#ifdef STELLA_HIGHFREQ
	/* High frequency PWM Mode, 64 Prescaler */
	STELLA_PRESCALER = _BV(STELLA_CS2);
	//debug_printf("Stella freq: %u Hz\n", F_CPU/64/(256*2));
	#else
	/* Normal PWM Mode, 128 Prescaler */
	STELLA_PRESCALER |= _BV(STELLA_CS0) | _BV(STELLA_CS2);
	//debug_printf("Stella freq: %u Hz\n", F_CPU/128/(256*2));
	#endif

	/* Interrupt on overflow and CompareMatch */
	STELLA_TIMSK |= _BV(STELLA_TOIE) | _BV(STELLA_COMPARE_IE);
}

/* Process recurring actions for stella */
void
stella_process (void)
{

	/* the main loop is too fast, slow down */
	if (stella_fade_counter == 0)
	{
		uint8_t i;
		/* Fade channels. stella_fade_counter is 0 currently. Set to 1
		if fading changed a channel brigthness value */
		for (i = 0; i < STELLA_CHANNELS; ++i)
		{
			if (stella_brightness[i] == stella_fade[i])
				continue;

			if (stella_brightness[i] > stella_fade[i])
				stella_brightness[i]--;
			else /* stella_color[i] < stella_fade[i] */
				stella_brightness[i]++;
			stella_fade_counter = 1;
		}

		if (stella_fade_counter) stella_sync = UPDATE_VALUES;

		/* reset counter */
		stella_fade_counter = stella_fade_step;
	}

	/* sort if new values are available */
	if (stella_sync == UPDATE_VALUES)
		stella_sort();
}

void
stella_setValue(const enum stella_set_function func, const uint8_t channel, const uint8_t value)
{
	#ifdef DEBUG_STELLA
		debug_printf("STELLA: channel: %d of %d\n", channel+1, STELLA_CHANNELS);
	#endif
	if (channel >= STELLA_CHANNELS) return;

	switch (func)
	{
		case STELLA_SET_IMMEDIATELY:
			stella_brightness[channel] = value;
			stella_fade[channel] = value;
			stella_sync = UPDATE_VALUES;
			#ifdef DEBUG_STELLA
				debug_printf("STELLA: set immediately  value: %d\n", value);
			#endif
			break;
		case STELLA_SET_FADE:
			stella_fade[channel] = value;
			#ifdef DEBUG_STELLA
				debug_printf("STELLA: set fadeing value: %d\n", value);
			#endif
			break;
		default:
			#ifdef DEBUG_STELLA
				debug_printf("STELLA: What? you set to %d\n", func);
			#endif
			break;
	}
}

void stella_setFadestep(const uint8_t fadestep) {
  stella_fade_step = fadestep;
}

uint8_t stella_getFadestep() {
  return stella_fade_step;
}

/* Get a channel value.
 * Only call this function with a channel<STELLA_CHANNELS ! */
inline uint8_t
stella_getValue(const uint8_t channel)
{
	return stella_brightness[channel];
}

/* How to use:
 * Do not call this directly, but use "stella_sync = UPDATE_VALUES" instead.
 * Purpose:
 * Sort channels' brightness values from high to low (and the
 * interrupt time points from low to high), to be able to switch on
 * channels one after the other depending on their brightness level
 * and point in time.
 * Implementation details:
 * Use a "linked list" to avoid expensive memory copies. Main difference
 * to a real linked list is, that all elements are already preallocated
 * on the stack and are not allocated on demand.
 * The function directly writes to a "just calculated"-structure and if we
 * want new values in the pwm interrupt, we just have to swap pointers from
 * the "interrupt save"-structure to the "just calculated"-structure. (The
 * meaning of both structures changes, too, of course.)
 * Although we provide each channel in the structure with its neccessary
 * information such as portmask and brightness level, we will actually
 * ignore brightness levels of 0% and 100% due to not linking them to the linked list.
 * 100%-level channels are switched on at the beginning of each
 * pwm cycle and not touched afterwards. Channels with same brightness
 * levels are merged together (their portmask at least).
 * */
inline void
stella_sort()
{
	struct stella_timetable_entry* current, *last;
	uint8_t i;

	cal_table->head = 0;
	cal_table->port[0].mask = 0;
	cal_table->port[0].port = &STELLA_PORT1;

	for (i=0;i<STELLA_CHANNELS;++i)
	{
		/* set data of channel i */
		cal_table->channel[i].port.mask = _BV(i+STELLA_OFFSET_PORT1);
		cal_table->channel[i].port.port = &STELLA_PORT1;

		cal_table->channel[i].value = 255 - stella_brightness[i];
		cal_table->channel[i].next = 0;

		/* Special case: 0% brightness (Don't include this channel!) */
		if (stella_brightness[i] == 0) continue;

		//cal_table->portmask |= _BV(i+STELLA_OFFSET);

		/* Special case: 100% brightness (Merge pwm cycle start masks! Don't include this channel!) */
		if (stella_brightness[i] == 255)
		{
				cal_table->port[0].mask |= _BV(i+STELLA_OFFSET_PORT1);
			continue;
		}

		/* first item in linked list */
		if (!cal_table->head)
		{
			cal_table->head = &(cal_table->channel[i]);
			continue;
		}
		/* add to linked list with >=1 entries */
		current = cal_table->head; last = 0;
		while (current)
		{
			// same value as current item: do not add to linked list
			// but just update the portmask (DO THIS ONLY IF BOTH CHANNELS OPERATE ON THE SAME PORT)
			if (current->value == cal_table->channel[i].value && current->port.port == cal_table->channel[i].port.port)
			{
				#ifdef STELLA_PINS_PORT2
				if (i>=STELLA_PINS_PORT1)
					current->port.mask |= _BV( (i-STELLA_PINS_PORT1) +STELLA_OFFSET_PORT2);
				else
					current->port.mask |= _BV(i+STELLA_OFFSET_PORT1);
				#else
				current->port.mask |= _BV(i+STELLA_OFFSET_PORT1);
				#endif
				break;
			}
			// insert our new value at the head of the list
			else if (!last && current->value > cal_table->channel[i].value)
			{
				cal_table->channel[i].next = cal_table->head;
				cal_table->head = &(cal_table->channel[i]);
				break;
			}
			// insert our new value somewhere in betweem
			else if (current->value > cal_table->channel[i].value)
			{
				cal_table->channel[i].next = last->next;
				last->next = &(cal_table->channel[i]);
				break;
			}
			// reached the end of the linked list: just append our new entry
			else if (!current->next)
			{
				current->next = &(cal_table->channel[i]);
				break;
			}
			// else go to the next item in the linked list
			else
			{
				last = current;
				current = current->next;
			}
		}
	}

	#ifdef DEBUG_STELLA
	// debug out
	current = cal_table->head;
	i = 0;
	while (current)
	{
		i++;
		//debug_printf("%u %s\n", current->value, debug_binary(current->portmask));
		current = current->next;
	}
	debug_printf("Mask1: %s %u\n", debug_binary(stella_portmask[0]), stella_portmask[0]);
	#ifdef STELLA_PINS_PORT2
	debug_printf("Mask2: %s %u\n", debug_binary(stella_portmask[1]), stella_portmask[1]);
	#endif
	#endif

	/* Allow the interrupt to actually apply the calculated values */
	stella_sync = NEW_VALUES;
}