/*
 DigiSlatePlus

 WC1602 LC-display class
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

#include "lcd.h"


void LCD::begin(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {

	const char arr0[] = {0x20, 0x7E, 0x20, 0};    // forward arrow
	const char arr1[] = {0x20, 0x7F, 0x20, 0};    // reverse arrow
	
	_arroz[0] = (char*)arr0;
	_arroz[1] = (char*)arr1;

	//   rs  en  d4 d5 d6 d7;
	_disp = new LiquidCrystal(rs, en, d4, d5, d6, d7);

	// =============================================================
	// display startup text on LCD
	_disp->begin(16, 2);
	_disp->clear();
}

void LCD::status(char* status) {
	// _disp->setCursor(12, 0);
	// _disp->print(status);
	print(status, 12, 0);
}


void LCD::dir(bool dir) {
	_disp->setCursor(0, 0);
	_disp->write(_arroz[dir]);
}

void LCD::fps(uint8_t fps) {
	val8(fps, 3, 0);
	_disp->print(" fps");
}

void LCD::ubits(char* uBits) {
	_disp->setCursor(0,1);
	_disp->print("UB:");
	_disp->print(uBits);
}


// display text at x, y
void LCD::print(char* text, uint8_t x, uint8_t y) {
	_disp->setCursor(x, y);
	_disp->print(text);
}

// display byte value at x, y
void LCD::val8(uint8_t val, uint8_t x, uint8_t y) {
	_disp->setCursor(x, y);
	_disp->print("   ");
	_disp->setCursor(x, y);
	_disp->print(val);
}

// display long value at x, y
void LCD::val16(uint16_t val, uint8_t x, uint8_t y) {
	_disp->setCursor(x, y);
	_disp->print("     ");
	_disp->setCursor(x, y);
	_disp->print(val);
}

// display int value at x, y
void LCD::val32(uint32_t val, uint8_t x, uint8_t y) {
	_disp->setCursor(x, y);
	_disp->print("          ");
	_disp->setCursor(x, y);
	_disp->print(val);
}


// clear display
void LCD::clear(void) {
	_disp->clear();
}