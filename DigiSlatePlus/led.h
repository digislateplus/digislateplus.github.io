#ifndef LED_H
#define LED_H

#include "tc.h"

#define F1 0x02
#define F10 0x06

#define S1 0x08
#define S10 0x04

#define M1 0x03
#define M10 0x07

#define H1 0x05
#define H10 0x01


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