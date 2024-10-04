#include "flash.h"


// start flash
// set size if ESP32
// return the max size
uint16_t FLASH::begin(void) {

	#ifdef DEBUG
		Serial.print("init EEPROM with length ");
		Serial.println(EEPROM.length());
	#endif

	return begin(EEPROM.length());
}


uint16_t FLASH::begin(uint16_t size) {
	_size = size;

	return _size;
}


// write timecode framerate and bits to flash store
void FLASH::write(TIMECODE tc) {

	uint8_t flags = 0;

	flags |= (tc.dropframe << FLASH_FLAGS_DROPFRAME);
	flags |= (tc.colorframe << FLASH_FLAGS_COLORFRAME);
	flags |= (tc.biphase << FLASH_FLAGS_BIPHASE);
	flags |= (tc.flag0 << FLASH_FLAGS_FLAG0);
	flags |= (tc.flag1 << FLASH_FLAGS_FLAG1);

	EEPROM.write(FLASH_FPS, tc.fps);
	EEPROM.write(FLASH_OFFSET, tc.offset);
	EEPROM.write(FLASH_FLAGS, flags);

	EEPROM.commit();
}


// get timecode framerate and bits from flash store
TIMECODE FLASH::read(void) {

	TIMECODE tc;

	uint8_t fps = EEPROM.read(FLASH_FPS);
	uint8_t offset = EEPROM.read(FLASH_OFFSET);
	uint8_t flags = EEPROM.read(FLASH_FLAGS);

	tc.fps = fps;

	tc.dropframe = (flags >> FLASH_FLAGS_DROPFRAME) & 0x01;
	tc.colorframe = (flags >> FLASH_FLAGS_COLORFRAME) & 0x01;
	tc.biphase = (flags >> FLASH_FLAGS_BIPHASE) & 0x01;
	tc.flag0 = (flags >> FLASH_FLAGS_FLAG0) & 0x01;
	tc.flag1 = (flags >> FLASH_FLAGS_FLAG1) & 0x01;

	return tc;
}


// write userbits
void FLASH::write_userbits(USERBITS ub) {

	for (_i = 0; _i < 8; _i++) {
		EEPROM.write(FLASH_USERBITS + _i, ub.bit[_i]);
	}
  
	EEPROM.commit();
}


// read userbits
USERBITS FLASH::read_userbits(void) {

	USERBITS ub;

	for (_i = 0; _i < 8; _i++) {
		ub.bit[_i] = EEPROM.read(FLASH_USERBITS + _i);
	}
}