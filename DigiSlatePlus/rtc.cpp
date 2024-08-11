/*
 DigiSlatePlus

 DS3231 RTC class
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

#include "rtc.h"


// start RTC service
int8_t RTC::begin(uint16_t int_port) {

  // set interrupt port
  _int_port = int_port;

  // init RTC
  _rtc = new RTC_DS3231();
  _status = true;

  // try to start RTC
  if (! _rtc->begin()) {
    _status = false;
  }

  // RTC found -> activate
  else {
    // power loss?
    if (_rtc->lostPower()) {
      // _rtc->adjust(DateTime(F(__DATE__), F(__TIME__)));
      _status = -1;
    }

    // activate 1 Hz interrupt output
    _rtc->writeSqwPinMode(DS3231_SquareWave1Hz);
  }  

  return status();
}


// get DateTime
DateTime RTC::get(void) {
  return _rtc->now();
}

// set time to rtc
// h, m, s, d, m, y
void RTC::set(uint16_t  hr, uint16_t  min, uint16_t  sec, uint16_t  day, uint16_t  month, uint16_t  yr) {
    set(DateTime(yr, month, day, hr, min, sec));
}

// set DateTime
void RTC::set(DateTime dt) {
  _rtc->adjust(dt);
}

// get Temperature
float RTC::temperature(void) {
  return _rtc->getTemperature();
}


// return status
int8_t RTC::status(void) {
	return _status;
}