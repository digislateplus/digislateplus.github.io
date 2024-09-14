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

#include <Arduino.h>
#include <RTClib.h>

#include "setup.h"

#include "lcd.h"
#include "led.h"
#include "rled.h"
#include "tc.h"
#include "rtc.h"
#include "reader.h"

LCD lcd;
LED led;
RLED rled;
TC tc;
RTC rtc;
READER reader;

bool clap;
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
int btnpressed, btnold, btncount;

byte h, m, s, f;      //hours, minutes, seconds, frame
byte buf_temp[8];     //timecode buffer
byte command, data, index;


enum {          // the states of the ISR state machine
	isrNull,
	isrInit,
	isrSync,
	isrRead
};

const uint8_t dfFlag = 0x04;          // in 'frames tens'


// vars shared with ISR
//
volatile uint8_t v_tcBuff[10];        // double-buffer for raw TC
volatile bool v_tcReady;              // indicates frame available
volatile bool v_tcRvs;                // true if TC was moving in reverse
volatile uint8_t v_isrState;          // initing -> syncing -> reading
volatile uint8_t v_tcFrameCtr;        // counter for v_tcFrameMax
volatile uint8_t v_tcFrameMax;        //  for imputing frame rate


bool m_tcDF;                          // DF flag was seen in raw bits

// run mode
// 		false => free run
// 		true => read
bool runMode;


// =========================================
//
void setup() {


	// set runMode to read
	runMode = false;
	clap = false;

	realtime = micros();
	old_realtime = realtime;


	// =============================================================
	// INIT timecode
	tc.begin();
	tc.set(0,0,0,0);
	tc.fps(25);


	// =============================================================
	// INIT IO
	pinMode(SIGNAL_OUTPUT,OUTPUT);
	pinMode(BUTTON,INPUT_PULLUP);


	// =============================================================
	// INIT flash led
	rled.begin(FLASH_LED);
	rled.on();


	// =============================================================
	// INIT LC-Display
	lcd.begin(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

	lcd.print(" DigiSlate-Plus", 0, 0);
	lcd.print("  initializing", 0, 1);


	// =============================================================
	// INIT LED-Display
	led.begin(LOAD_PIN);


	//display all zeros, and add decimal points to LSB
	led.set(88, 88, 88, 88);
	delay(250);
	led.set(tc.get());


	// =============================================================
	// end INIT sequence
	rled.off();
	lcd.clear();



	// =============================================================
	// start function
	// lcd.dir(false);
	lcd.fps(tc.fps());
	lcd.status(" run");

	// disp.print(uBits);


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


// debug => set time
// h,m,s,d,m,y 
// rtc.set(7,30,0,9,8,2024);


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

// // DEBUG
// lcd.clear();

	start_timer1(tc.fps());
}


// =========================================
//
void loop() {

	// get clapbar state
	// true = closed
	bool clapbar = !digitalRead(BUTTON);


	// =============================================================
	// const char hexchar[] = "0123456789ABCDEF";    // for uBits
	// static uint8_t rawTC[8];                      // snapshot of tc/ubits data
	// char tCode[12] = {"00:00:00:00"};      		// readable text
	// static char uBits[17] = {"UB:             "}; // 8 hex chars centered


	// =============================================================
	// no TC on input
	// free run mode
	if (runMode == false) {

		// =============================================================
		// update time if timecode has changed
		if (tc.changed()) {
			tc.unchange();


			// ===================================
			// slate is open
			// check for clap button
			if (!clapbar) {
				clap = false;
				run = true;
			}


			// ===================================
			// slate is closed
			// clapped
			else if (!clap) {

				run = false;
				clap = true;
				rled.flash(30);

				claptime = millis();
			}

			// clap is closed
			else {

				// check for timeout
				if (millis() > (claptime + CLAP_LONG_CLOSED)) {
					run = true;
				}

			}


			// ===================================
			// slate is closed for CLAP_LONG_CLOSED ms
			if (run) {

				if (tc.enable()) {
					led.set(tc.get());
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




	// =========================================
	// READ MODE
	// =========================================
	else {


		// return to run mode
		if (millis() > (lastreadtime + READ_TIMEOUT)) {
			runMode = false;

			start_timer1(tc.fps());
		}

		else {

			// disaple run timecode
			tc.enable(false);

			// check if reader is in sync
			if (reader.sync()) {

				lcd.status("sync");

				if (reader.available()) {

					TC temp; //= reader.get();
					temp.set(12,13,14,15);

					TIMECODE tctemp;
					tctemp = reader.get();

					led.set(tctemp);

					// lcd.val8(tctemp.h,0,1);
					// lcd.val8(tctemp.m,3,1);
					// lcd.val8(tctemp.s,6,1);
					// lcd.val8(tctemp.f,9,1);
				}
			}
			else {
				lcd.status("read");
			}

		}

		// DEBUG
		// rled.set(reader.sync());
	}
}    // end of loop()



// =========================================
// flip byte order for backwards reading
uint8_t flip8(uint8_t b) {

	// reverses the bit order within a byte

	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}


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
			OCR1A = 4166; // 24 fpx
			break;
		case 25:
			OCR1A = 4000; // 25 fps
			break;
		case 30:
			OCR1A = 3332; // 30 fps
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


// =========================================
// RTC interrupt every exact second
void syncISR() {

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
	runMode = true;
	stop_timer1();

	lastreadtime = millis();

	// read bit
	if (reader.read()) {

		// complete frame read
		// display timecode
	}

	// rled.set(true);
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