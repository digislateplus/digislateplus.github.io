/*
 DigiSlatePlus
 */

/*
This Source Code Form is subject to the terms of the
Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed
with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

Copyright (c) 2024 Thomas Winkler

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
*/


/*
 The DigiSlatePlus creates timecode synchronized with a
 DS3231 high precision RTC clock and sent to the 3,5mm Jack.

 The time is set by maintaining a LTC timecode to the input
 and powering up the slate. The RTC time is set according
 to the provided timecode.
 !!! (NOT YET IMPLEMENTED)

 When no timecode is found on the BNC connector, the internal
 generated timecode is displayed and available on the BNC.
 */


/* Adaptive SMPTE/EBU LTC Reader

	based on code by Jim Mack
			Editing Services Co.
			esc@mdxi.com

	 Reads LTC (audio timecode) on pin 2, at any common
		frame rate and at any speed from about 1/10x to 5x
		play, in forward or reverse.

	 Proper reading requires a good quality signal and
		conditioning. A simple booster will often be
		enough to read consumer-level signals. A schematic
		for a simple input stage is shown in the blog post.

	 This code will have less success tracking sources with
		high compression or generation loss. Better audio =
		better results, especially at higher speeds.
*/


/*
 FLOAT
 
 setup()
	1. init classes and variables
	2. start LED and LC displays and clap flash LED
	3. init timecode structure
	4. init real time clock
	5. init interrupts
		a. init reader class
		b. attach interrupts

 loop()
	1. get clapbar switch
	
	is RUNMODE
		1. IF boot or runmode did not change
			a. read time from rtc and write to timecode
			b. set framerate
		2.	IF timecode has changed
			a. check clapbar and flash light
			b. display timecode on LED if running (not just clapped)
			c. IF tick (interrupt from RTC occured)
				recalculate bit timer for tc output
		3. display status (run, clap, init) on LCD

	is READMODE
		1. check for read timeout > set to runmode
		2. IF boot or mode changed
			a. set mode to READMODE
		3. diable timecode
		4. IF reader synced
			a. IF value available
				> get reader timecode
				> display on LED
				> IF framerate changed
					* display fps on LCD
				> IF read boot
					* display status jam on LCD

					* IF rtc not updated
						> get date from rtc
						> write read time to RTC
						> set rtc updated true

				> ELSE display status sync on LCD
		5. ELSE display status read on LCD

 */

/*
 inspire libraries:
 ------------------
 LedControl			https://github.com/wayoda/LedControl


 dependencies:
 -------------
 RTCLIB.h 			by Adafruit / fork of Jeelab's RTC library Version 2.1.4
 LiquidCrystal.h 	by Arduino Version 1.0.7
 */



#include <Arduino.h>
#include <RTClib.h>

#include "setup.h"

#include "lcd.h"
#include "led.h"
#include "rled.h"
#include "tc.h"
#include "rtc.h"
#include "reader.h"
#include "flash.h"
#include "settings.h"
#include "button.h"


// =========================================
// init classes
LCD lcd;

LED led_tc;
LED led_matrix_1;
LED led_matrix_2;

RLED rled;
RLED dled;

TC tc;
RTC rtc;

READER reader;
FLASH flash;
BUTTON button;

TC reader_tc;


bool run;
uint32_t claptime;
uint32_t old_claptime;


// =========================================
// timecode timing
long realtime;
long old_realtime;
long cycletime;

long timertime;
long lastreadtime;

bool tick;


// DEBUG
long offset;


// =========================================
// run mode
// 		RUNMODE => free run mode
// 		READMODE => read mode
bool runMode;
bool lastRunMode;

bool boot;				// true on system start
bool rtc_updated;		// false on system start

#include <EEPROM.h>

