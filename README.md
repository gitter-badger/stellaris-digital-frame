stellaris-picture-viewer
========================

This is an implementation of a "Digital picture frame"
for the Stellaris LM3S9B92 evaluation board.

The projcet files are for Code Composer Studio from TI.

The code can be used freely with no restrictions.

Featuring:
- a real time clock showing on the top left of the screen at all times.
- a slideshow of the pictures on the sdcard
- choose a picture and show it

Dependencies:
- Texas Instruments Graphics lib
- Texas Instruments Driver Lib
Both can be freely downloaded and used from Texas Instruments

Peripherals used in the project:
- SDCARD - SPI interface
- LCD TFT - with controller ssd1289
- Touch screen - with controller XPT2046/ADS7843
- RTC - DS3234

This application is easy port to your configuration.
