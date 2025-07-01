#ifndef SCENE_H
#define SCENE_H

#include <Arduino.h>
#include "config.h"

#include "led_matrix.h"


class SCENE {

public:
	void begin(LED_MATRIX* display);

	char* int2char(uint16_t);

	void set(char* scene, char* take, char* nr);
	void scene(char* scene);
	void take(char* take);
	void number(char* nr);

private:
	LED_MATRIX* _display;

};


#endif