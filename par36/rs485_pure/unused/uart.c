
/*
 * Copyright (c) 2006-2008 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "uart.h"
#include <config.h>

uint8_t address = 0;

char eeprominit[4] EEMEM = {'I','N','I','T'};
uint8_t eepromaddr EEMEM = UART_ADDRESS;

void enableOnlySender() {
	UCSRB =  (1 << TXEN) | (1 << RXCIE);
}

void enableOnlyReceiver() {
	UCSRB =  (1 << RXEN) | (1 << RXCIE);
	// Flush Receive-Buffer (entfernen evtl. vorhandener ungültiger Werte) 
	do
	{
		// UDR auslesen (Wert wird nicht verwendet) 
		UDR;
	}
	while (UCSRA & (1 << RXC));
}

/**
 * Initializes the UART.
 */
void uart_init()
{
	UBRRH    = UBRRH_VALUE;
	UBRRL    = UBRRL_VALUE;
	
	/* set frame format: 8 bit, no parity, 1 bit */
	UCSRC = (1 << UCSZ1) | (1 << UCSZ0);
	/* enable serial receiver and transmitter */
	enableOnlyReceiver();
	
	// Rücksetzen von Receive und Transmit Complete-Flags 
	UCSRA = (1 << RXC) | (1 << TXC);
	
	// enable receiving
	DDRD|=_BV(2);
	PIN_CLEAR(D,2);
	
	// check eeprom and get address
	char eeprom_verify_string[4];
	eeprom_read_block ((char*)eeprom_verify_string, (char*)eeprominit, sizeof(eeprom_verify_string));
	
	if (eeprom_verify_string[0] == 'I' && eeprom_verify_string[1] == 'N' &&
		eeprom_verify_string[2] == 'I' && eeprom_verify_string[3] == 'T')
	{
		address = eeprom_read_byte((char*)&(eepromaddr));
	}
	if (address == 0)
		address = 251;
}

void uart_putpacket_to_host(uint8_t cmd, uint8_t value) {
	if (cmd == 255 || value == 255)
		return;
	PIN_SET(D,2);
	enableOnlySender();
	for (uint8_t i=0;i<255;++i)
		asm("nop");

	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = 255; // start 
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = address; // from address 
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = 0; // to address (host==0)
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = cmd; // cmd
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = value; // value
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = 0; // end 
	loop_until_bit_is_set(UCSRA, UDRE);
	
	// Warten bis Transfer abgeschlossen
// 	UCSRA |= (1 << TXC);
// 	loop_until_bit_is_set(UCSRA, TXC);
	// Senden abschalten
	PIN_CLEAR(D,2);
	enableOnlyReceiver();
}

const struct uartPacket uart_getPacket() {
	hasPacket = 0;
	return availablePacket;
}

uint8_t uart_hasPacket() {
	return hasPacket;
}

void setAddress(uint8_t a) {
	address = a;
	eeprom_write_byte((uint8_t*)&(eepromaddr), a);
}

uint8_t getAddress() {
	return address;
}

//############################################################################
//Daten empfangen
ISR (USART_RX_vect)
//############################################################################
{
	static volatile uint8_t packetPosition = 0;
	if(UCSRA & ((1<<FE)|(1<<DOR)|(1<<UPE)) ) // frame error, overrun error, parity error
	{
		packetPosition = 0;
		return;
	}
	
	const unsigned char newbyte = UDR;
	if (newbyte == 255) {
		packetPosition = 1;
		return;
	}
	
	switch (packetPosition) {
		case 1:
			tempPacket.from_address = newbyte;
			++packetPosition;
			break;
		case 2:
			tempPacket.to_address = newbyte;
			++packetPosition;
			break;
		case 3:
			tempPacket.cmd = newbyte;
			++packetPosition;
			break;
		case 4:
			tempPacket.value = newbyte;
			++packetPosition;
			break;
		case 5:
			if (newbyte == 0 && (tempPacket.to_address == UART_BROADCAST_ADDRESS || tempPacket.to_address == address)) {
				availablePacket = tempPacket;
				hasPacket = 1;
			}
			packetPosition = 0;
		default:
			packetPosition = 0;
	};
}