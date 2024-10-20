/*
 DigiSlatePlus

 SMPTE/EBU Timecode class header
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
 The class stores the current timecode data.

 Calling the inc() method, the timecode is incremented by one frame.
 This method is called by a timer interrupt, that occurs every 1/2 bit
 (24 fps => 3840 Hz, 25 fps => 4000 Hz, 30 fps => 4800 Hz).

 The time sending a second of timecode by the timer is measured and
 returned by the inc() class to synchronice the timer speed with the
 realtime clock speed (done in the main routine).

 The update_binary() method writes the data to the 10 byte binary array
 in correct order for direct output.
 */


#ifndef TC_H
#define TC_H

#include <Arduino.h>
#include "config.h"

// timecode definitions
#define GEN_MAX_BITS 8
#define GET_MAX_BYTES 10

struct TIMECODE {
	uint8_t h;
	uint8_t m;
	uint8_t s;
	uint8_t f;
	uint8_t fps;
	uint8_t offset;		// offset in frames between read timecode and rtc second irq
	bool dropframe;
	bool colorframe;
	bool biphase;
	bool flag0;
	bool flag1;
	bool changed;
};

struct USERBITS {
	uint8_t bit[8];
};


class TC {

public:
	void begin(void);

	void enable(bool);
	bool enable(void);

	void set(TIMECODE);
	void set(uint8_t h, uint8_t m, uint8_t s, uint8_t f, uint8_t fps);
	void set(uint8_t h, uint8_t m, uint8_t s, uint8_t f);
	void set(uint8_t*);

	// write and read flags
	void flags(uint8_t);
	uint8_t flags(void);

	TIMECODE get(void);

	void update_binary(void);

	void fps(uint8_t);
	uint8_t fps(void);

	void sync(void);
	uint8_t offset(void);

	void ubits(USERBITS*);
	void ubits(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
	void ubit(uint8_t, uint8_t);
	uint8_t ubit(uint8_t nr);
	USERBITS ubits(void);

	long inc(bool);
	bool changed(void);
	void unchange(void);

	bool fps_changed(void);
	void fps_change(bool);

private:
	TIMECODE _tc;
	USERBITS _ub;

	uint8_t _binary[10];
	uint8_t _bit_counter;
	uint8_t _byte_counter;

	uint8_t _last_fps;
	bool _fps_changed;

	bool _enable;

	bool _start;
	bool _level;

	uint16_t _i;

	long _time;
	long _old_time;
	long _length;
};


#endif