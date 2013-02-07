Firmware for an Atmel Attiny2313 µPC featuring
 - soft pwm for up to 4 pwm channels
 - uart sending via ringbuffer
 - basic protocol to set pwm light values via uart

Build:
- Use make to build the firmware (gcc-avr + avr-libc needed)

Linux Transfer Script:
- Use make flash or make flashWithEEprom to download the firmware to your ISP flashable avr device via avrdude/usb
