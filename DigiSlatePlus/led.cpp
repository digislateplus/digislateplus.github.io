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


void LED::begin(uint8_t load_pin) {

	_load_pin = load_pin;

	pinMode(_load_pin, OUTPUT);
	digitalWrite(_load_pin, HIGH);

	// =============================================================
	// INIT LED-Display
	SPI.setBitOrder(MSBFIRST);  //MSB first
	SPI.begin();

	delay(200);

	//test by turning on, then off
	_write(0x0F, 0x01);
	delay(200);
	_write(0x0F, 0x00);

	_write(0x09, 0xFF);  //enable onboard bit decode (Mode B)
	_write(0x0A, 0x0F);  //max intensity
	_write(0x0B, 0x07);  //display all digits
	_write(0x0C, 0x01);  //turn on chip
}


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


/*  Write VALUE to register ADDRESS on the MAX7219. */
void LED::_write(uint8_t address, uint8_t value) {

	digitalWrite(_load_pin, LOW); //Toggle enable pin to load MAX7219 shift register
	SPI.transfer(address);
	SPI.transfer(value);
	digitalWrite(_load_pin, HIGH); 
}


void LED::_digits(uint8_t val, uint8_t ten, uint8_t one) {
	_write(ten, (val / 10) | '0');
	_write(one, (val % 10) | '0');
}