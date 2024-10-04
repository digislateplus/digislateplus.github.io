/*
 DigiSlatePlus

 MAX7219A LED-driver class
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

#include <Arduino.h>
#include <SPI.h>

#include "setup.h"

#include "led.h"
#include "tc.h"


void LED::begin(SPIClass* spi, uint8_t load_pin, uint8_t type, uint8_t count) {

	uint8_t i;

	_spi = spi;
	_load_pin = load_pin;
	_type = type;
	_count = count;

	pinMode(_load_pin, OUTPUT);
	digitalWrite(_load_pin, HIGH);

	// =============================================================
	// INIT LED-Display
	#ifdef DEBUG
		Serial.println("-----------------------------");
		Serial.print("init type ");
		Serial.print(type);
		Serial.print(" LED on SPI bus ");
		Serial.print(SPI_BUS);
		Serial.print(" load port ");
		Serial.println(_load_pin);
	#endif

	_spi->setBitOrder(MSBFIRST);  //MSB first
	_spi->begin(SCK, MISO, MOSI, _load_pin);

	delay(200);


	//test by turning on, then off
	for(i = 0; i < _count; i++) {


		#ifdef DEBUG
			Serial.print("init registers of MAX7219 #");
			Serial.println(i);
		#endif


		// displaytest
		_send(i, OP_DISPLAYTEST, 0x01);

		// clearDisplay(i);
		_send(i, OP_INTENSITY, 0x0F);  //max intensity

		//scanlimit is set to max on startup
		_send(i, OP_SCANLIMIT,0x07);


		// set decode by type
		switch (_type) {

			case LED_TYPE_7_SEGMENT:

				_send(i, OP_DECODEMODE, 0xFF);  //enable onboard bit decode (Mode B)

				break;

			case LED_TYPE_MATRIX:

				_send(i, OP_DECODEMODE, 0x00);  //disable onboard bit decode (Mode B)
				break;
		}

	}

	delay(750);


	//we go into shutdown-mode on startup
	for(i = 0; i < _count; i++) {
		_send(i, OP_SHUTDOWN, 0x01);  //turn on chip
	}


	// _write(OP_DISPLAYTEST, 0x01);
	// _write(OP_DISPLAYTEST, 0x00);
}


// void LED::_test_patter(uint8_t addr) {

	// _send(addr, )
// }

void LED::set(TIMECODE tc) {
	set(tc.h, tc.m, tc.s, tc.f);
}

void LED::set(uint8_t h, uint8_t m, uint8_t s, uint8_t f) {
	hours(h);
	minutes(m);
	seconds(s);
	frames(f);
}


void LED::frames(uint8_t val) {
	_digits(val, LED_F10, LED_F1);
}

void LED::seconds(uint8_t val) {
	_digits(val, LED_S10, LED_S1);
}

void LED::minutes(uint8_t val) {
	_digits(val, LED_M10, LED_M1);
}

void LED::hours(uint8_t val) {
	_digits(val, LED_H10, LED_H1);
}


// send count bytes
void LED::_send(uint8_t addr, uint8_t opcode, uint8_t data) {

	//Create an array with the data to shift out
	int offset = addr*2;
	int maxbytes = _count * 2;

	// clear output data
	for(_i = 0; _i < _count; _i++) {
		_spidata[_i] = (byte)0;
	}

	//put our device data into the array
	_spidata[offset+1] = opcode;
	_spidata[offset] = data;

	//enable the line 
	digitalWrite(_load_pin,LOW);

	//Now shift out the data 
	for(_i = _count; _i > 0; _i--) {
	    shiftOut(_load_pin, SCK, MSBFIRST, _spidata[_i-1]);
	}

	//latch the data onto the display
	digitalWrite(_load_pin,HIGH); 
}


/*  Write VALUE to register ADDRESS on the MAX7219. */
void LED::_write(uint8_t address, uint8_t value) {

	digitalWrite(_load_pin, LOW); //Toggle enable pin to load MAX7219 shift register
	_spi->transfer(address);
	_spi->transfer(value);
	digitalWrite(_load_pin, HIGH); 
}


void LED::_digits(uint8_t val, uint8_t ten, uint8_t one) {
	_write(ten, (val / 10) | '0');
	_write(one, (val % 10) | '0');
}


// draw character at x-position (0-64)
void LED::chr(uint8_t chr, uint8_t pos) {

}

// print text starting at x = 0
void LED::print(char* text, uint8_t length) {
	print(text, length, 0);
}


// print text starting at position (0-64)
void LED::print(char* text, uint8_t length, uint8_t pos) {

	Serial.print("print to matrix: ");

	for (_i = 0;_i < length; _i++) {
		Serial.print(text[_i]);
	}

	// display colums
	// MSB = down
	_send(0, OP_DIGIT0, B00011111);
	_send(0, OP_DIGIT1, B00100100);
	_send(0, OP_DIGIT2, B01000100);
	_send(0, OP_DIGIT3, B00100100);
	_send(0, OP_DIGIT4, B00011111);
	_send(0, OP_DIGIT5, B00000000);
	_send(0, OP_DIGIT6, B00000000);
	_send(0, OP_DIGIT7, B00000000);

	Serial.println();

}


// set row of display
void LED::_set_row(uint8_t address, uint8_t row, uint8_t data) {

}