// =========================================
void setup() {



	// set initial state
	// set initial state to readmode
	runMode = READMODE;
	lastRunMode = runMode;

	boot = true;
	rtc_updated = false;


	// start timing
	realtime = micros();
	old_realtime = realtime;


	// =============================================================
	// INIT IO
	pinMode(SIGNAL_OUTPUT,OUTPUT);
	button.begin(SLATE_PORT);


	// =============================================================
	// INIT flash led
	rled.begin(FLASH_LED);
	rled.on();

	dled.begin(DOTS_LED);
	dled.on();


	// =============================================================
	// INIT LC-Display
	lcd.begin(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);


	lcd.print(" DigiSlate-Plus", 0, 0);
	lcd.print("  initializing", 0, 1);


	// =============================================================
	// INIT LED-Display
	led_tc.begin(LOAD_TC);
	led_matrix_1.begin(LOAD_1);
	led_matrix_2.begin(LOAD_2);


	// test LED display
	led_tc.set(88, 88, 88, 88);
	delay(500);


	// =============================================================
	// start real time clock
	switch (rtc.begin(RTC_INT_PORT)) {

		case 1:
			lcd.status(" RTC");

			break;

		case 0:
			lcd.status("!RTC");
			break;

		case -1:
			lcd.status(" SET");
			break; 
	}


	// =============================================================
	// INIT timecode
	tc.begin();
	tc.set(0,0,0,0);


	// =============================================================
	// check for setup
	lcd.clear();
	lcd.print("  Tripple clap", 0, 0);
	lcd.print(" to enter setup", 0, 1);


	SETTINGS settings;

	if (settings.begin(&button, &lcd, &led)) {
		settings.exec();
	}


	// =============================================================
	// INIT flash
	// get data from EEPROM
	flash.begin();
	TIMECODE flash_tc = flash.read();


	// framerate is not valid > load default values and write to flash
	if (!in_array(flash_tc.fps, framerates, 3)) {

		tc.fps((uint8_t)DEFAULT_FRAMERATE);
		tc.flags(0);

		lcd.clear();
		lcd.print("No flash values!", 0, 0);
		lcd.print("  load default", 0, 1);

		flash.write(tc.get());
		flash.write_userbits(tc.ubits());

		delay(1000);
	}


	// set found framerate
	else {
		tc.fps(flash_tc.fps);
	}


	// display timecode on LED
	led_tc.set(tc.get());


	// =============================================================
	// end INIT sequence
	rled.off();
	lcd.clear();


	// =============================================================
	// activate interrupts
	pinMode(RTC_INT_PORT, INPUT_PULLUP);  	// rtc sync input


	// start reader class
	reader.begin(SIGNAL_INPUT);

	tick = false;

	cli();
	attachInterrupt(digitalPinToInterrupt(SIGNAL_INPUT), readISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(RTC_INT_PORT), syncISR, FALLING);
	sei();


	start_timer1(tc.fps());
}


// =========================================
//
void loop() {

	// get clapbar state
	// true = closed
	bool clapbar = !button.get();


	// =============================================================
	// =============================================================
	// free run mode
	// no TC on input
	// =============================================================
	// =============================================================
	if (runMode == RUNMODE) {


		// ===================================
		// mode changed to runMode
		if (boot || (runMode != lastRunMode)) {

			lcd.clear();

			lastRunMode = runMode;
			boot = false;
			rtc_updated = false;

			// init runMode
			// read rtc and set time code
			if (rtc.status() != false) {

				// init time of slate to rtc
				DateTime time = rtc.get();
				tc.set(time.hour(), time.minute(), time.second(), 0);

				// set date in user bits
				// tc.ubits((time.year() / 1000) & 0xF, (time.year() / 100) & 0xF, (time.year() % 100) & 0xF, (time.month() / 10) & 0xF, (time.month() % 10) & 0xF, (time.day() / 10) & 0xF, (time.day() % 10) & 0xF);

				// snprintf("%0d:%0d:%0d", tc.ubit[0], tc.ubit[2], tc.ubit[3]);

				// display date as userbits on LCD
		// DEBUG uncomment for second row display
				// lcd.val8(time.day(), 0, 1);
				// lcd.print(".", 2, 1);
				// lcd.val8(time.month(), 3, 1);
				// lcd.print(".", 5, 1);
				// lcd.val16(time.year(), 6, 1);
			}


			// ===================
			// set lc display data
			lcd.fps(tc.fps());
		}


		// =============================================================
		// slate just closed (button open)
		if (button.changed() && clapbar == true) {
			rled.flash(30);
			run = false;
		}


		// slate was closed (button open) for CLAP_LONG_CLOSED
		if (button.opened(CLAP_LONG_CLOSED)) {
			// rled.flash(30);
			run = true;
		}


		// =============================================================
		// update time if timecode has changed
		if (tc.changed()) {
			tc.unchange();


			// ===================================
			// slate is closed for CLAP_LONG_CLOSED ms
			if (run) {

				if (tc.enable()) {
					led_tc.set(tc.get());
				}
			}

			// second started
			// correct timer every second
			if (tick) {

				tick = false;

				// =========================================
				// sync timer by rtc
				// rtc time smaller
				// speed um timer
				if (cycletime < timertime) {
					OCR1A--;
				}

				// slow down timer
				else if (cycletime > timertime) {
					OCR1A++;
				}


				// enable if different is low enough
				if (abs(cycletime - timertime) < ENABLE_LIMIT) {
					tc.enable(true);
				}
				else {
					tc.enable(false);
				}
			}
		}


		// show status
		if (tc.enable()) {

			if (run) {
				lcd.status(" run");
			}
			else {
				lcd.status("clap");
			}
		}
		else {
			lcd.status("init");
		}
	}
	// =============================================================
	// END OF RUN MODE
	// =============================================================



	// =============================================================
	// =============================================================
	// READ MODE
	// =============================================================
	// =============================================================
	else {


		// return to run mode
		if (millis() > (lastreadtime + READ_TIMEOUT)) {
			runMode = RUNMODE;

			start_timer1(tc.fps());
		}

		else {


			// mode changed to readmode
			if (boot | (runMode != lastRunMode)) {

				reader.reset();
				lastRunMode = runMode;
				lcd.clear();
			}


			// =========================================
			// disaple run timecode
			tc.enable(false);


			// check if reader is in sync
			if (reader.sync()) {

				// a timecode value is available
				if (reader.available()) {


					// get read timecode
					reader_tc.set(reader.get());

					led_tc.set(reader_tc.get());


					// =====================
					// display new framerate
					if (reader.fps_changed()) {
						lcd.fps(reader_tc.fps() + 1);
					}


					// =======================================
					// if just booted -> write timecode to rtc
					if (boot) {
						lcd.status(" jam");
						lcd.print("offset ", 0, 1);
						lcd.val8(reader_tc.offset(), 7, 1);

						// rtc not jet updated > do it
						if (!rtc_updated) {

							// get current rtc settings
							DateTime rtc_time = rtc.get();
							DateTime new_time;

							// set new time to rtc
							rtc.set(reader_tc.get().h, reader_tc.get().m, reader_tc.get().s, rtc_time.day(), rtc_time.month(), rtc_time.year());

							// write flags to flash store
							flash.write(reader_tc.get());

							// timecode update completed
							rtc_updated = true;
						}
					}

					else {
						lcd.status("sync");
						lcd.val8(reader_tc.offset(), 0, 1);
					}
				}
			}

			// not synced
			else {
				lcd.status("read");
			}
		}

	}
	// =============================================================
	// END OF READ MODE
	// =============================================================

}

