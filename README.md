# DigiSlate++

The project is in an early developement state.

DigiSlatePlus is inspired by the [DigiSlate](https://www.youtube.com/watch?v=TnaWQZtmPek) and the DIY timecode generator from [mitkunz](https://github.com/mitkunz/diy_timecode_generator). The aim for the DigiSlate++ is not only to be able to read and display timecode, but also to generate it itself. To make this possible, the hardware is extended by a real time clock with high running accuracy. The RTC module DS3231 with temperature-compensated quartz offers the possibility.

The DigiSlate++ also offers an additional LED display on which numbers and texts can be shown. It is intended to show the scene and take numbers. The LC display on the back is extended with a pure rotary encoder and some buttons to be able to address additional functions.

In order to realise this, a new hardware design was necessary that combines all components. An ESP32S3 is used as the processor, which also offers a USB interface in addition to Bluetooth and WiFi.
