#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include "setup.h"
#include "lcd.h"
#include "led.h"


class SETTINGS {

public:
	bool begin(uint8_t port, LCD*, LED*);
	void exec(void);

private:
	
	LCD* _lcd;
	LED* _led;

	uint8_t _port;
	uint8_t _status;
	uint8_t _oldstatus;

};


#endif