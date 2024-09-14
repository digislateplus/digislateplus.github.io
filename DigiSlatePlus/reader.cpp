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
	if (_sync) {
// DEBUG
		// digitalWrite(SIGNAL_OUTPUT, val);
	}



// ToDo
// calculate frame time?

	

	// suppress short spikes
	if (delta > (TIMECODE_THRESHOLD / 2)) {

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
// get timecode structure
TIMECODE READER::get(void) {
	return _tc.get();
}


// ===========================================
// add bit to buffer
void READER::_add(bool bit) {

// DEBUG
// create peak
// _peak();

	// is synced > write data
	if (_sync) {

		// set bit >> LSB to the left
		if (bit) {
			_timecode[_byte_counter] |= (1 << (7 - _bit_counter));
		}

		// clear bit
		else {
			_timecode[_byte_counter] &= (1 << (7 - _bit_counter));
		}


		// increment bit counter
		_inc();
	}


	// check sync word
	if (_check_sync_word(bit)) {
		_sync = true;

		// is in sync > reset and set sync to true
		_reset();
	}
}


// ===========================================
// increment counter
// return bit index (0-79)
uint8_t READER::_inc(void) {


	_bit_counter++;

	if (_bit_counter >= 8) {
		_bit_counter = 0;
		_byte_counter++;


		// date complete > write raw data to timecode
		// reset counter
		if (_byte_counter == 8) {
_peak();
			_tc.set(_timecode);
			_reset();
			_sync = false;
		}

		// overflow (no reset from sync word)
		// 8 byte data (the sync word is not stored)
		// if (_byte_counter >= 10) {
		// 	_reset();
		// 	_sync = false;
		// }
	}



	return _index();
}


// ===========================================
// get bit index
uint8_t READER::_index(void) {
	return (_byte_counter * 8) + _bit_counter;
}


// ===========================================
// reset counter
void READER::_reset(void) {

	_bit_counter = 0;
	_byte_counter = 0;
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
	digitalWrite(SIGNAL_OUTPUT, !digitalRead(SIGNAL_OUTPUT));
	digitalWrite(SIGNAL_OUTPUT, !digitalRead(SIGNAL_OUTPUT));
}