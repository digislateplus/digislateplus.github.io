# MAX Electronics  
### SMPTE DigiSlate Model:2SWF9  
Rev: 1.0  
Bill Of Material

https://m.youtube.com/watch?v=TnaWQZtmPek&pp=ygUYbWF4ZWxlY3Ryb25pY3MgZGlnaXNsYXRl

## Resistors:  
|Nr|value|package|Lieferant|  
|--|-----|-------|---------|
|R1,4,5,6|10K|0805|**ok**|
|R2,3|10K|1206|**ok**|
|R9-12|100R|1206|**ok**|
|R13-15|470R|1206|**ok**|
|R16|360R|1206|**ok**|
|R17-19|1K|0805|**ok**|
|R20|15K|1206|**ok**|
|R21|1.5K|1206|**ok**|
|R22|2.2K|1206|**ok**|
|R23|2.7K|1206|**ok**|
|RV1|20K|TS53YJ|**ok**|
|RV3|10K|TS53YJ|**ok**|
|RV2|10K|3296W|**ok**|

## Ceramic Caps:  
|Nr|value|package|Lieferant|  
|--|-----|-------|---------|
|C1,3,4|100nF/50v|0805|**ok**|
|C5|100nF/50v|1206|**ok**|
|C2|100nF/50v|5mm|**ok**|
|C9,10,12|10nF/50v|1206|**ok**|
|C13|22uF/50v|1210|**ok**|
|C14|1uF/50v|0805|**ok**|

## Electrol. Caps:  
|Nr|value|package|Lieferant|  
|--|-----|-------|---------|
|C15,16|220uF/25v|8x10|**ok**|

## Tantalum Caps:  
|Nr|value|package|Lieferant|  
|--|-----|-------|---------|
|C17|470uF/10v|Kemet-D|**ok**|
|C18,19|100uF/10v|Kemet-D|**ok**|
|C20,21|100uF/10v|Kemet-T|**ok**|

## LED/Diodes: 
|Nr|value|package|Lieferant|  
|--|-----|-------|---------|
|D1,2|SS310|SMA|**ok**|
|D3|1N4148|1206|**ok**|
|LD1,2|Red Clear|10mm|**ok**|
|LD3-8|Red Dif Flat|3mm|**ok**|
|LD9|R/G, Com-A|3mm|**ok**|

## Displays:  
|Nr|value|package|Lieferant|  
|--|-----|-------|---------|
|HR1,2|12101AS|7Segment|**ok**|
|MN1,2|12101AS|7Segment|**ok**|
|SC1,2|12101AS|7Segment|**ok**|
|FR1,2|12101AS|7Segment|**ok**|
|DS1|WC1602A|Dot-Mtrx|**ok**|

## IC’s:  
|Nr|value|package|Lieferant|  
|--|-----|-------|---------|
|U1|Atmega 328p-AU|TQFP-32|**ok**|
|U2|MAX7219|SOIC-24W|**ok**|
|U3|MT3608|SOT-23-6|EBay|
|U4|78M05|TO-252-2|**ok**|
|U5|LM2596S|TO-263-5|**ok**|
|U6|TP4056|SOP8|lagernd|
|U8|PC817|DIP4|**ok**|
|Q1-4|MMBT2222 (BC337)|SOT-23|**ok**|

## Inductor:  
|Nr|value|package|Lieferant|  
|--|-----|-------|---------|
|L1|22uH|7.3x7.3mm|**ok**|
|L2|68uH|7.3x7.3mm|**ok**|

## Miscellaneous:  
|Nr|value|package|Lieferant|  
|--|-----|-------|---------|
|CN1,2|2Pin Molex|P:2.54mm|**ok**|
|CN3|BNC|Panel MT|fehlt|
|CN4|2x3 Header|P:2.54mm|**ok**|
|CN5|Barrel Jack|5.5x2.1mm|**ok**|
|CN6|3.5 TRS Jack|SJ1-3525N|**ok**|
|SW1|1x2-Header-P:2.54mm||**ok**|
|SW2|NO Reed Switch|P:25mm|fehlt|
|Y1|Crystal-16Mhz|CSTxExxV|**ok**|

# Software
https://github.com/mitkunz/diy_timecode_generator

# Hardware-Anpassungen
Zusätzliches RTC-Modul DC3231
- Atmega Pin 1 (PD3/D3) => Pin 26 (PC3/D17) - WC1602C Pin 14 (D7) (ändern der LCD-Library)
- Atmega Pin 1 (INT1) => DC3231 SQW
- Atmega Pin 27 (SDA) => DC3231 SDA
- Atmega Pin 27 (SCL) => DC3231 SCL
# Interrupt 
**timer 1 settings**
i=interrupt frequency (Hz) = (Arduino clock speed c=16,000,000Hz) / (p=prescaler * (c=compare match register + 1))

compare match register = [ 16,000,000Hz/ (prescaler * desired interrupt frequency) ] - 1

timer1, prescaler 1

24fps => 3840Hz => 260us; cmr=4166
25fps => 4000Hz => 250us; cmr=4000
30fps => 4800Hz => 208us; cmr=3332