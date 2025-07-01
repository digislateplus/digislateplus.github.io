#include "button.h"


// set port
void BUTTON::begin(uint8_t port) {

	_port = port;
	pinMode(_port, INPUT_PULLUP);
}


// get button status
// set open/close time if status has changed
// status = false => closed
bool BUTTON::get(void) {

	_status = digitalRead(_port);

	// status changed
	if (_status != _last_status) {

		_changed = true;
		_last_status = _status;

		// just closed
		if (_status) {
			_last_close_time = millis();

			_repeat++;
		}

		// just opened
		else {
			_last_open_time = millis();
		}
	}

	else {
		_changed = false;
	}

	return _status;
}


// button changed since last check
bool BUTTON::changed(void) {
	return _changed;
}




// is closed for time milliseconds
bool BUTTON::opened(uint16_t time) {
	return (!get() && (millis() >= (_last_open_time + time)));
}


// is closed for time milliseconds
bool BUTTON::closed(uint16_t time) {
	return (get() && (millis() >= (_last_close_time + time)));
}


// reset repeat counter
void BUTTON::reset(void) {
	_repeat = 0;
}


// get repeats
uint8_t BUTTON::repeats(void) {
	return _repeat;
}