#ifndef READER_H
#define READER_H

#include <Arduino.h>
#include "config.h"
#include "tc.h"

/*
 begin(port)
 	starts the reader listening to port
 
 read()
 	The read method is called by a change interrupt
 	on the signal input.

 	The timecode signal changes polarity at each bit.
 	Ich the value is 1, the polarity is also changed
 	in the middle of the bit clock.

 	bool start 		true: start of a bit
 					false: mid of a bit

	1.	The method reads the input value.
	2.	The time from the last call is calculated in ms.
 	3a.	If the time is longer than the threshold (27ms) a
 		logic 0 occured. The start variable is set to 1.
 			add(0)
 	3b.	If the time is shorter and start is true, a mid bit
 		change occured. Set start to false.
 		If start is true, the bit has ended. Set start to true. 
 			add(1)

	bit rate / half bit rate
	24 fps	520µs / 260µs
	25 fps	500µs / 250µs
	30 fps	417µs / 207,5µs
	threshold at 338 µs
 
 sync()
 	Returns true, if the reader is in sync.

 available()
 	Returns true, if a new value is available.
 
 add()
	Adds a value to the timecode buffer.

	1.	If synced, add bit to buffer. Increment bit and byte counter.
	2.	check_sync_word(bit)
	3.	If sync word occured, reset and set sync to true and the sync
		word counter is reset.

 check_sync_word(bit)
 	Check 16 bits for the sync word (1011111111111100).
 	If a bit is correct, the sync_word_counter is incremented.
 	At the next call, the next bit is checked.

	1.	If bit is correct, increment sync word counter.
	2.	If bit is incorrect, reset counter and set sync to false.
	3.	If the counter reaches 15, sync is set to true.

 inc()
 	Increments the bit and byte counter. Returns the bit index (0-63).
 	If an overflow occurs, the counters and data is reset.
 */

#define TIMECODE_THRESHOLD 338 //µs


class READER {

public:
	void begin(uint16_t port);
	void reset(void);
	bool read(void);
	bool sync(void);
	bool available(void);
	bool fps_changed(void);

	TIMECODE get(void);

	long debug;

private:
	uint8_t _inc(void);
	uint8_t _index(void);

	uint8_t _reverse(uint8_t);

void _peak(void);
void _rect(void);

	void _add(bool);
	void _reset(void);
	bool _check_sync_word(bool);

	uint8_t _i;			// global counter index

	uint16_t _port;

	TC _tc;

	bool _start;
	bool _sync;

	long _threshold;

	long _last_time;
	long _bit_time;
	long _half_bit_time;

	uint8_t _raw_timecode[10];

	uint8_t _counter;

uint8_t _bit_counter;
uint8_t _byte_counter;

	uint16_t _sync_word;
	uint16_t _sync_register;
};


#endif