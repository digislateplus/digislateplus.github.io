#include "flash.h"


// write timecode framerate and bits to flash store
void FLASH::write(TIMECODE tc) {

	uint8_t fps;
	uint8_t flags = 0;

	fps = tc.fps;
	flags |= (tc.dropframe << FLASH_FLAGS_DROPFRAME);
	flags |= (tc.colorframe << FLASH_FLAGS_COLORFRAME);
	flags |= (tc.biphase << FLASH_FLAGS_BIPHASE);
	flags |= (tc.flag0 << FLASH_FLAGS_FLAG0);
	flags |= (tc.flag1 << FLASH_FLAGS_FLAG1);

	EEPROM.update(FLASH_FPS, fps);
	EEPROM.update(FLASH_FLAGS, flags);
}


// get timecode framerate and bits from flash store
TIMECODE FLASH::read(void) {

	TIMECODE tc;

	uint8_t fps = EEPROM.read(FLASH_FPS);
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
		EEPROM.update(FLASH_USERBITS + _i, ub.bit[_i]);
	}
}


// read userbits
USERBITS FLASH::read_userbits(void) {

	USERBITS ub;

	for (_i = 0; _i < 8; _i++) {
		ub.bit[_i] = EEPROM.read(FLASH_USERBITS + _i);
	}
}