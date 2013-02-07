#pragma once
#include <stdbool.h>
#include "packet.h"

void    usiTwiSlaveInit(uint8_t ownAddress);	// send slave address
uint8_t uart_hasPacket();
struct uartPacket uart_getPacket();
void uart_consumedPacket();