/*
 DigiSlatePlus

 MAX7219A LED-driver class header
 */

/*
This Source Code Form is subject to the terms of the
Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2024 Thomas Winkler

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 The class is a framework to display values on the LED 7-segment display.
 The assignment of the data to the segments is defined in the config.h file.
 */


#ifndef LED_H
#define LED_H

#include "tc.h"


//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP   0x00
#define OP_DIGIT0 0x01
#define OP_DIGIT1 0x02
#define OP_DIGIT2 0x03
#define OP_DIGIT3 0x04
#define OP_DIGIT4 0x05
#define OP_DIGIT5 0x06
#define OP_DIGIT6 0x07
#define OP_DIGIT7 0x08
#define OP_DECODEMODE  0x09
#define OP_INTENSITY   0x0A
#define OP_SCANLIMIT   0x0B
#define OP_SHUTDOWN    0x0C
#define OP_DISPLAYTEST 0x0F


class LED {
public:

	// start led class
	// count = number of cascading controllers
	void begin(SPIClass*, uint8_t, uint8_t count);
	void set(TIMECODE);
	void set(uint8_t, uint8_t, uint8_t, uint8_t);
	void frames(uint8_t);
	void seconds(uint8_t);
	void minutes(uint8_t);
	void hours(uint8_t);

	// set/clear decimal point
	void dp(uint8_t digit, bool status);

	// tick with decimal point
	void tick(uint8_t digit);
	

private:
	uint8_t _load_pin;

	void _write(uint8_t, uint8_t);
	void _digits(uint8_t, uint8_t, uint8_t);

	uint8_t _i;
	uint8_t _count;

	/* The array for shifting the data to the devices */
	byte _spidata[16];

	SPIClass* _spi = NULL;
};


#endif