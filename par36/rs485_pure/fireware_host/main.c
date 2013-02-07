#include <inttypes.h>
#include <avr/io.h>
//#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "uart.h"
#include "packet.h"

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

int main(void)
{
	// Alles TriState
	DDRB = 0xff;
	DDRD = 0;
	PORTB = 0;
	PORTD = 0;

	// Init: Subsysteme
	uart_init(UART_BAUD_SELECT(BAUD,F_CPU));
	//wdt_enable(WDTO_500MS);
	
	sei();
	
	uint16_t slower=0;
	uint16_t tasten=0;
	uint8_t i=0;
	// Hauptprogramm
	while (1) {
		if (!++tasten) {
			if (PIND & _BV(4)) {
				PIN_SET(B,6);
			}
			else if (PIND & _BV(5)) {
				PIN_SET(B,6);
			}
			else if (PIND & _BV(6)) {
				PIN_SET(B,6);
			} else {
				PIN_CLEAR(B,6);
			}
			if (!slower) {
				struct uartPacket packet;
				packet.addrinfo=UART_BROADCAST_ADDRESS;
				packet.cmd=10+0;
				packet.value=i;
				uart_writePacket(&packet);
				packet.addrinfo=UART_BROADCAST_ADDRESS;
				packet.cmd=10+1;
				packet.value=i;
				uart_writePacket(&packet);
				packet.addrinfo=UART_BROADCAST_ADDRESS;
				packet.cmd=10+2;
				packet.value=i;
				uart_writePacket(&packet);
				// 			_delay_ms(100);
				if (i & 0x01) {
					PIN_SET(B,7);
				} else {
					PIN_CLEAR(B,7);
				}
				i++;
			}
		}
		
		if (!uart_hasPacket())
			continue;
		
		struct uartPacket packet = uart_getPacket();

	}
	return 0;
}
