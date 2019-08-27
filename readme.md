# M5Stack timer

[tocstart]: # (toc start)

  * [Introduction](#introduction)
  * [Setting up](#setting-up)
  * [Requirements](#requirements)
  * [Developer Notes](#developer-notes)
  * [License](#license)

[tocend]: # (toc end)

## Introduction

This little sketch provides you a digital countdown/timer with an adjustable period from 5 seconds to almost 100 minutes.
It will require a M5Stack basic module as thats hardware is about absolutely necessary, but could be adapted to other platforms.

## Setting Up

Take a look at config.h - this will not be a complete reference; if you need more, please take a look at the sources.

You can adapt the used colours, colours and the step of increasing/decreasing the seconds value (default: by 5).

## Requirements

What do you need to "use" this sketch?

* An M5Stack module
  I developed it with the basic version (https://m5stack.com/collections/m5-core/products/basic-core-iot-development-kit).

* The Arduino IDE
  Well, it may work with the ESP IDF, but could not provide any help as i am not using that.

* The ESP32 Board Manager (https://github.com/espressif/arduino-esp32)
  There are others and i would expect them to work, too - but i simply did not check that. 
  Bottom line: If you get strange compilation issues, give that one a try.
  
* The library framework provided by M5Stack, a library simply called "M5Stack" (https://github.com/m5stack/m5stack)

* optional(!): M5Stack SD Updater (https://github.com/tobozo/M5Stack-SD-Updater/)
  This is neither required nor related to the sketchs purpose; i just sense it to be very handy as it enables you to "upload" sketches just by putting the binaries onto the SD card - with very little footprint in the sketch (source).
  Feel free to disable it by commenting out / removing the #include "M5StackUpdater.h" in the beginning of timer.ino.
  If you want to use it, the repository also contains a json and jpeg file meant to be used with that - copy them to the json/jpeg directory of your SD card. As usual, if you press the leftmost button during booting the M5Stack, that menu is run "instead of" this sketch.

## Developer Notes

As i am relying on the special lib for the M5Stack hardware (providing simple access to the buttons, display and "speaker"), changing to other hardware will not be too simple.

There is no real config to enable/disable the M5Stack SD Loader - you can simply (un)comment thats include in the top of the main file (timer.ino); the usual startup check for it is made depending of the #define of that include. So don't disable it by copying that define the sketch!

## License

The MIT License (MIT)

Copyright (c) 2019 by Arnold Schommer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
