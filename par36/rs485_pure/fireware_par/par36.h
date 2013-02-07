
#pragma once
#include <stdint.h>

enum cmds {
	CMD_INITIALIZE = 0,
	CMD_GETDATA = 1,
	CMD_SETFADESTEP,
	
	CMD_SETLIGHTVALUE = 10,
	CMD_SETLIGHTVALUE_END = 19,
	CMD_FADELIGHTVALUE = 20,
	CMD_FADELIGHTVALUE_END = 29
};

uint8_t getAddress(void);
void setAddress(uint8_t a);
void par36_init();