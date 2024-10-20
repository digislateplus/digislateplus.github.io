#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include "config.h"
#include "lcd.h"


#define SET_SCENE 0
#define SET_TAKE 1
#define SET_NUMBER 2

#define SET_HOUR 3
#define SET_MINUTE 4
#define SET_SECOND 5

#define SET_FPS 6

#define SET_DEFAULT SET_SCENE

// static const char* settings_menu[] = {"scene","take","number","hour","minut","second","fps"};


class SETTINGS {

public:
	void begin(LCD*);
	void exec(void);
	void enter(void);
	void inc(void);
	void dec(void);

private:

	LCD* _lcd;

	bool _active;
	uint8_t _status;

};


#endif