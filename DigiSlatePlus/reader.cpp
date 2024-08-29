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

	bool val = digitalRead(_port);
	
	long delta = millis() - _last_time;
	_last_time = millis();


	// is logic 0
	if (delta > TIMECODE_THRESHOLD) {

		// must be bit start
		_start = true;
		_add(0);
	}

	// half bit
	else {

		// is mid bit > add logic 1
		if (_start) {
			_add(1);
		}

		// toggle start
		_start != _start;
	}

	// increment counter
	_inc();

	return _sync;
}


// set bit
void READER::_add(bool bit) {

	// sync word found
	if (_check_sync_word(bit)) {
		_reset();
		_sync = true;
	}


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
	}


	// increment bit counter
	_inc();
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
	if (_byte_counter >= 10) {
		_reset();
	}

	return (_byte_counter * 8) + _bit_counter;
}


// ===========================================
// reset counter
void READER::_reset(void) {

	_sync = false;
	_start = true;

	_bit_counter = 0;
	_byte_counter = 0;

	_sync_word_counter = 0;

	// reset timecode raw data
	for (_i = 0; _i < 10; _i++) {
		_timecode[_i] = 0;
	}
}


// add bit to sync word and check
// return true, if sync word found
// sync word: 0b1011111111111100
bool READER::_check_sync_word(bool bit) {

	bool sync = false;

	// check sync word bit (from left to right)
	// increment counter
	if (bit == (_sync_word & (1<<(15-bit)))) {

		_sync_word_counter++;

		if (_sync_word_counter > 15) {
			_sync_word_counter = 0;
			sync = true;
		}
	}

	// no match > reset counter
	else {
		_sync_word_counter = 0;
	}

	return sync;
}