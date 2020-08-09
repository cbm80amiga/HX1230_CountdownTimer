# HX1230_CountdownTimer
LowPower Arduino Countdown Timer

YouTube video:

https://youtu.be/rDXpSH2rGX0


## Features:
- the timer is always enabled and remains ready in deep sleep mode
- low power modes used as often as possible to save energy
- 500mAh LiPo cell should last for 2-3 years depending of how often the timer will be actually used
- backlight typically is used only for 15 seconds, can be removed completely
- deep sleep counter is used in countdown mode to save energy, it is not very accurate,
  can be replaced by regular sleep in case of issues
- could't decide which font should be used so finally there are 4 looks (4 fonts, 2 progress indicators)

## BoM:
- Arduino Pro Mini
- HX1230 96x68 pixel LCD
- button
- buzzer or mini speaker
- 100ohm resistor - 2 pcs

## To consider:
- buzzer/speaker should be connected via transistor
- lithium battery charging PCB could be considered


## Connections

|HX1230 pin|Pin name| Arduino|
|--|--|--|
|#1| RST    |D6 or any digital|
|#2| CE  |D7 or any digital|
|#3| N/C     ||
|#4| DIN | D11/MOSI |
|#5| CLK  |D13/SCK|
|#6| VCC| 3.3V|
|#7| BL| D8 via 100ohm resistor|
|#8 |GND|GND|


The buzzer is connected directly to D4 via 100ohm resistor, but using extra transistor is recommended
  
|Pin| Arduino|
|--|--|
|buzzer +|D4 via 100ohm resistor|
|buzzer -|GND|

The button is connected between D3 and GND
  
If you find it useful and want to buy me a coffee or a beer:

https://www.paypal.me/cbm80amiga
