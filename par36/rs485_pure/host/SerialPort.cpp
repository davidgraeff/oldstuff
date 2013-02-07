// *****************************************************************
// This file is part of the book "Embedded Linux - Das Praxisbuch"
//
// Copyright (C) 2008-2012 Joachim Schroeder
// Chair Prof. Dillmann (IAIM),
// Institute for Computer Science and Engineering,
// University of Karlsruhe. All rights reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free
// Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
// Boston, MA 02110-1301, USA.
// *****************************************************************

// *****************************************************************
// Filename:  SerialPort.cpp
// Copyright: Joachim Schroeder, Chair Prof. Dillmann (IAIM),
//            Institute for Computer Science and Engineering (CSE),
//            University of Karlsruhe. All rights reserved.
// Author:    Joachim Schroeder, Stephan Riedel
// Date:      26.04.2008
// *****************************************************************

#include "SerialPort.h"

bool SerialPort::isOpen() {
  return (m_fd >= 0);
}

SerialPort::SerialPort(char* device, long speed, int parity, int databits, int stopbits, bool block) {

	struct termios options;
	int flags = O_RDWR | O_NOCTTY;
	if (!block) flags |= O_NDELAY;

	m_fd = open(device, flags );

	if (m_fd < 0) {
		printf ("SerialPort(): Could not open device %s.\n", device);
		return;
	}
	
	if (tcgetattr(m_fd, &options) != 0) {
		printf ("SerialPort(): Could not get terminal attributes for %s !\n", device);
		return;					// get the current options
	}
	m_options_old = options;

	switch (speed) {															// set baudrate
		case 50:
			cfsetispeed(&options, B50);
			cfsetospeed(&options, B50);
			break;
		case 75:
			cfsetispeed(&options, B75);
			cfsetospeed(&options, B75);
			break;
		case 110:
			cfsetispeed(&options, B110);
			cfsetospeed(&options, B110);
			break;
		case 134:
			cfsetispeed(&options, B134);
			cfsetospeed(&options, B134);
			break;
		case 150:
			cfsetispeed(&options, B150);
			cfsetospeed(&options, B150);
			break;
		case 200:
			cfsetispeed(&options, B200);
			cfsetospeed(&options, B200);
			break;
		case 300:
			cfsetispeed(&options, B300);
			cfsetospeed(&options, B300);
			break;
		case 600:
			cfsetispeed(&options, B600);
			cfsetospeed(&options, B600);
			break;
		case 1200:
			cfsetispeed(&options, B1200);
			cfsetospeed(&options, B1200);
			break;
		case 2400:
			cfsetispeed(&options, B2400);
			cfsetospeed(&options, B2400);
			break;
		case 4800:
			cfsetispeed(&options, B4800);
			cfsetospeed(&options, B4800);
			break;
		case 9600:
			cfsetispeed(&options, B9600);
			cfsetospeed(&options, B9600);
			break;
		case 19200:
			cfsetispeed(&options, B19200);
			cfsetospeed(&options, B19200);
			break;
		case 38400:
			cfsetispeed(&options, B38400);
			cfsetospeed(&options, B38400);
			break;
		case 57600:
			cfsetispeed(&options, B57600);
			cfsetospeed(&options, B57600);
			break;
		case 115200:
			cfsetispeed(&options, B115200);
			cfsetospeed(&options, B115200);
			break;
	}

	// set options
	options.c_iflag &= ~(ICRNL);
	options.c_iflag |= IGNBRK;

	// set controller flags
	if (parity) {
		options.c_cflag |= PARENB;													
		if (parity == 1) options.c_cflag |= PARODD;				// odd parity
		else if (parity == 2) options.c_cflag &= ~PARODD;		// even parity
	}
	else {
		options.c_cflag &= ~PARENB;									// no parity
	}

	if (stopbits == 1) {
		options.c_cflag &= ~CSTOPB;									// 1 stopbit
	}
	else if (stopbits == 2){
		options.c_cflag |= CSTOPB;										// 2 stopbits
	}

	if (databits) {
		options.c_cflag &= ~CSIZE;										// set character size
		if (databits == 5) options.c_cflag |= CS5;						// character size = 5
		else if (databits == 6) options.c_cflag |= CS6;				// =6
		else if (databits == 7) options.c_cflag |= CS7;				// =7
		else if (databits == 8) options.c_cflag |= CS8;				// =8
	}

	options.c_cflag |= (CLOCAL | CREAD);							// ignore CD-signal

	// no Echo, no control character, no interrupts
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | IEXTEN | ECHOK | ECHOCTL | ECHOKE);
	options.c_oflag &= ~(OPOST | ONLCR);							// set "raw" input
	options.c_cc[VMIN]  = 1;											// wait for at least 1 character
	options.c_cc[VTIME] = 0;											// timeout
	tcflush(m_fd,TCIOFLUSH);

	if (tcsetattr(m_fd, TCSAFLUSH, &options) != 0) {
		printf ("SerialPort(): Could not set terminal attributes for %s !\n", device);
	}
}

SerialPort::~SerialPort() {
	tcsetattr(m_fd, TCSAFLUSH, &m_options_old);							// set old options
	close(m_fd);														// close file descriptor
}

int SerialPort::writeData(char* buf, int num) {

	return write(m_fd, buf, num);
}

int SerialPort::readData(char* buf, int num) {
	
	return read(m_fd, buf, num);
}





