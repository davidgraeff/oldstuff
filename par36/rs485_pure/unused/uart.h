
/*
 * Copyright (c) 2006-2008 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

/**
 * \addtogroup arch
 *
 * @{
 */
/**
 * \addtogroup arch_uart
 *
 * @{
 */
/**
 * \file
 * UART header (license: GPLv2)
 *
 * \author Roland Riegel
 */

enum cmds {
    CMD_SETLIGHT0VALUE = 1,
    CMD_SETLIGHT1VALUE,
    CMD_SETLIGHT2VALUE,
    
    CMD_SETLIGHT0ON,
    CMD_SETLIGHT1ON,
    CMD_SETLIGHT2ON,
    
    CMD_FADELIGHT0VALUE,
    CMD_FADELIGHT1VALUE,
    CMD_FADELIGHT2VALUE,
    
    CMD_FADELIGHT0ON,
    CMD_FADELIGHT1ON,
    CMD_FADELIGHT2ON,
    
    CMD_GETFADESTEP,
    CMD_SETFADESTEP,
    
    CMD_GETADDRESS,
    CMD_SETADDRESS,
    
    CMD_TESTLIGHTS_ON,
    CMD_TESTLIGHTS_OFF,
	
	CMD_ACK
    
};

struct uartPacket {
    uint8_t from_address;
    uint8_t to_address;
    uint8_t cmd;
    uint8_t value;
};

void uart_init();
void uart_putpacket_to_host(uint8_t cmd, uint8_t value);
const struct uartPacket uart_getPacket();
uint8_t uart_hasPacket();

void setAddress(uint8_t a);
uint8_t getAddress();

void enableOnlySender();
void enableReceiverSender();
/**
 * @}
 * @}
 */

#endif

