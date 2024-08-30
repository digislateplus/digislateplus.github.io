/*
 DigiSlatePlus

 WC1602 LC-display class header
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
 The class provides a framwork to display text and integer values on the
 LC-display. Special texts and values can directly accessed without thinking
 about the correct place on the display.
 */


#ifndef LCD_H
#define LCD_H

#include <LiquidCrystal.h>  // for our 16x2 display
#include <SPI.h>


class LCD {

public:
	void begin(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

	// special display values
	void status(char*);
	void dir(bool);
	void fps(uint8_t);
	void ubits(char*);

	// generic display
	void val8(uint8_t, uint8_t, uint8_t);
	void val16(uint16_t, uint8_t, uint8_t);
	void val32(uint32_t, uint8_t, uint8_t);
	void print(char*, uint8_t, uint8_t);

	void clear(void);

private:
	LiquidCrystal* _disp;
	char* _arroz[2];
};


#endif