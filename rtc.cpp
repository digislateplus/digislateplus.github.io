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
      _rtc->adjust(DateTime(F(__DATE__), F(__TIME__)));
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