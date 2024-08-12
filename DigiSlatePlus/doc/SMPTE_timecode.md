# SMPTE/EBU timecode structure
[WikiAudio](https://www.wikiaudio.org/smpte-time-code/)

| Byte | Bit | content|
|------|-----|--------|
| 0 | 0-3 |	frame units |
| 0 | 4-7 | user bits 1 |
| 1 | 8-9 | frame tens |
| 1 | 10  | dropframe bit*(1)* |
| 1 | 11  |	colorframe bit*(2)* |
| 1 | 12-15 | user bits 2 |
| 2 | 16-19 | seconds units |
| 2 | 20-23 | user bits 3 |
| 3 | 24-26 | seconds tens |
| 3 | 27 | bi phase mark correction bit |
| 3 | 28-31 | user bits 4 |
| 4 | 32-35 | minutes units |
| 4 | 36-39 | user bits 5 |
| 5 | 40-42 | minutes tens |
| 5 | 43 | binary group flag bit*(3)* |
| 5 | 44-47 | user bits 6 |
| 6 | 48-51 | hours units |
| 6 | 52-55 | user bits 7 |
| 7 | 56-57 | hour tens |
| 7 | 58 | unused, reserved(*4*) |
| 7 | 59 | binary group flag bit*(5)* |
| 7 | 60-63 | user bits 8 |
| 8-9 | 64-79 | sync word*(6)* |

(1) 1=drop frame (frame 0 and 1 omitted from first second of each minute, but included when minutes divides by ten; approximates 29.97 frame/s)

(2) i.e. the time code is intentionally synchronised with a color TV field sequence.

(3) (with bit 59, 43,59 = 00 = no format for user bits, 10 = eight bit format, 01, 11 are unassigned and reserved).

(4) should transmit zero and ignore on receive for compatibility

(5) see bit 43 for encoding

(6) 0011 1111 1111 1101

# Signal
After every bit the output is inverted. A logical 1 changes the polarity in the middle of the bit clock. The data is sent starting with the least significant bit of each byte.

The interrupt frequence depends on the framerate and is controlled by the compare register of timer 1 from the Atmega328.

- 24 fps => 3840 Hz; cmr = 4166
- 25 fps => 4000 Hz; cmr = 4000
- 30 fps => 4800 Hz; cmr = 3332

The initial settings of the cmr value are synchroniced by measuring two times. The actual time that passed between two interrupts from the realtime clock is compared with a  sequence to send timecode, controlled by the timer 1 interrupt. The compare register is increated, if the timecode output is too fast, and decreases the value, if the timecode is too slow. So the accuracy of the timecode output is privided.