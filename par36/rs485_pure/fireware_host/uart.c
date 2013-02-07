/*************************************************************************
Title:    Interrupt UART library with receive/transmit circular buffers
Author:   Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:     $Id: uart.c,v 1.6.2.1 2007/07/01 11:14:38 peter Exp $
Software: AVR-GCC 4.1, AVR Libc 1.4.6 or higher
Hardware: any AVR with built-in UART, 
License:  GNU General Public License 
          
DESCRIPTION:
    An interrupt is generated when the UART has finished transmitting or
    receiving a byte. The interrupt handling routines use circular buffers
    for buffering received and transmitted data.
    
    The UART_RX_BUFFER_SIZE and UART_TX_BUFFER_SIZE variables define
    the buffer size in bytes. Note that these variables must be a 
    power of 2.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"

#if  defined(__AVR_ATmega8__)  || defined(__AVR_ATmega16__) || defined(__AVR_ATmega32__) \
  || defined(__AVR_ATmega323__)
  /* ATmega with one USART */
 #define ATMEGA_USART
 #define UART0_RECEIVE_INTERRUPT   USART_RXC_vect
 #define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
 #define UART0_STATUS   UCSRA
 #define UART0_CONTROL  UCSRB
 #define UART0_DATA     UDR
 #define UART0_UDRIE    UDRIE
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) 
 /* ATmega with two USART */
 #define ATMEGA_USART0
 #define ATMEGA_USART1
 #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
 #define UART1_RECEIVE_INTERRUPT   USART1_RX_vect
 #define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
 #define UART1_TRANSMIT_INTERRUPT  USART1_UDRE_vect
 #define UART0_STATUS   UCSR0A
 #define UART0_CONTROL  UCSR0B
 #define UART0_DATA     UDR0
 #define UART0_UDRIE    UDRIE0
 #define UART1_STATUS   UCSR1A
 #define UART1_CONTROL  UCSR1B
 #define UART1_DATA     UDR1
 #define UART1_UDRIE    UDRIE1
#elif defined(__AVR_ATmega48__) ||defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || \
      defined(__AVR_ATmega48P__) ||defined(__AVR_ATmega88P__) || defined(__AVR_ATmega168P__) || \
      defined(__AVR_ATmega328P__) 
 /* TLS-Added 48P/88P/168P/328P */
 /* ATmega with one USART */
 #define ATMEGA_USART0
 #define UART0_RECEIVE_INTERRUPT   USART_RX_vect
 #define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
 #define UART0_TRANSMIT_COMPLETE_INTERRUPT  USART_TXCE_vect
 #define UART0_STATUS   UCSR0A
 #define UART0_CONTROL  UCSR0B
 #define UART0_DATA     UDR0
 #define UART0_UDRIE    UDRIE0
#elif defined(__AVR_ATtiny2313__)
 #define ATMEGA_USART
 #define UART0_RECEIVE_INTERRUPT   USART_RX_vect 
 #define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
 #define UART0_STATUS   UCSRA
 #define UART0_CONTROL  UCSRB
 #define UART0_DATA     UDR
 #define UART0_UDRIE    UDRIE
#elif defined(__AVR_ATmega3290__) || defined(__AVR_ATmega6490__)
  /* TLS-Separated these two from the previous group because of inconsistency in the USART_RX */
  /* ATmega with one USART */
  #define ATMEGA_USART0
  #define UART0_RECEIVE_INTERRUPT   USART_RX_vect
  #define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
  #define UART0_STATUS   UCSR0A
  #define UART0_CONTROL  UCSR0B
  #define UART0_DATA     UDR0
  #define UART0_UDRIE    UDRIE0
#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega640__)
/* ATmega with two USART */
  #define ATMEGA_USART0
  #define ATMEGA_USART1
  #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
  #define UART1_RECEIVE_INTERRUPT   USART0_UDRE_vect
  #define UART0_TRANSMIT_INTERRUPT  USART1_RX_vect
  #define UART1_TRANSMIT_INTERRUPT  USART1_UDRE_vect
  #define UART0_STATUS   UCSR0A
  #define UART0_CONTROL  UCSR0B
  #define UART0_DATA     UDR0
  #define UART0_UDRIE    UDRIE0
  #define UART1_STATUS   UCSR1A
  #define UART1_CONTROL  UCSR1B
  #define UART1_DATA     UDR1
  #define UART1_UDRIE    UDRIE1  
#elif defined(__AVR_ATmega644__)
 /* ATmega with one USART */
 #define ATMEGA_USART0
 #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
 #define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
 #define UART0_STATUS   UCSR0A
 #define UART0_CONTROL  UCSR0B
 #define UART0_DATA     UDR0
 #define UART0_UDRIE    UDRIE0
