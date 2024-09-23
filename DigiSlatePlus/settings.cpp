#include "settings.h"
#include "setup.h"


bool SETTINGS::begin(uint8_t port, LCD* lcd, LED* led) {

	_port = port;

	_lcd = lcd;
	_led = led;

	_oldstatus = 0;
	_status = 1;


	// setup start
	bool clapbar;
	bool setup = false;
	uint8_t repeats = 0;
	bool clapped = false;
	bool open = false;
	long start_time = millis();


	// clap three times in one second to enter setup
	while(millis() < (start_time + SETTINGS_ENTER_TIMEOUT)) {

		// clap is open
		if (digitalRead(_port)) {
			open = true;
		}

		else {
			if (open) {
				open = false;
				repeats++;

				// end loop and start setup
				if (repeats >= SETTINGS_ENTER_REPEATS) {
					setup = true;
					break;
				}
			}
		}

		delay(10);
	}

	return setup;
}


void SETTINGS::exec(void) {

	_lcd->clear();
	_lcd->print("SETUP", 0, 0);

	while(_status <= SETTINGS_MAX_STATI) {

		// status has changed
		if (_status != _oldstatus) {
			_lcd->print(status_text[_status - 1], 0, 1);
		}

delay(1000);
_status++;
	}
}