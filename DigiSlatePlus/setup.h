/*
 DigiSlatePlus setup file
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

#ifndef SETUP_H
#define SETUP_H


// framerates
#define DEFAULT_FRAMERATE 25
static const uint8_t framerates[3] = {24,25,30};


// runmodes
#define RUNMODE 0
#define READMODE 1


// setup
#define SETTINGS_MAX_STATI 5
#define SETTINGS_ENTER_TIMEOUT 2000
#define SETTINGS_ENTER_REPEATS 2

static const char* status_text[SETTINGS_MAX_STATI] = {"hour  ","minute","second","frames","fps   "};


// timecode data settings
#define FLASH_FPS 0
#define FLASH_OFFSET 1
#define FLASH_FLAGS 2
#define FLASH_USERBITS 3

#define FLASH_FLAGS_DROPFRAME 0
#define FLASH_FLAGS_COLORFRAME 1
#define FLASH_FLAGS_BIPHASE 2
#define FLASH_FLAGS_FLAG0 3
#define FLASH_FLAGS_FLAG1 4


// timer settings
#define ENABLE_LIMIT 1000		// ms for stable sync
#define CLAP_DENOISE 100		// ms
#define CLAP_LONG_CLOSED 2000	// time with closed clap to enable tc display 

#define READ_TIMEOUT 500		// time without input to switch to run mode

#define TIMER_24 4166			// basic timer setting for 24 fps
#define TIMER_25 4000			// basic timer setting for 25 fps
#define TIMER_30 3332			// basic timer setting for 30 fps


// IO definitions
#define FLASH_LED A0	// flash led output
#define BUTTON A1		// clap reed button input

#define SIGNAL_OUTPUT 0 // TC output to BNC
#define SIGNAL_INPUT 2	// INT0 / for audio input
#define RTC_INT_PORT 3  // INT1 / RTC sync input


// lc display pinout
#define LCD_RS 9
#define LCD_EN 8
#define LCD_D4 6
#define LCD_D5 5
#define LCD_D6 4
#define LCD_D7 17


// led display pinout
#define LOAD_PIN 7    //Load/CS pin


// led register settings
#define LED_F1 0x02
#define LED_F10 0x06
#define LED_S1 0x08
#define LED_S10 0x04
#define LED_M1 0x03
#define LED_M10 0x07
#define LED_H1 0x05
#define LED_H10 0x01


#endif