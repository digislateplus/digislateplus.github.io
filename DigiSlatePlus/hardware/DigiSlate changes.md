# Changes to the original DigiSlate design

This project is based on the [DigiSlate by Max Christensen](https://www.youtube.com/watch?v=TnaWQZtmPek). Some adaptations have been made to add the timecode generator function, which are listed here.

## RTC-Module
A DS3231 real-time clock module has been added to the bottom right of the PCB (seen from the front). Five isolated areas in the GND area on both sides of the board have been scratched free. The RTC module can be soldered in through six holes, whereby the GND connection is located directly in the GND area. The VCC is provided from C20 with a short wire. The 32k output of the RTC keeps unused.

## Atmega328
In order to connect the RTC module, some changes must be made around the Atmega328 microcontroller.

### Interrupt
Pin 1 (PD3/INT1) is required for the interrupt of the RTC interrupt from the squarewave output (SQW). The pin is used as D7 line for the LC display and must be changed. The trace can be cut directly next to the Atmega pin before the through-connection. A wire connects the SQW output of the RTC to pin 1.

Pin 17 (D17/PC3) is unused and can be connected directly to pin 14 of the LCD module with a short wire. In the LCD class, D7 has be routed to output 17 (see setup.h).

### I2C
The SDA and SCL connectors of the rtc module are connected to Pin 27 (SDA) and Pin 28 (SCL) of the Atemga328 using thin wires.

## 3,5mm Jack
The trigger out is scipped and used as output of the timecode signal with a audio line level.

The U8 opto coupling chip is removed. A 5k6 resistor is soldered between pins 1 and 2 of  jumper 1 and pin 3 of the former opto coupler on the backside of the PCB. On the front side a 1k8 resistor connects pin 3 of the former opto coupler with the GND pin of the Jach connector. This creates a voltage devider the provides a signal of aroun 1 Volt on the output.

The pin 30 (D0/PD0) of the Atmega is used as output for the timecode (see setup.h).