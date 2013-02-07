#include "par36.h"

#include <avr/eeprom.h>

uint8_t address = 0;
char eepromdata[5] EEMEM = {'I','N','I','T', I2C_ADDRESS};

void par36_init()
{
	// check eeprom and get address
	char eeprom_verify_string[sizeof(eepromdata)];
	eeprom_read_block ((char*)eeprom_verify_string, (char*)eepromdata, sizeof(eeprom_verify_string));
	
	if (eeprom_verify_string[0] == 'I' && eeprom_verify_string[1] == 'N' &&
		eeprom_verify_string[2] == 'I' && eeprom_verify_string[3] == 'T')
	{
		address = eeprom_verify_string[4];
	}
	if (address == 0)
		address = I2C_ADDRESS;
}

uint8_t getAddress(void) {
	return address;
}

void setAddress(uint8_t a) {
	address = a;
	eeprom_write_byte((uint8_t*)&(eepromdata[4]), a);
}
