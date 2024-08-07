/*
 TIMECODE class

 author: Thomas Winkler
 e-mail: thomas.winkler@iggmp.net
 copyright: 2024
 */

#include "tc.h"


// set timecode
void TC::set(uint8_t h, uint8_t m, uint8_t s, uint8_t f) {
	_tc.h = h;
	_tc.m = m;
	_tc.s = s;
	_tc.f = f;

	_tc.changed = true;
}

// set timecode with framerate
void TC::set(uint8_t h, uint8_t m, uint8_t s, uint8_t f, uint8_t fr) {
	set (h, m, s, f);
	fps(fr);
}

// set framerate
void TC::fps(uint8_t fr) {
	_tc.fps = fr;
}

// get timecode struct
TIMECODE TC::get(void) {
	return _tc;
}

// get framerate
uint8_t TC::fps(void) {
	return _tc.fps;
}


// =============================================================
// inc TC
// do not increment when tick is false
// tick is set to true, if the rtc second interrupt occured
TC::inc(bool tick) {

	_tc.f++;

	// frames overflow
	if (_tc.f >= _tc.fps) {

		// only if tick
		if (tick) {
			_tc.f = 0;
			_tc.s++;

			tick = false;
		}

		// wait for tick
		else {
			return;
		}
	}

	if (_tc.s > 59) {
		_tc.s = 0;
		_tc.m++;
	}

	if (_tc.m > 59) {
		_tc.m = 0;
		_tc.h++;
	}

	if (_tc.h > 23) {
		_tc.f = 0;
		_tc.s = 0;
		_tc.m = 0;
		_tc.h = 0;
	}

	_tc.changed = true;
}


// return tc status
bool TC::changed(void) {
	return _tc.changed;
}

void TC::unchange(void) {
	_tc.changed = false;
}