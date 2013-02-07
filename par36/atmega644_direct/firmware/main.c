
#include "config.h"

#include <inttypes.h>
#include <avr/io.h>
//#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <string.h>

#include "softpwm/stella.h"
#include "curtain/curtain.h"
#include "serial/uart.h"

#define soft_reset()        \
do                          \
{                           \
wdt_enable(WDTO_15MS);  \
for(;;)                 \
{                       \
}                       \
} while(0)

// Function Pototype
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

// Function Implementation
void wdt_init(void)
{
    MCUSR = 0;
    wdt_disable();

    return;
}

inline void sendStella(void)
{
    unsigned char data[4+STELLA_CHANNELS] = {'O','K','L',STELLA_CHANNELS};

    for (uint8_t i=0;i<STELLA_CHANNELS;++i)
        data[i+4] = stella_getValue(i);
	for (uint8_t i=0;i<sizeof(data);++i)
		uart_putc(data[i]);
}

inline void sendInit(void)
{
    unsigned char data[] = {'O','K','I',PROTOCOL_VERSION};
	for (uint8_t i=0;i<sizeof(data);++i)
		uart_putc(data[i]);
}


inline void sendMotor(void)
{
    unsigned char data[] = {'O','K','M',curtain_getPosition(),curtain_getMax()};
	for (uint8_t i=0;i<sizeof(data);++i)
		uart_putc(data[i]);
}

inline void sendAck(void)
{
    unsigned char data[] = {'O','K','A'};
	for (uint8_t i=0;i<sizeof(data);++i)
		uart_putc(data[i]);
}

uint8_t storedValues[STELLA_CHANNELS];
inline void storeValues(void) {
	for (uint8_t i=0;i<STELLA_CHANNELS;++i) {
		storedValues[i] = stella_getValue(i);
		stella_setValue(STELLA_SET_FADE, i, 0);
	}
}

inline void restoreValues(void) {
	for (uint8_t i=0;i<STELLA_CHANNELS;++i) {
		stella_setValue(STELLA_SET_FADE, i, storedValues[i]);
	}
}

int main(void)
{
    // Alles TriState
    DDRA = 0;
    DDRB = 0;
    DDRC = 0;
    DDRD = 0;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;

    // save power
    MCUCR |= _BV(JTD);
    PRR |= _BV(PRTWI) | _BV(PRSPI) | _BV(PRADC);
    //ADCSRA &= ~_BV(ADEN);
    //ACSR |=_BV(ACD);

    // Power Led
    DDRA|=_BV(7);
    PIN_SET(A,7);

    sei();

    // Init: Subsysteme
    uart_init(UART_BAUD_SELECT(BAUD,F_CPU));
    stella_init();
//     curtain_init();
    //sensors_init();

    // Init: Variablen
    enum stateEnum {
        StateCommands,
        StateCurtainValue,
        StateLedChannel,
        StateLedValue
    } laststate, state = StateCommands;

    uint8_t slowdown_counter = 0;
    uint8_t slowdown_counter2 = 0;
    uint8_t panic_counter = 0;
	uint8_t panic_counter_led = 0;
	uint8_t panic_counter_brightness = 0;
    uint8_t stella_channel = 0;
    enum stella_set_function stella_fading = STELLA_SET_IMMEDIATELY;

	uint8_t packetPosition = 0;
	unsigned char lastChar, haslastChar = 0;
    slowdown_counter = 0;
    slowdown_counter2 = 0;
    stella_setValue(STELLA_SET_IMMEDIATELY, 0, 0);
    stella_process();

	 wdt_enable(WDTO_250MS);
	#define PANIC_THRESHOLD 150
    // Hauptprogramm
    while (1) {
		wdt_reset();
        stella_process();
        curtain_process();
        //sensors_process();

        //panic counter
        if (++slowdown_counter == 0) {

            if (++slowdown_counter2==0) {
                if (panic_counter < PANIC_THRESHOLD)
                    ++panic_counter;
            }
            
			if (panic_counter == PANIC_THRESHOLD) {
				panic_counter = PANIC_THRESHOLD+1;
				panic_counter_led = 0;
				panic_counter_brightness = 0;
				packetPosition = 0;
				storeValues();
				PIN_SET(A,6);
			}
			if (panic_counter == PANIC_THRESHOLD+1) {
				if (panic_counter_brightness==255) {
					panic_counter=PANIC_THRESHOLD+2;
					continue;
				}
				stella_setValue(STELLA_SET_FADE, panic_counter_led, ++panic_counter_brightness);
			}
			if (panic_counter == PANIC_THRESHOLD+2) {
				if (panic_counter_brightness==0) {
					panic_counter=PANIC_THRESHOLD+1;
					if (++panic_counter_led >= STELLA_CHANNELS)
						panic_counter_led = 0;
					continue;
				}
				stella_setValue(STELLA_SET_FADE, panic_counter_led, --panic_counter_brightness);
			}
        }

		uint16_t r = uart_getc();
		uint8_t info = r >> 8;
		if (r == UART_NO_DATA)
			continue;
		else if (info!=0) {
			PIN_SET(A,6);
			PIN_SET(A,7);
			packetPosition = 0;
			continue;
		}
		
		const unsigned char temp = haslastChar ? lastChar : 0;
		haslastChar = 1;
		lastChar = r & 0xff;
		if (lastChar == 0xff && temp == 0xff) { // two proceeding 255-values start a valid packet
			packetPosition = 1;
			state = StateCommands;
			continue;
		}

		if (!packetPosition)
			continue;
        
		// reset panic counter
		if (panic_counter) {
			if (panic_counter>=110) {
				restoreValues();
				PIN_CLEAR(A,6);
			}
			panic_counter = 0;
		}
		
		laststate = state;
		switch (state) {
		case StateCommands:
			switch (lastChar) {
			case 0xef: // get values
				sendInit();
				sendStella();
				//sendSensor();
				sendMotor();
				break;
			case 0xdf: // prepare for a curtain value
				state = StateCurtainValue;
				break;
			case 0xcf: // prepare for a led channel and value (fade:immediately)
				state = StateLedChannel;
				stella_fading = STELLA_SET_IMMEDIATELY;
				break;
			case 0xbf: // prepare for a led channel and value (fade:fade)
				state = StateLedChannel;
				stella_fading = STELLA_SET_FADE;
				break;
			case 0xaf: // prepare for a led channel and value (fade:fade flashy)
				state = StateLedChannel;
				stella_fading = STELLA_SET_FLASHY;
				break;
			case 0x9f: // prepare for a led channel and value (fade:fade immediately+relative)
				state = StateLedChannel;
				stella_fading = STELLA_SET_IMMEDIATELY_RELATIVE;
				break;
			case 0x00: // reset panic counter and acknowdlegde
				sendAck();
				break;
			}
			break;
		case StateCurtainValue:
			state = StateCommands;
			curtain_setPosition(lastChar);
			break;
		case StateLedChannel:
			state = StateLedValue;
			stella_channel = lastChar;
			break;
		case StateLedValue:
			state = StateCommands;
			stella_setValue(stella_fading, stella_channel, lastChar);
			break;
		}
		// the state has not changed -> a full packet has been received -> prepare for the next one
		if (laststate==state) {
			packetPosition=0;
		}
    }
    return 0;
}
