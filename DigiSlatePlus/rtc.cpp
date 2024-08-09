#include "rtc.h"


// start RTC service
int8_t RTC::begin(uint16_t int_port) {

  // set interrupt port
  _int_port = int_port;

  // init RTC
  _rtc = new RTC_DS3231();
  _status = true;

  // try to start RTC
  if (! _rtc->begin()) {
    _status = false;
  }

  // RTC found -> activate
  else {
    // power loss?
    if (_rtc->lostPower()) {
      // _rtc->adjust(DateTime(F(__DATE__), F(__TIME__)));
      _status = -1;
    }

    // activate 1 Hz interrupt output
    _rtc->writeSqwPinMode(DS3231_SquareWave1Hz);
  }  

  return status();
}


// get DateTime
DateTime RTC::get(void) {
  return _rtc->now();
}

// set time to rtc
// h, m, s, d, m, y
void RTC::set(uint16_t  hr, uint16_t  min, uint16_t  sec, uint16_t  day, uint16_t  month, uint16_t  yr) {
    set(DateTime(yr, month, day, hr, min, sec));
}

// set DateTime
void RTC::set(DateTime dt) {
  _rtc->adjust(dt);
}

// get Temperature
float RTC::temperature(void) {
  return _rtc->getTemperature();
}


// return status
int8_t RTC::status(void) {
	return _status;
}