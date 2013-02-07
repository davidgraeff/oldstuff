
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>

#include <util/delay.h>

#include "usbdrv/usbdrv.h"
#include "usbdrv/oddebug.h"

#define VERSION_MAJOR 1
#define VERSION_MINOR 8
#define VERSION_STR "1.08"
// EEMEM wird bei aktuellen Versionen der avr-lib in eeprom.h definiert
// hier: definiere falls noch nicht bekannt ("alte" avr-libc)
#ifndef EEMEM
// alle Textstellen EEMEM im Quellcode durch __attribute__ ... ersetzen
#define EEMEM  __attribute__ ((section (".eeprom")))
#endif
 
/* ------------------------------------------------------------------------- */

unsigned char	usbFunctionSetup(unsigned char data[8]) {
  static unsigned char replyBuf[4];
  usbMsgPtr = replyBuf;
  unsigned char len = (data[1] & 3)+1;       // 1 .. 4 bytes 
  unsigned char target = (data[1] >> 3) & 3; // target 0 .. 3
  unsigned char i;

  // request byte:

  // 7 6 5 4 3 2 1 0
  // C C C T T R L L

  // TT = target bit map 
  // R = reserved for future use, set to 0
  // LL = number of bytes in transfer - 1 

  switch(data[1] >> 5) {

  case 0: // echo (for transfer reliability testing)
    replyBuf[0] = data[2];
    replyBuf[1] = data[3];
    return 2;
    break;
    
  case 1: // command
//     target &= controller;  // mask installed controllers
// 
//     if(target) // at least one controller should be used ...
//       for(i=0;i<len;i++)
// 	lcd_command(target, data[2+i]);
    break;

  case 2: // data
//     target &= controller;  // mask installed controllers
// 
//     if(target) // at least one controller should be used ...
//       for(i=0;i<len;i++)
// 	lcd_data(target, data[2+i]);
    break;

  case 3: // set
    switch(target) {

    case 0:  // contrast
//       set_contrast(data[2]);
      break;

    case 1:  // brightness
//       set_brightness(data[2]);
      break;

    default:
      // must not happen ...
      break;      
    }
    break;

  case 4: // get
    switch(target) {
    case 0: // version
      replyBuf[0] = VERSION_MAJOR;
      replyBuf[1] = VERSION_MINOR;
      return 2;
      break;

    case 1: // keys
      replyBuf[0] = ((PINC & _BV(5))?0:1) | 
	            ((PINB & _BV(0))?0:2);
      replyBuf[1] = 0;
      return 2;
      break;

    case 2: // controller map
      replyBuf[0] = 0;
      replyBuf[1] = 0;
      return 2;
      break;      

    default:
      // must not happen ...
      break;      
    }
    break;

  default:
    // must not happen ...
    break;
  }

  return 0;  // reply len
}

/* ------------------------------------------------------------------------- */

int	main(void) {
  wdt_enable(WDTO_1S);

  /* all outputs except INT0 and RxD/TxD */
  USBDDR = ~USBMASK;	   /* all outputs except USB data */
  USBOUT = 0;
  usbDeviceDisconnect();

  /* USB Reset by device only required on Watchdog Reset */
  _delay_loop_2(40000);   // 10ms

  usbDeviceConnect();
  usbInit();

  sei();
  for(;;) {	/* main event loop */
    wdt_reset();
    usbPoll();
  }
  return 0;
}

/* ------------------------------------------------------------------------- */
