#ifndef LED_H
#define LED_H

#include "tc.h"


class LED {
public:

	void begin(uint8_t);
	void set(TIMECODE);
	void set(uint8_t, uint8_t, uint8_t, uint8_t);
	void frames(uint8_t);
	void seconds(uint8_t);
	void minutes(uint8_t);
	void hours(uint8_t);

private:
	uint8_t _load_pin;
	void _write(uint8_t, uint8_t);
	void _digits(uint8_t, uint8_t, uint8_t);
};


#endif