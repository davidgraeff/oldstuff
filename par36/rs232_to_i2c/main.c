
#include "config.h"

#include <inttypes.h>
#include <avr/io.h>
//#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "serial/uart.h"

#include "i2cmaster.h"

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

char answer[20];
uint8_t answer_length;

uint8_t write_slave(uint8_t addr, uint8_t cmd, uint8_t value)
{
  if (i2c_start(addr + I2C_WRITE) == 1)
	  return 1;

  if (i2c_write(cmd) == 1)
	  return 1;
  if (i2c_write(value) == 1)
	  return 1;
  i2c_stop();
  return 0;
}

void detect_slaves_send_answer()
{
	answer[0] = 0xff;
	uint8_t answer_length; // = answer[1]; // length
	answer_length = 2;
	for (uint8_t addr=1;addr<112;++addr) {
		if (write_slave(addr, 0, 1) == 0) {
			answer[answer_length] = addr;
			++answer_length;
		}
	}
	answer[1] = answer_length;
	for (uint8_t i=0;i<answer_length;++i) {
		uart_putc(answer[i]);
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
	
	// Init: Subsysteme
	i2c_init();
	uart_init(UART_BAUD_SELECT(BAUD,F_CPU));
	//wdt_enable(WDTO_500MS);
	
	sei();

	uint16_t lt = 0;
	uint8_t flag = 0;
	// Hauptprogramm
	while (1) {
		//wdt_reset();
// 		char t = uart_getc();
// 		uart_putc(t==0?'a':t);
// 		 while (!(UCSRA & (1 << UDRE)));
// 		UDR = t==0?'a':t;
		if (!++lt) {
			if (flag) {
				if (flag & 0x01)
					PORTA |= _BV(6);
				else
					PORTA &= ~_BV(6);
				--flag;
			}
		}
		
		if (!uart_hasPacket())
			continue;
		
		flag = 20;
		
		struct uartPacket packet = uart_getPacket();
		if (packet.senderid==0) {
			detect_slaves_send_answer();
		} else
			write_slave(packet.senderid, packet.cmd, packet.value);
	}
	return 0;
}
