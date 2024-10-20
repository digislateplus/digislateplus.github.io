#include "settings.h"
#include "config.h"


void SETTINGS::begin(LCD* lcd) {

	_lcd = lcd;
	_active = false;
}


void SETTINGS::exec(void) {

	// // enter setup
	if (!_active) {
		_lcd->clear();
		_lcd->print("SETUP", 0, 0);

		_status = SET_DEFAULT;
		_active = true;
	}

	// exit setup
	else {
		_lcd->clear();

		_active = false;
	}
}


void SETTINGS::enter(void) {

	if (_active) {
		_lcd->print("button", 0, 1);
	}
}


void SETTINGS::inc(void) {

}


void SETTINGS::dec(void) {

}