#ifndef LCD_H
#define LDC_H

#include <LiquidCrystal.h>  // for our 16x2 display
#include <SPI.h>


class LCD {

public:
	void begin(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
	void status(char*);
	void dir(bool);
	void fps(uint8_t);
	void ubits(char*);

	void val8(uint8_t, uint8_t, uint8_t);
	void val16(uint16_t, uint8_t, uint8_t);
	void val32(uint32_t, uint8_t, uint8_t);
	void print(char*, uint8_t, uint8_t);

	void clear(void);

private:
	LiquidCrystal* _disp;
	char* _arroz[2];
};


#endif