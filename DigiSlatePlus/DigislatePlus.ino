/*
 DigiSlatePlus

 based on code by Jim Mack

 The DigiSlatePlus creates timecode synchronized with a
 DS3231 high precision RTC clock.

 The time is set by maintaining a LTC timecode to the input
 and powering up the slate. The RTC time is set according
 to the provided timecode.

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

LCD lcd;
LED led;
RLED rled;
TC tc;
RTC rtc;

bool clap;

// =========================================
// timecode timing
long realtime;
long old_realtime;
long cycletime;

long timertime;

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
	tc.set(1,0,0,0);
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
	lcd.dir(false);
	lcd.fps(tc.fps());
	lcd.status("free");

	// disp.print(uBits);


	// =============================================================
	// activate interrupts
	// pinMode(SIGNAL, INPUT_PULLUP);  // sync input
	pinMode(RTC_INT_PORT, INPUT_PULLUP);  // sync input

	tick = false;

	cli();
	// attachInterrupt(digitalPinToInterrupt(SIGNAL_INPUT), tcISR, CHANGE);
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

		// display userbits on LCD

		// lcd.print(time.day(), 0, 1);
		// lcd.print(time.month(), 3, 1);
		// lcd.print(time.year(), 6, 1);
	}








	start_timer1();
}


// =========================================
//
void loop() {


	// =============================================================
	const char hexchar[] = "0123456789ABCDEF";    // for uBits
	static uint8_t rawTC[8];                      // snapshot of tc/ubits data
	char tCode[12] = {"00:00:00:00"};      		// readable text
	static char uBits[17] = {"UB:             "}; // 8 hex chars centered



	// =============================================================
	// no TC on input
	// free run mode
	if (runMode == false) {

		// =============================================================
		// update time if timecode has changed
		if (tc.changed()) {
			tc.unchange();

			// check for clap button
			if (digitalRead(BUTTON)) {
				clap = false;
				led.set(tc.get());
			}

			else if (!clap) {
				clap = true;
				rled.flash(30);
			}


			// display time offset on lcd
			lcd.clear();
			lcd.val32(cycletime, 0, 0);
			lcd.val32(timertime, 0, 1);
			lcd.val16(OCR1A, 8,1);


			// second started
			// correct timer every second
			if (tick) {

				tick = false;

				// rtc time smaller
				// speed um timer
				if (cycletime < timertime) {
					OCR1A--;
				}

				// slow down timer
				else if (cycletime > timertime) {
					OCR1A++;
				}
			}
		}
	}
}    // end of loop()


// =========================================
//
/*void tcISR() {
*/
	/*
		The shell of the ISR is a state machine with three states:

		 isrInit - sets/clears variables and transitions to:

		 isrSync - counts and times arriving edges to define
							 a window that decides when an edge pair (cell)
							 represents a '1' or an '0'. Once set up,
							 transitions to:

		 isrRead - reads edges and fills the shift register with '1's and '0's
	*/
