
#include <util/setbaud.h>
//115200 57600
//#define UART_BUFFER  80 

#define HEX__(n) 0x##n##LU 
/* 8-bit conversion function */ 
#define B8__(x) ((x&0x0000000FLU)?1:0)     \
+((x&0x000000F0LU)?2:0)     \
+((x&0x00000F00LU)?4:0)     \
+((x&0x0000F000LU)?8:0)     \
+((x&0x000F0000LU)?16:0)    \
+((x&0x00F00000LU)?32:0)    \
+((x&0x0F000000LU)?64:0)    \
+((x&0xF0000000LU)?128:0) 
#define B8(d) ((unsigned char)B8__(HEX__(d))) 

/*#define _PORT_CHAR(character) PORT ## character
#define PORT_CHAR(character) _PORT_CHAR(character)*/
#define PIN_CLEAR(port,pin) (PORT ## port) &= ~_BV(pin)
#define PIN_SET(port,pin) (PORT ## port) |= _BV(pin)
