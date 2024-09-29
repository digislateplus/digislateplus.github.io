#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "setup.h"
#include "button.h"


class BUTTON {

public:
	void begin(uint8_t port);
	bool get(void);
	bool changed(void);

	bool opened(uint16_t);
	bool closed(uint16_t);
	
	void reset(void);
	uint8_t repeats();

private:
	uint8_t _port;

	bool _status;
	bool _last_status;
	bool _changed;

	long _last_open_time;
	long _last_close_time;
	uint8_t _repeat;

};

#endif