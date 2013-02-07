
#include "config.h"

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> // sei
#include <avr/wdt.h>

#include "softpwm/stella.h"
#include "par36.h"
#include "usiTwiSlave.h"


#define soft_reset()        \
do                          \
{                           \
wdt_enable(WDTO_15MS);  \
for(;;)                 \
{                       \
}                       \
} while(0)


// // Function Pototype
// void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));
// 
// // Function Implementation
// void wdt_init(void)
// {
// 	MCUSR = 0;
// 	wdt_disable();
// 	return;
// }


uint8_t debugSendAddress = 0;
uint8_t testlights = 0;
static void testlight(void) {
	static uint8_t counter;
	static uint8_t value;
	if (!testlights)
		return;
	
	if (++counter)
		return;
	
	if (testlights==1) {
		if (++value==255) {
			testlights = 2;
		}
	} else {
		if (--value==0)
			testlights = 1;
	}
	stella_setValue(STELLA_SET_IMMEDIATELY, 0, value);
	stella_setValue(STELLA_SET_IMMEDIATELY, 1, value);
	stella_setValue(STELLA_SET_IMMEDIATELY, 2, value);
}

int main(void)
{
	// Alles TriState
	DDRA = 0;
	DDRB = 0;
	DDRD = 0;
	PORTA = 0;
	PORTB = 0;
	PORTD = 0;
	
	// Init: Subsysteme
	par36_init();
	usiTwiSlaveInit(getAddress());	// TWI slave init
	stella_init();
	//wdt_enable(WDTO_500MS);
	
	sei();
	
	stella_setValue(STELLA_SET_IMMEDIATELY, 2, 112);

// 	uint8_t echovalue = 0;
	
	// Hauptprogramm
	while (1) {
		//wdt_reset();
		testlight();
		stella_process();

		if (!uart_hasPacket())
			continue;
		
		struct uartPacket packet = uart_getPacket();

// 		echovalue = packet.value;
		if (packet.cmd == CMD_INITIALIZE) {
			stella_setValue(STELLA_SET_IMMEDIATELY, 0, 0);
			stella_setValue(STELLA_SET_IMMEDIATELY, 1, 0);
			stella_setValue(STELLA_SET_IMMEDIATELY, 2, 0);
			testlights = packet.value;
		} else if (packet.cmd == CMD_GETDATA) {
			
		} else if (packet.cmd == CMD_SETFADESTEP) {
			stella_setFadestep(packet.value);
		} else if (packet.cmd >= CMD_SETLIGHTVALUE && packet.cmd <= CMD_SETLIGHTVALUE_END) {
			stella_setValue(STELLA_SET_IMMEDIATELY, packet.cmd-CMD_SETLIGHTVALUE, packet.value);
		} else if (packet.cmd >= CMD_FADELIGHTVALUE && packet.cmd <= CMD_FADELIGHTVALUE_END) {
			stella_setValue(STELLA_SET_FADE, packet.cmd-CMD_FADELIGHTVALUE, packet.value);
		}

		uart_consumedPacket();
		
	}
	return 0;
}
