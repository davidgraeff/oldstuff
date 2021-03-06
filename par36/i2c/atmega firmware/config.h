
#define STELLA_PRESCALER   		TCCR0B
#define STELLA_TIMSK       		TIMSK
#define STELLA_CS0         		CS00
#define STELLA_CS2         		CS02
#define STELLA_TOIE        		TOIE0
#define STELLA_COMPARE_IE  		OCIE0B
#define STELLA_COMPARE_VECTOR	TIMER0_COMPB_vect
#define STELLA_OVERFLOW_VECTOR  TIMER0_OVF_vect
#define STELLA_COMPARE_REG 		OCR0B

#define STELLA_OFFSET_PORT1 2
#define STELLA_PORT1 PORTB
#define STELLA_DDR_PORT1 DDRB
#define STELLA_HIGHFREQ
#define STELLA_FADE_STEP_INIT 10

#define STELLA_PORT_COUNT 1
#define STELLA_CHANNELS 3


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
