#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include "setup.h"
#include "lcd.h"
#include "led.h"
#include "button.h"


class SETTINGS {

public:
	bool begin(BUTTON*, LCD*, LED*);
	void exec(void);

private:

	LCD* _lcd;
	LED* _led;
	BUTTON* _button;

	uint8_t _status;
	uint8_t _oldstatus;

};


#endif