#include "lcd.h"


void LCD::begin(uint8_t rs, uint8_t en, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7) {

	const char arr0[] = {0x20, 0x7E, 0x20, 0};    // forward arrow
	const char arr1[] = {0x20, 0x7F, 0x20, 0};    // reverse arrow
	_arroz[0] = arr0;
	_arroz[1] = arr1;

	//   rs  en  d4 d5 d6 d7;
	_disp = new LiquidCrystal(rs, en, d4, d5, d6, d7);

	// =============================================================
	// display startup text on LCD
	_disp->begin(16, 2);
	_disp->clear();
}

void LCD::status(char* status) {
	_disp->setCursor(12, 0);
	_disp->print(status);
}


void LCD::dir(bool dir) {
	_disp->setCursor(0, 0);
	_disp->write(_arroz[dir]);
}

void LCD::fps(uint8_t fps) {
	_disp->setCursor(3, 0);
	_disp->print(fps);
	_disp->print(" fps");
}

void LCD::ubits(char* uBits) {
	_disp->setCursor(0,1);
	_disp->print("UB:");
	_disp->print(uBits);
}


void LCD::print(char* text, uint8_t x, uint8_t y) {
	_disp->setCursor(x, y);
	_disp->print(text);
}


void LCD::clear(void) {
	_disp->clear();
}