/*
 DigiSlatePlus

 based on code by Jim Mack

 The DigiSlatePlus creates timecode synchronized with a
 DS3231 high precision RTC clock.

 The time is set by maintaining a LTC timecode to the input
 and powering up the slate. The RTC time is set according
 to the provided timecode.

 When no timecode is found on the BNC connector, the internal
 generated timecode is available on the BNC instead.
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



// =========================================
// timecode timing
long realtime;
long old_realtime;
uint16_t cycletime;
bool tick;


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
long curr_time;
long old_time;


// =========================================
//
void setup() {


	// set runMode to read
	runMode = false;

	// DEVEL
	curr_time = millis();
	old_time = curr_time;


	// =============================================================
	// INIT timecode
	tc.set(1,0,0,0);
	tc.fps(25);


	// =============================================================
	// INIT IO
	pinMode(BUTTON,INPUT_PULLUP); 
	digitalWrite(BUTTON,0);


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



	// display rtc on LCD
	DateTime time = rtc.get();
	tc.set(time.hour(), time.minute(), time.second(), 0);

	// lcd.print(time.day(), 0, 1);
	// lcd.print(time.month(), 3, 1);
	// lcd.print(time.year(), 6, 1);





/*	// INIT TC-input
	pinMode(SIGNAL_INPUT, INPUT_PULLUP);  // conditioned TC input

	// =============================================================
	// INIT timer for interrupt
	cli();

	TCCR1A = 0;             // normal count-up mode
	TCCR1B = bit(CS11);     // prescaler / 8
	TCNT1 = 0;              // start at 0

	sei();

	v_isrState = isrInit;

	attachInterrupt(digitalPinToInterrupt(SIGNAL_INPUT), tcISR, CHANGE);
	attachInterrupt(digitalPinToInterrupt(SYNC_INPUT), syncISR, CHANGE);
*/

	// TCCR1A = 0;             // normal count-up mode
	// TCCR1B = bit(CS11);     // prescaler / 8
	// TCNT1 = 0;              // start at 0


	noInterrupts();
	// Clear registers
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	// 25 Hz (16000000/((624+1)*1024))
	OCR1A = 624;
	// CTC
	TCCR1B |= (1 << WGM12);
	// Prescaler 1024
	TCCR1B |= (1 << CS12) | (1 << CS10);
	// Output Compare Match A Interrupt Enable
	TIMSK1 |= (1 << OCIE1A);
	interrupts();
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
			led.set(tc.get());
		}
	}


	// =============================================================
	// TC on input
	// read mode
