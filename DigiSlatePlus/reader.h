#ifndef READER_H
#define READER_H

#include <Arduino.h>

// 24 fps	42ms / 21ms
// 25 fps	40ms / 20ms
// 30 fps	33ms / 16,5ms
// threshold at 27 ms
#define TIMECODE_THRESHOLD 27


class READER {

public:
	void begin(uint16_t port);
	bool read(void);


private:
	uint8_t _inc(void);
	void _add(bool);
	void _reset(void);
	bool _check_sync_word(bool);

	uint8_t _i;			// global counter index

	uint16_t _port;

	bool _start;
	bool _sync;

	long _last_time;
	long _bit_time;
	long _half_bit_time;

	uint8_t _timecode[10];

	uint8_t _bit_counter;
	uint8_t _byte_counter;

	uint16_t _sync_word;
	uint8_t _sync_word_counter;
};


#endif