#elif defined(__AVR_ATmega164P__) || defined(__AVR_ATmega324P__) || defined(__AVR_ATmega644P__)
 /* ATmega with two USART */
 #define ATMEGA_USART0
 #define ATMEGA_USART1
 #define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
 #define UART1_RECEIVE_INTERRUPT   USART0_UDRE_vect
 #define UART0_TRANSMIT_INTERRUPT  USART1_RX_vect
 #define UART1_TRANSMIT_INTERRUPT  USART1_UDRE_vect
 #define UART0_STATUS   UCSR0A
 #define UART0_CONTROL  UCSR0B
 #define UART0_DATA     UDR0
 #define UART0_UDRIE    UDRIE0
 #define UART1_STATUS   UCSR1A
 #define UART1_CONTROL  UCSR1B
 #define UART1_DATA     UDR1
 #define UART1_UDRIE    UDRIE1
#else
 #error "no UART definition for MCU available"
#endif

volatile uint8_t hasPacket;
volatile unsigned char lastChar;
volatile unsigned char haslastChar;
struct uartPacket availablePacket, tempPacket;

const struct uartPacket uart_getPacket() {
	hasPacket = 0;
	return availablePacket;
}

uint8_t uart_hasPacket() {
	return hasPacket;
}

uint8_t uart_hasLastChar(void) {
	return haslastChar;
}

unsigned char uart_getc(void) {
	haslastChar = 0;
	return lastChar;
}

ISR(UART0_RECEIVE_INTERRUPT)
/*************************************************************************
Function: UART Receive Complete interrupt
Purpose:  called when the UART has received a character
**************************************************************************/
{
	static volatile uint8_t packetPosition = 0;
	if(UCSRA & ((1<<FE)|(1<<DOR)|(1<<UPE)) ) // frame error, overrun error, parity error
	{
		packetPosition = 0;
		return;
	}

	const unsigned char temp = haslastChar ? lastChar : 0;
	haslastChar = 1;
	lastChar = UDR;
	if (lastChar == 255 && temp == 255) { // two proceeding 255-values start a valid packet
		packetPosition = 1;
		return;
	}
	
	switch (packetPosition) {
		case 1:
			tempPacket.addrinfo = lastChar;
			++packetPosition;
			break;
		case 2:
			tempPacket.cmd = lastChar;
			++packetPosition;
			break;
		case 3:
			tempPacket.value = lastChar;
			++packetPosition;
			break;
		case 4:
			packetPosition = 0;
			if (lastChar != 0) // need a tailing 0 for a valid packet
				break;

			availablePacket = tempPacket;
			hasPacket = 1;
		default:
			packetPosition = 0;
	};
}

void uart_writePacket(struct uartPacket* p) {
		uart_mode_send();
		uint16_t wait=0;
		while(++wait);
		while (!(UART0_STATUS & (1<<UDRE)));
		UART0_DATA = 0xff;
		while (!(UART0_STATUS & (1<<UDRE)));
		UART0_DATA = 0xff;
		while (!(UART0_STATUS & (1<<UDRE)));
		UART0_DATA = p->addrinfo;
		while (!(UART0_STATUS & (1<<UDRE)));
		UART0_DATA = p->cmd;
		while (!(UART0_STATUS & (1<<UDRE)));
		UART0_DATA = p->value;
		while (!(UART0_STATUS & (1<<UDRE)));
		UART0_DATA = 0;
		while (!(UART0_STATUS & (1<<UDRE)));
// 		while(++wait);
		uart_mode_receive();
}

/*************************************************************************
Function: uart_init()
Purpose:  initialize UART and set baudrate
Input:    baudrate using macro UART_BAUD_SELECT()
Returns:  none
**************************************************************************/
void uart_init(unsigned int baudrate)
{
    /* Set baud rate */
    if ( baudrate & 0x8000 )
    {
    	 UART0_STATUS = (1<<U2X);  //Enable 2x speed 
    	 baudrate &= ~0x8000;
    }
    UBRRH = (unsigned char)(baudrate>>8);
    UBRRL = (unsigned char) baudrate;
   
    /* Enable USART receiver and transmitter and receive complete interrupt */
    UART0_CONTROL = _BV(RXCIE)|(1<<RXEN)|(1<<TXEN);
    
    /* Set frame format: asynchronous, 8data, no parity, 1stop bit */
    UCSRC = (1 << UCSZ1) | (1 << UCSZ0);
}/* uart_init */

inline void uart_mode_receive() {
	PIN_CLEAR(B,0);
}

inline void uart_mode_send() {
	PIN_SET(B,1);
}