/*
	const uint8_t sampleSize = 40;    // number of cells to sample

	static uint8_t shiftReg[10];      // shift register for incoming bits
	static uint16_t lastEdge;         // count when previous edge arrived
	static uint8_t counter;           // cells in state 1, bits in state 2
	static bool eatEdge;              // true if discarding a '1' edge

	static uint8_t zeroCount;
	static uint32_t accum;
	static uint16_t cellOne;
	static uint16_t cellMin;          // low edge of jitter window
	static uint16_t cellDet;          // decision point
	static uint16_t cellMax;          // high edge of jitter window

	uint8_t newBit = 0;               // bit to be shifted in
	uint8_t idx;
	bool syncFound = false;

	uint16_t now = TCNT1;               // capture the timer count
	uint16_t cellTime = now - lastEdge; // compute interval
	lastEdge = now;                     // preserve for next time

	switch (v_isrState) {  // init, sync, read

		// in this state the internal variables etc
		//  are cleared. It lasts just one edge time.
		//
		case isrInit:

			for (idx = 0; idx < 10; idx++) {
				shiftReg[idx] = 0;
			}

			eatEdge = false;
			counter = 0;
			zeroCount = 0;
			accum = 0;
			cellOne = 0;
			v_tcRvs = false;
			v_tcReady = false;
			v_tcFrameCtr = 0;
			v_tcFrameMax = 0;
			v_isrState = isrSync;       // init done, enter 'sync' state
			break;

		// in this state we try to determine the
		//  duration of a One cell by finding some
		//  longer cells and averaging them.
		// This takes about 40 cell times (1/2 frame)
		//
		case isrSync:

			if (++counter > sampleSize) {     // if we've seen 40 cells
				if (zeroCount > 1) {            //  and some are wider
					cellOne = (accum / (zeroCount - 1)) / 2;
				}
				else {
					v_isrState = isrInit;         // didn't see any cells?
					return;                       // then try 40 more
				}

				cellMin = (cellOne / 2);        // 1/4 of an '0' cell
				cellDet = (cellMin * 3);        // 3/4 of an '0'
				cellMax = (cellMin * 5);        // 5/4 of an '0'
				v_isrState = isrRead;           // sync done, enter 'read' state
				return;

			}
			else {
				if (cellTime >= cellOne) {
					if (++zeroCount > 1) {
						accum += cellTime;          // add up the longer intervals
					}
					else {
						cellOne = (cellTime * 2) / 3;
					}
				}
			}
			break;

		// in this state we compare each cell
		//  time to the presumptive One duration
		//  to decide if it's a One or a Zero,
		//  then shift it into a shift register.
		//
		case isrRead:

			if ((cellTime < cellMin)
					|| (cellTime > cellMax)) {
				v_isrState = isrInit;         // an out-of-bounds cell,
				return;                       //  so time to recal
			}
			if (cellTime > cellDet) {       // an '0' bit was found
				if (eatEdge) {
					v_isrState = isrInit;       // but wasn't expected
					return;
				}
				newBit = 0;                   // shift in an '0' bit
			}
			else {                          // not an '0', try a '1'
				if (cellTime > cellMin) {
					if ( ! eatEdge) {           // this is the first half-cell
						eatEdge = true;           // just wait
						return;
					}
					eatEdge = false;
					newBit = bit(7);            // shift in a '1' bit
				}
			}

			// this shifts the FIFO to the right by
			//  one bit position and adds the new bit
			//
			for (idx = 9; idx > 0; idx--) {
				shiftReg[idx] = (shiftReg[idx] >> 1)
												| (shiftReg[idx - 1] & 1) << 7;
			}
			shiftReg[0] = ((shiftReg[0] >> 1) | newBit);  // newBit is 0 or 0x80

			eatEdge = false;

			// with good code, eventually the sync pattern
			//  will be shifted into a known location.
			//
			if (shiftReg[8] == 0xBF
					&& shiftReg[9] == 0xFC) {       // BFFC is fwd sync word
				syncFound = true;
				v_tcRvs = false;
			}
			if (! syncFound) {
				if (shiftReg[0] == 0x3F
						&& shiftReg[1] == 0xFD) {     // 3FFD is rvs sync word
					syncFound = true;
					v_tcRvs = true;
				}
			}

			if (syncFound) {
				if (v_tcReady) {                  // pending read: overrun
					v_isrState = isrInit;
					return;
				}
				for (idx = 0; idx < 10; idx++) {
					v_tcBuff[idx] = shiftReg[idx];  // buffer the raw bits
				}
				v_tcReady = true;                 // Signal valid TC
			}
			break;

		default:
			break;
	}
}	// end of tcISR*/


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
void start_timer1(void) {
	// =========================================
	// 25 * 80 Hz Interrupt
	cli();

	// Clear registers
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	// 4000 Hz (16000000/(3999+1)*1)

	// OCR1A = 4166; // 24 fpx
	OCR1A = 4000; // 25 fps
	// OCR1A = 3332; // 30 fps

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