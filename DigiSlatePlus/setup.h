#ifndef SETUP_H
#define SETUP_H


// IO definitions
#define FLASH_LED A0	// flash led output
#define BUTTON A1		// clap reed button input
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