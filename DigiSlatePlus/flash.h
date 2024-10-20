#ifndef FLASH_H
#define FLASH_H

#include <EEPROM.h>
#include "config.h"
#include "tc.h"


class FLASH {

public:
	uint16_t begin(void);
	uint16_t begin(uint16_t);
	void write(TIMECODE);
	TIMECODE read(void);

	void write_userbits(USERBITS);
	USERBITS read_userbits(void);

private:
	uint8_t _i;
	uint16_t _size;
	
};

#endif