/****************************************************************************
** This file is part of the linux remotes project
**
** Use this file under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
****************************************************************************/
#include "driver-interface-infrared.h"
#include <sstream> //for converting strings to int
#include <iostream>
#include <bitset>

namespace liri {

/* Currently only supports rc5 and nec protocoll.
   Eleminates all togglebits and similar and just use the data bits.
   To save memory data bits are composited to bytes. */
bool IDriverInfrared::decode_infrared(liri::KeyCode& target, char* input_bits, int length) {
	/* TODO repeat last key, used by NEC */
	if (length == 3 && input_bits[0] == LIRI_INFRARED_BURST && input_bits[1] == LIRI_INFRARED_HIGH) {
		return false;
	}

	/* min length of a rc5 or nec command */
	if (length < 5) return false;

	// if rc5 take all bits but the togglebits (the 2nd/4th bit)
	if (length > 20 && length < 28) {
		input_bits[2] = LIRI_INFRARED_LOW;
		input_bits[4] = LIRI_INFRARED_LOW;
	}
	// if NEC take all but the last inverted data bits ~8
	else if (length > 30) {
		length -= 8;
	}

	/* set code length */
	target.keycodeLen = length / 8 + 1;
	if (target.keycodeLen > LIRI_KEYCODE_LENGTH)
		target.keycodeLen = LIRI_KEYCODE_LENGTH;

	/* encode bits to byte */
	std::bitset<8> bs;
	int offset;

	//std::cout << "decode_infrared raw Länge: " << length << " encoded Länge: " << target.keycodeLen << " Daten: ";
	for (unsigned int c=0;c<target.keycodeLen;++c) {
		bs.reset();
		offset = c*8;
		for( int i = 0; i < 8; ++i ) {
			if (offset+i>=length) break;
			bs.set( i, input_bits[offset+i]);
			/*
			switch (input_bits[offset+i]) {
				case LIRI_INFRARED_LOW:
				std::cout << "L ";
				break;
				case LIRI_INFRARED_HIGH:
				std::cout << "H ";
				break;
				case LIRI_INFRARED_BURST:
				std::cout << "B ";
				break;
				default:
				break;
			}
			*/
		}
		target.keycode[c] = char( bs.to_ulong() );
		//std::cout << (int)(unsigned char)target.keycode[c] << " ";
	}
	//std::cout << std::endl;
	return true;
}

} //end namespace