/*	else {

		// check button
		btnpressed = !digitalRead(BUTTON);

		if(btncount < 1000) {
			if(btnpressed == 1) {
				btncount++;
			}
		}

		if(btncount >= 1) {
			if(btnpressed == 0) {
				btncount--;
			}
		}

		if (btncount < 1000) {
			if( btnpressed == 1 ) {
				if(btnpressed !=  btnold  ){
					btncount =  10000;

					digitalWrite(RLED, 1); delay(30);
					digitalWrite(RLED,0);
				}
			}
		}

		//  if( btnpressed == 0 ){
		//   digitalWrite(RLED,0);
		// }


		if(btnpressed == 1) {
			if(btncount >= 1000) {
				btnold = 1;
			}
		}

		if(btncount < 10) {
			if(btnpressed == 0) {
				btnold = 0;
			}
		}


		// =============================================================
		int idx;

		if (v_tcReady) {                      // move TC bits from the buffer
			if (!v_tcRvs) {                     //  correctly for fwd or rvs
				for (idx = 0; idx < 8; idx++) {
					rawTC[idx] = (v_tcBuff[idx]);
				}
			}
			else {
				for (idx = 0; idx < 8; idx++) {           // for reverse timecode,
					rawTC[idx] = flip8(v_tcBuff[9 - idx]);  //  also reverse the bits
				}
			}

			// now rawTC[] has the bits in forward order
			// whether the code was read forward or reverse

			v_tcReady = false;                          // tell ISR it's handled

			m_tcDF = (rawTC[6] & dfFlag);               // test the DF bit

			tCode[8] = m_tcDF ? ';' : ':';              // choose sec/frm separator

			tCode[0] =  (rawTC[0] & 0x03) | '0';        // hours tens
			tCode[1] =  (rawTC[1] & 0x0F) | '0';        // hours units
			tCode[3] =  (rawTC[2] & 0x07) | '0';        // minutes tens
			tCode[4] =  (rawTC[3] & 0x0F) | '0';        // minutes units
			tCode[6] =  (rawTC[4] & 0x07) | '0';        // seconds tens
			tCode[7] =  (rawTC[5] & 0x0F) | '0';        // seconds units
			tCode[9] =  (rawTC[6] & 0x03) | '0';        // frames tens
			tCode[10] = (rawTC[7] & 0x0F) | '0';        // frames units


			// =============================================================
			// display TC on LED
			uint8_t bb = (rawTC[7] & 0x0F) | '0';  
			display_write(0x02,bb); //enable decimal point on LSB  //frame

			bb = (rawTC[6] & 0x03) | '0';
			display_write(0x06, bb);             // frame
			bb = (rawTC[5] & 0x0F) | '0';   
			display_write(0x08, bb);               // sec
			bb = (rawTC[4] & 0x07) | '0';   
			display_write(0x04, bb);                 // sec
			bb = (rawTC[3] & 0x0F) | '0';   
			display_write(0x03, bb);                 // min             
			bb = (rawTC[2] & 0x07) | '0';  
			display_write(0x07, bb);                 // mib
			bb = (rawTC[1] & 0x0F) | '0';    
			display_write(0x05, bb);                 // hour
			bb =  (rawTC[0] & 0x03) | '0'; 
			display_write(0x01, bb);                 // hour

			//  //recalculate timecode once FRAMES LSB quarter-frame received
			//   h = (buf_temp[7] & 0x01)*16 + buf_temp[6];
			//   m = buf_temp[5]*16 + buf_temp[4];
			//   s = buf_temp[3]*16 + buf_temp[2];
			//   f = buf_temp[1]*16 + buf_temp[0];
			//   display_timecode();
		

			for (idx = 0; idx < 8; idx++) {
				uBits[idx + 4] = hexchar[rawTC[idx] >> 4];  // user bits little endian
			}

			uint8_t frVal = ((tCode[9] & 0x03) * 10)
											+ (tCode[10] & 0x0F);       // impute the frame rate
			if (frVal > v_tcFrameMax) {                 //  by finding the highest
				v_tcFrameMax = frVal;                     //  frame number seen in
			}
			if (++v_tcFrameCtr > 31) {                  //  the last 31 frames
				tcRate = v_tcFrameMax + 1;
				v_tcFrameMax = 0;
				v_tcFrameCtr = 0;                         // do this every 31 frames
			}


			// =============================================================
			//display TC on LCD
			disp.home();

			disp.print(tCode);
			disp.write(arroz[v_tcRvs]);

			if (m_tcDF) {
				disp.print("DF");
			}
			else {
				disp.print(tcRate);
			}

			disp.setCursor(0, 1);
			disp.print(uBits);
		}
	}
*/


}    // end of loop()


// =========================================
//
void tcISR() {

	/*
		The shell of the ISR is a state machine with three states:

		 isrInit - sets/clears variables and transitions to:

		 isrSync - counts and times arriving edges to define
							 a window that decides when an edge pair (cell)
							 represents a '1' or an '0'. Once set up,
							 transitions to:

		 isrRead - reads edges and fills the shift register with '1's and '0's
	*/

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
}	// end of tcISR


// create timecode
// RTC interrupt
void syncISR() {

	tick = true;

	// micros for 1 second
	cycletime = realtime - old_realtime;
	realtime = micros();

	// rled.flash(30);
}

// timer 1 interrupt
ISR(TIMER1_COMPA_vect) {

	// inc
	// on frame overflow, wait till second has started
	tc.inc(tick);
}


// =========================================
//
uint8_t flip8(uint8_t b) {

	// reverses the bit order within a byte

	b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
	b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
	b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
	return b;
}