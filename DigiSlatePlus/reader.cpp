#include "reader.h"


// start listening at port
void READER::begin(uint16_t port) {

	_port = port;
	pinMode(port, INPUT_PULLUP);

	// set initial threshold
	_threshold = TIMECODE_THRESHOLD;

	// sync is true if frame is available
	_sync = false;
	_reset();

	_sync_word =  0b0011111111111101;
	_sync_register = 0;
}


// ===========================================
// input value changed
bool READER::read(void) {

	long current = micros();
	long delta = current - _last_time;
	_last_time = current;

	// recalculate threshold
	// _threshold = (_threshold + delta) / 2;

	bool val = digitalRead(_port);


	// loop write to tc output if synced
	// if (_sync) {
// DEBUG
		// digitalWrite(SIGNAL_OUTPUT, val);
	// }



// ToDo
// calculate frame time?

	
	// suppress short spikes
	if (delta > (TIMECODE_THRESHOLD / 2)) {
// _peak();

		// long time
		// is logic 0 of 1+0
		if (delta > TIMECODE_THRESHOLD) {

			// 0 bit following 1 bit
			if (!_start) {
				_start = true;
			}

			// is 0 bit
			else {

			}

			// must be bit start
			_start = true;
			_add(0);
		}


		// short time
		// half bit
		else {

			// is mid bit > add logic 1
			if (_start) {
				_start = false;
			}

			else {
				_add(1);
				_start = true;
			}
		}
	}

	// return sync status
	return _sync;
}


// ===========================================
// return sync status
bool READER::sync(void) {
	return _sync;
}


// ===========================================
// True if a new value is available
bool READER::available(void) {
	return _tc.changed();
}


// ===========================================
// framerate has changed
bool READER::fps_changed(void) {
	return _tc.fps_changed();
}


// ===========================================
// get framerate


// ===========================================
// get timecode structure
TIMECODE READER::get(void) {
	return _tc.get();
}


// ===========================================
// add bit to buffer
void READER::_add(bool bit) {

	uint8_t byte_count = _counter >> 3;
	uint8_t bit_count = _counter & 0b111;


	// is synced > write data
	if (_sync) {

		// set bit in reverse order >> LSB to the left
		if (bit) {
			_raw_timecode[byte_count] |= (1 << bit_count);
		}

		// clear bit
		else {
			_raw_timecode[byte_count] &= ~(1 << bit_count);
		}


		// increment bit counter
		_inc();
	}


	// check sync word
	else if (_check_sync_word(bit)) {
		_sync = true;

		// is in sync > reset and set sync to true
		_reset();
	}
}


// ===========================================
// increment counter
// return bit index (0-79)
// reset sync -> sync word must follow
uint8_t READER::_inc(void) {

	_counter ++;

	// end of data count (8 byte)
	if (_counter >= 64) {

		_tc.set(_raw_timecode);
		_reset();
		_sync = false;
	}

	return _index();
}


uint8_t READER::_reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}


// ===========================================
// get bit index
uint8_t READER::_index(void) {
	return _counter;
}


// ===========================================
// reset counter
void READER::_reset(void) {
	_counter = 0;
}


// ===========================================
// add bit to sync word and check
// return true, if sync word found
// sync word: 0b0011111111111101
bool READER::_check_sync_word(bool bit) {


	bool sync = false;

	// 16 bit register
	// shift left and add bit
	// compare with sync word
	_sync_register = (_sync_register << 1) | bit;

	if (_sync_register == _sync_word) {
		sync = true;
	}

	return sync;
}


// DEBUG
// create peak
void READER::_peak(void) {
	_rect();
	_rect();
}


void READER::_rect(void) {
	digitalWrite(SIGNAL_OUTPUT, !digitalRead(SIGNAL_OUTPUT));
}