#include "scene.h"



void SCENE::begin(LED_MATRIX* display) {

	_display = display;

	_display->clear();
	_display->print("\x10\x0", 0);
	_display->print("|\x11\x0", 25);
	_display->print("|\x0", 50);

	set("1\x0", "1\x0", "1\x0");

}


char* SCENE::int2char(uint16_t val) {

	char numberstring[(((sizeof val) * CHAR_BIT) + 2)/3 + 3];
	sprintf(numberstring, "%d\x0", val);

	return numberstring;
}


void SCENE::set(char* scene_val, char* take_val, char* nr_val) {
	scene(scene_val);
	take(take_val);
	number(nr_val);
}


void SCENE::scene(char* scene) {

	_display->area(4, 23);
	_display->clear();
	_display->print(scene, ALIGN_CENTER);

	_display->area();
}


void SCENE::take(char* take) {

	_display->area(31, 48);
	_display->clear();
	_display->print(take, ALIGN_RIGHT);

	_display->area();
}


void SCENE::number(char* nr) {

	_display->area(52, 63);
	_display->clear();
	_display->print(nr, ALIGN_LEFT);

	_display->area();
}