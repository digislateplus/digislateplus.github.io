#include "reader.h"


// start listening at port
void READER::begin(uint16_t port) {

	_port = port;
	pinMode(port, INPUT_PULLUP);

	// sync is true if frame is available
	_sync = false;
	_reset();

	_sync_word =  0b1011111111111100;
}


// ===========================================
// input value changed
bool READER::read(void) {

	long current = micros();
	long delta = current - _last_time;
	_last_time = current;

	bool val = digitalRead(_port);

// create peak
// digitalWrite(SIGNAL_OUTPUT, !digitalRead(SIGNAL_OUTPUT));
// digitalWrite(SIGNAL_OUTPUT, !digitalRead(SIGNAL_OUTPUT));

	// long time
	// is logic 0
	if (delta > TIMECODE_THRESHOLD) {

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
	return true;
}


// ===========================================
// add bit to buffer
void READER::_add(bool bit) {

// DEBUG
// create peak
// digitalWrite(SIGNAL_OUTPUT, !digitalRead(SIGNAL_OUTPUT));
// digitalWrite(SIGNAL_OUTPUT, !digitalRead(SIGNAL_OUTPUT));


	// is synced > write data
	if (_sync) {

		// set bit
		if (bit) {
			_timecode[_byte_counter] |= (1<<_bit_counter);
		}

		// clear bit
		else {
			_timecode[_byte_counter] &= (1<<_bit_counter);
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
	}

	// overflow (no reset from sync word)
	// 8 byte data (the sync word is not stored)
	if (_byte_counter >= 10) {
		_reset();
		_sync = false;
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

	// reset timecode raw data
	// for (_i = 0; _i < 10; _i++) {
	// 	_timecode[_i] = 0;
	// }
}


// ===========================================
// add bit to sync word and check
// return true, if sync word found
// sync word: 0b0011111111111101
bool READER::_check_sync_word(bool bit) {

	bool sync = false;

// DEBUG
// debug = _sync_word_counter;


	// check sync word bit (from left to right)
	// increment counter
	if (bit == (0b1011111111111100>>_sync_word_counter & 0x01)) {

for (_i=0;_i<_sync_word_counter+1;_i++) {
	digitalWrite(SIGNAL_OUTPUT, HIGH);
	digitalWrite(SIGNAL_OUTPUT, LOW);
}

		_sync_word_counter++;

		if (_sync_word_counter >= 15) {
			_sync_word_counter = 0;
			sync = true;
		}
	}

	// no match > reset counter
	else {
		_sync_word_counter = 0;
	}

// DEBUG
// set value
// digitalWrite(SIGNAL_OUTPUT, _sync);

	return sync;
}