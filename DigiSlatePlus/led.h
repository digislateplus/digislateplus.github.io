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
 The assignment of the data to the segments is defined in the setup.h file.
 */


#ifndef LED_H
#define LED_H

#include "tc.h"


class LED {
public:

	void begin(uint8_t);
	void set(TIMECODE);
	void set(uint8_t, uint8_t, uint8_t, uint8_t);
	void frames(uint8_t);
	void seconds(uint8_t);
	void minutes(uint8_t);
	void hours(uint8_t);

private:
	uint8_t _load_pin;
	void _write(uint8_t, uint8_t);
	void _digits(uint8_t, uint8_t, uint8_t);
};


#endif