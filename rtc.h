#ifndef RTC_H
#define RTC_H

#include "RTClib.h"

/*
 Date and time functions using a DS3231 RTC connected via I2C and Wire lib

 class reference
 https://adafruit.github.io/RTClib/html/class_r_t_c___d_s3231.html
 */


#define RTC_ONLINE 0
#define RTC_POWER_LOSS 1
#define RTC_OK 1

class RTC {

public:
	int8_t begin(uint16_t int_port);
	DateTime get(void);
	void set(DateTime);
	float temperature(void);
	int8_t status(void);

private:
	RTC_DS3231* _rtc;
	uint8_t _status;
	uint16_t _int_port;
};

#endif