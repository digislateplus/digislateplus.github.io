#include "settings.h"
#include "setup.h"


bool SETTINGS::begin(BUTTON* button, LCD* lcd, LED* led) {

	_button = button;
	_lcd = lcd;
	_led = led;

	_oldstatus = 0;
	_status = 1;

	// reset button counter
	_button->reset();

	// setup start
	bool setup = false;
	long start_time = millis();


	// clap three times in one second to enter setup
	while(millis() < (start_time + SETTINGS_ENTER_TIMEOUT)) {

		// check button
		button->get();
		
		// check for repeats
		if (button->repeats() >= SETTINGS_ENTER_REPEATS) {
			setup = true;
			break;
		}

		delay(10);
	}

	return setup;
}


void SETTINGS::exec(void) {

	_lcd->clear();
	_lcd->print("SETUP", 0, 0);

	// _led->clear();
	// _led->print("SETUP", 0, 5);

	while(_status <= SETTINGS_MAX_STATI) {

		// status has changed
		if (_status != _oldstatus) {
			_lcd->print(status_text[_status - 1], 0, 1);
		}

delay(1000);
_status++;
	}
}