// end of loop()



// =========================================
// INTERRUPT ROUTINES
// =========================================

// start bit timer interrupt
void start_timer1(uint8_t fps) {

	// =========================================
	// 25 * 80 Hz Interrupt
	cli();

	// Clear registers
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	// set inital cmr by framerate
	switch(fps) {
		case 24:
			OCR1A = TIMER_24; // 24 fpx
			break;
		case 25:
			OCR1A = TIMER_25; // 25 fps
			break;
		case 30:
			OCR1A = TIMER_30; // 30 fps
			break;
	}


	// CTC
	TCCR1B |= (1 << WGM12);
	// Prescaler 8
	TCCR1B |= (1 << CS10);
	// Output Compare Match A Interrupt Enable
	TIMSK1 |= (1 << OCIE1A);

	sei();
	// =========================================
}


// stop timer
// write 0 to clock select bits
void stop_timer1(void) {
	TCCR1B &= ~(1 << CS10);
	TCCR1B &= ~(1 << CS11);
	TCCR1B &= ~(1 << CS12);
}


// check if value is in array
// (needle, array, length)
bool in_array(uint8_t val, uint8_t* ary, uint8_t length) {

	for (uint8_t i = 0; i < length; i++) {

		if (val == ary[i]) {
			return true;
		}
	}

	return false;
}


// =========================================
// RTC interrupt every exact second
void syncISR() {

	// sync timecode frames to second start
	reader_tc.sync();

	tick = true;

	// micros for 1 second
	realtime = micros();
	cycletime = realtime - old_realtime;
	old_realtime = realtime;
}


// =========================================
// timecode read interrupt
void readISR(void) {

	// init read mode
	runMode = READMODE;
	stop_timer1();

	lastreadtime = millis();

	// read bit
	reader.read();
}


// =========================================
// timer 1 interrupt (16 bit) => 1/2 bit timecode
// framerate * 80
// timer 1
// 	24 fps => 3840 Hz; cmr = 4166
// 	25 fps => 4000 Hz; cmr = 4000
// 	30 fps => 4800 Hz; cmr = 3332
ISR(TIMER1_COMPA_vect) {

	long time;

	// tick signals the second from the rtc
	// the timecode can run in an frame offset
	timertime = tc.inc(tick);
}



/* Check for micros() rollover
if (currentMicros < lastMicros) {
elapsedMicros = (18446744073709551615ULL - lastMicros) + currentMicros + 1;
} else {
elapsedMicros = currentMicros - lastMicros;
}*/