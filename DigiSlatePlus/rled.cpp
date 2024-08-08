#include "rled.h"


void RLED::begin(uint16_t port) {
	_port = port;
	pinMode(port, OUTPUT),
	off();
}


void RLED::on(void) {
	digitalWrite(_port, HIGH);
}

void RLED::off(void) {
	digitalWrite(_port, LOW);
}

void RLED::toggle(void) {
	digitalWrite(_port, !digitalRead(_port));
}

void RLED::flash(void) {
	flash(30);
}

void RLED::flash(uint16_t time) {
	flash(time, 1);
}

void RLED::flash(uint16_t time, uint8_t repeat) {

	for (uint8_t i = 0; i < repeat; i++) {
		on();
		delay(time);
		off();
		delay(time);
	}
}