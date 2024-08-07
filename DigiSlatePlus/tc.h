/*
 TIMECODE class

 author: Thomas Winkler
 e-mail: thomas.winkler@iggmp.net
 copyright: 2024
 */

#ifndef TC_H
#define TC_H

#include <Arduino.h>


struct TIMECODE {
	uint8_t h;
	uint8_t m;
	uint8_t s;
	uint8_t f;
	uint8_t fps;
	bool changed;
};


class TC {

public:
	void set(uint8_t h, uint8_t m, uint8_t s, uint8_t f);
	void set(uint8_t h, uint8_t m, uint8_t s, uint8_t f, uint8_t fps);
	TIMECODE get(void);
	void fps(uint8_t);
	uint8_t fps(void);
	void inc(bool);
	bool changed(void);
	void unchange(void);

private:
	TIMECODE _tc;
};


#endif