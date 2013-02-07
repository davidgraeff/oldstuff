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
// Filename:  SerialPort.h
// Copyright: Joachim Schroeder, Chair Prof. Dillmann (IAIM),
//            Institute for Computer Science and Engineering (CSE),
//            University of Karlsruhe. All rights reserved.
// Author:    Joachim Schroeder, Stephan Riedel
// Date:      26.04.2008
// *****************************************************************

#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <iostream>

//! Class handling the serial port.
class SerialPort {

	public:
		SerialPort(char* device, long speed, int parity, int databits, int stopbits, bool block);
		~SerialPort();
		int writeData(char* buf, int num);       //!< write buffer, number of bytes
		int readData(char* buf, int num);        //!< read buffer, number of bytes
		bool isOpen();
	private:
		int m_fd;                                //!< file descriptor
		struct termios m_options_old;            //!< previous options for serial port
};

#endif /* _SERIALPORT_H_ */





