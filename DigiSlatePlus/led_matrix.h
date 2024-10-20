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
 The class is a framework to display values on the LED 8x8 dot display.
 The assignment of the data to the segments is defined in the config.h file.
 */


#ifndef LED_MATRIX_H
#define LED_MATRIX_H


#include <Arduino.h>
#include <SPI.h>

#include "config.h"
#include "led.h"
#include "charset.h"


class LED_MATRIX {

public:
	void begin(SPIClass*, uint8_t width, uint8_t height);
	void add_controller(uint8_t port);

	void home(void);
	void clear(void);
	void clear(uint8_t);	

	void print(char*, uint8_t);
	void print(char*, uint8_t, uint8_t);

	void chr(uint8_t, uint8_t);

private:
	void _send(uint8_t addr, uint8_t opcode, uint8_t data);

	uint8_t _i;
	
	uint8_t _count;

	uint8_t _width;
	uint8_t _height;

	uint8_t _cursor;
	uint8_t _spidata[64];


	SPIClass* _spi;

	uint8_t _controller_count;
	uint8_t _port[MATRIX_CONTROLLERS];
};


#endif