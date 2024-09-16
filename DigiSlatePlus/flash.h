#ifndef FLASH_H
#define FLASH_H

#include <EEPROM.h>
#include "setup.h"
#include "tc.h"


class FLASH {

public:
	void write(TIMECODE);
	TIMECODE read(void);

	void write_userbits(USERBITS);
	USERBITS read_userbits(void);

private:
	uint8_t _i;
};

#endif