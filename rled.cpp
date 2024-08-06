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

void RLED::flash(void) {
	flash(30);
}

void RLED::flash(uint16_t time) {
	on();
	delay(time);
	off();
}