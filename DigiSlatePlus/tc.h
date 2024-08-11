/*
 TIMECODE class

 author: Thomas Winkler
 e-mail: thomas.winkler@iggmp.net
 copyright: 2024
 */

/*
SMPTE/EBU timecode structure (https://www.wikiaudio.org/smpte-time-code/)
 
 Bit     content
 ------------------
** byte 0
 0-3	frame units
 4-7	user bits 1
** byte 1
 8-9	frame tens
 10		dropframe bit 	1=drop frame (frame 0 and 1 omitted from first second of each minute, but included when minutes divides by ten; approximates 29.97 frame/s)
 11 	colorframe bit 	i.e. the time code is intentionally synchronised with a color TV field sequence.
 12-15	user bits 2
** byte 2
 16-19	secs units
 20-23	user bits 3
** byte 3
 24-26	secs tens
 27		bi phase mark correction bit 
 28-31	user bits 4
** byte 4
 32-35	mins units
 36-39	user bits 5
** byte 5
 40-42	mins tens
 43		binary group flag bit (with bit 59, 43,59 = 00 = no format for user bits, 10 = eight bit format, 01, 11 are unassigned and reserved).
 44-47	user bits 6
** byte 6
 48-51	hours units
 52-55	user bits 7
** byte 7
 56-57	hour tens
 58		unused, reserved, should transmit zero and ignore on receive for compatibility
 59		binary group flag bit (see bit 43 for encoding)
 60-63	user bits 8
** byte 8 - 9
 64-79	sync word (should be 0011 1111 1111 1101)

 logic 0 	no change in clock frame
 logic 1 	change in frame

 */

#ifndef TC_H
#define TC_H

#include <Arduino.h>
#include "setup.h"

// timecode definitions
#define GEN_MAX_BITS 8
#define GET_MAX_BYTES 10

struct TIMECODE {
	uint8_t h;
	uint8_t m;
	uint8_t s;
	uint8_t f;
	uint8_t fps;
	bool dropframe;
	bool colorframe;
	bool biphase;
	bool flag0;
	bool flag1;
	bool changed;
};

struct USERBITS {
	uint8_t bit[8];
};


class TC {

public:
	void begin(void);

	void set(TIMECODE);
	void set(uint8_t h, uint8_t m, uint8_t s, uint8_t f, uint8_t fps);
	void set(uint8_t h, uint8_t m, uint8_t s, uint8_t f);
	TIMECODE get(void);

	void update_binary(void);

	void fps(uint8_t);
	uint8_t fps(void);

	void ubits(USERBITS*);
	void ubits(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
	void ubit(uint8_t, uint8_t);
	uint8_t ubit(uint8_t nr);
	USERBITS ubits(void);

	long inc(bool);
	bool changed(void);
	void unchange(void);

private:
	TIMECODE _tc;
	USERBITS _ub;

	uint8_t _binary[10];
	uint8_t _bit_counter;
	uint8_t _byte_counter;

	bool _start;
	bool _level;

	uint16_t _i;

	long _time;
	long _old_time;
	long _length;
};


#endif