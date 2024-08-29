/*
 DigiSlatePlus

 LED class
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

#include "rled.h"


void RLED::begin(uint16_t port) {
	_port = port;
	pinMode(port, OUTPUT),
	off();
}


void RLED::set(bool val) {
	digitalWrite(_port, val);
}

void RLED::on(void) {
	set(HIGH);
}

void RLED::off(void) {
	set(LOW);
}

void RLED::toggle(void) {
	set(!digitalRead(_port));
}

void RLED::flash(void) {
	flash(30);
}

void RLED::flash(uint16_t time) {
	flash(time, 1);
}

void RLED::flash(uint16_t time, uint8_t repeat) {

	for (uint8_t i = 0; i < repeat; i++) {
		on();
		delay(time);
		off();
		delay(time);
	}
}