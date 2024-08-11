/*
 DigiSlatePlus

 DS3231 RTC class header
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
 Access the rtc clock.
 */


#ifndef RTC_H
#define RTC_H

#include "RTClib.h"

/*
 Date and time functions using a DS3231 RTC connected via I2C and Wire lib

 class reference
 https://adafruit.github.io/RTClib/html/class_r_t_c___d_s3231.html
 */


#define RTC_ONLINE 0
#define RTC_POWER_LOSS 1
#define RTC_OK 1

class RTC {

public:
	int8_t begin(uint16_t int_port);
	DateTime get(void);
	void set(uint16_t hr, uint16_t min, uint16_t sec, uint16_t day, uint16_t month, uint16_t yr);
	void set(DateTime);

	float temperature(void);
	int8_t status(void);

private:
	RTC_DS3231* _rtc;
	uint8_t _status;
	uint16_t _int_port;
};

#endif