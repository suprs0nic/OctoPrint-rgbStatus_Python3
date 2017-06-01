* OctoPrint RGB Status

** An OctoPrint plugin to show the state of the 3D printer using RGB(W) leds.

This plugin extends OctoPrint with functionality to display its state through RGB(W) LEDs. The status colors of the LEDs are configurable through OctoPrint's settings file. During runtime, 12-bit color data for each channel is sent over SPI. The plugin has only been tested on a Raspberry Pi with a TLC5947 PWM controller. The plugin uses a Python C++ extension for optimal performance.