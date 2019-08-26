/*

 simple timer / countdown
 for: M5Stack

 (c) Arnold Schommer 2019

 v0.0: start of development
 v0.5: first partially useful version
 v0.7: less flicker due to less re-drawings
 v0.8: inherit "Int16Widget" from "Widget" - instead of including a Widget-instance into Int16Widget
 v0.9: fix some bugs concerning restart
 v0.9.2: make reDraw() virtual to unify redrawing of all widgets
 v1.0: re-arranging the main widgets (minutes, seconds) - (partially) dynamic
 v1.1: added saving the "selected" time to NVS (using <Preferences>)
 
 */
/*
 *  MIT License
 *  
 *  Copyright (c) 2019 Arnold Schommer
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#define PROJECT "M5Stack timer"
#define VERSION "1.1"

#include <M5Stack.h>        // https://github.com/m5stack/M5Stack
#include "M5StackUpdater.h" // https://github.com/tobozo/M5Stack-SD-Updater
#include <Preferences.h>
#include "config.h"     // after M5Stack.h because it references colors defined there
#include "widgets.h"    // buttonlike gui elements

//*************************************
// global vars

Widget *AllWidgets[6];

// indices to the various widgets:
#define WI_TITLE    0
#define WI_MINUTES  1
#define WI_SECONDS  2
#define WI_BUTTON_A 3
#define WI_BUTTON_B 4
#define WI_BUTTON_C 5
#define WI_NONE     ((uint8_t)~0)

// some widgets are used quite often; pretend they are simple objects:
#define minutesWidget (*((Int16Widget *)AllWidgets[WI_MINUTES]))
#define secondsWidget (*((Int16Widget *)AllWidgets[WI_SECONDS]))

// short-term highlightened widget
uint8_t toFlash = WI_NONE;

// partially dynamic main layout
#define TIME_TOP    (MAIN_TOP + 10)
#define time_colon_left      ((GFX_WIDTH - M5.lcd.textWidth(":", TIMER_FONT) * TIMER_TEXTSIZE) / 2)
#define time_minutes_left    (time_colon_left - minutesWidget.getWidth() - 4)
#define time_seconds_left    (time_colon_left + M5.lcd.textWidth(":", TIMER_FONT) * TIMER_TEXTSIZE + 4)

// settings to be saved to / read from NVS:
Preferences prefs;
typedef struct {
  uint8_t minutes;
  uint8_t seconds;
} settingsType;
settingsType settings = { FALLBACK_INITIAL_MINUTES, FALLBACK_INITIAL_SECONDS };

//*************************************
// re-draw my screen:

void redraw(bool doClear = false) {
    static uint8_t toUnFlash = WI_NONE;
    uint8_t i;
//Serial.printf("redraw(%d)\n", doClear);
    if(doClear) M5.lcd.clearDisplay();

    if(toUnFlash != WI_NONE) {
        AllWidgets[toUnFlash]->changeBackColor(COLOR_BUTTONS);
        toUnFlash = WI_NONE;
    }
    if(toFlash != WI_NONE) {
        AllWidgets[toFlash]->changeBackColor(COLOR_BUTTONS_ACTIVE);
        toUnFlash = toFlash;
        toFlash = WI_NONE;
    }

    M5.lcd.setTextSize(TIMER_TEXTSIZE);
    M5.lcd.setTextDatum(TL_DATUM);
    if(doClear) M5.lcd.drawString(":", time_colon_left,TIME_TOP+2, TIMER_FONT);
    for(i=0; i<sizeof(AllWidgets)/sizeof(Widget *); ++i)
//{ Serial.printf("redraw: widget #%d@%x\n", i, AllWidgets[i]);
        AllWidgets[i]->reDraw(doClear);
//}
}

//*************************************
// get/save settings (quite failsafe)

#define SETTINGS_KEY    "settings"

bool getSettings(void) {
    bool result = false;
    settingsType newSettings;

    prefs.begin(PROJECT_LABEL);
    if(prefs.getBytesLength(SETTINGS_KEY) == sizeof(settingsType))
        if(prefs.getBytes(SETTINGS_KEY, (void *) &newSettings, sizeof(settingsType)) == sizeof(settingsType)) {
            settings = newSettings;
            result = true;
        }
    prefs.end();
    return result;
}

bool saveSettings(void) {
    bool result;

    prefs.begin(PROJECT_LABEL);
    result = prefs.putBytes(SETTINGS_KEY, (void *) &settings, sizeof(settingsType)) == sizeof(settingsType);
    prefs.end();
    return result;
}

//*************************************
// init phase
void setup() {
    // initialize the M5Stack object
    M5.begin(SERIAL_BAUDS);
    delay(200); // wait for serial to "settle", preventing mangling of initial bits
    Serial.println(PROJECT" v"VERSION" starting");

    // "enable" Update from SD
#ifdef __M5STACKUPDATER_H
    if(digitalRead(BUTTON_A_PIN) == 0) {
        Wire.begin();
        Serial.println("switching to SD/TF menu");
        updateFromFS(SD);
        ESP.restart();
    }
#endif
   
    // init' the "internal" LCD
    M5.lcd.setBrightness(M5STACK_BRIGHTNESS);
    M5.lcd.fillScreen(COLOR_BACK);
    M5.lcd.setTextColor(COLOR_TEXT);

    // (try to) restore previous setup
    getSettings();
    
    // create the Widgets - this is not done "statically" because it requires the M5 object to be set up (before)
    AllWidgets[WI_TITLE] = new Widget(0,0, COLOR_TITLE_BG,COLOR_TEXT, LABELS_FONT,LABELS_TEXTSIZE, WALIGN_CENTER, 1, PROJECT);
    AllWidgets[WI_TITLE]->setWidth(GFX_WIDTH);
    AllWidgets[WI_MINUTES] = (Widget *) new Int16Widget(0,TIME_TOP, COLOR_BUTTONS_ACTIVE,COLOR_TEXT, TIMER_FONT,TIMER_TEXTSIZE, WALIGN_CENTER, 2, settings.minutes);
    // time_minutes_left includes a (method) call to minutesWidget!!!
    AllWidgets[WI_MINUTES]->move(time_minutes_left, TIME_TOP);
    static const char minFormat[] = "%02d";
    AllWidgets[WI_SECONDS] = (Widget *) new Int16Widget(time_seconds_left,TIME_TOP, COLOR_BUTTONS,COLOR_TEXT, TIMER_FONT,TIMER_TEXTSIZE, WALIGN_CENTER, 2, settings.seconds, minFormat);
    AllWidgets[WI_BUTTON_A] = new Widget(LBUTTON_X,BUTTONS_Y, COLOR_BUTTONS, COLOR_TEXT, LABELS_FONT,LABELS_TEXTSIZE, WALIGN_CENTER, 6, "-");
    AllWidgets[WI_BUTTON_B] = new Widget(MBUTTON_X,BUTTONS_Y, COLOR_BUTTONS, COLOR_TEXT, LABELS_FONT,LABELS_TEXTSIZE, WALIGN_CENTER, 6, "+");
    AllWidgets[WI_BUTTON_C] = new Widget(RBUTTON_X,BUTTONS_Y, COLOR_BUTTONS, COLOR_TEXT, LABELS_FONT,LABELS_TEXTSIZE, WALIGN_CENTER, 6, ">");

    redraw(true);
}

// whar "area" (page of the menu) are we in? - AE=active area
#define AE_MINUTES  0
#define AE_SECONDS  1
#define AE_MAIN     2
#define FIRST_AE    AE_MINUTES
#define LAST_AE     AE_MAIN

//*************************************
// check if a button was "pressed", but including some repetitions:
// a) button was just pressed
// b) button was held for some period

bool buttonPressed(Button b) {
    return b.wasPressed() ||
           (b.isPressed() && b.pressedFor(BUTTON_REPEAT_AFTER));
}

//*************************************
// increment/decrement an numeric widget with given limit & step

void rangedInc(Int16Widget *w, const int16_t min, const int16_t max, const int16_t step) {
    int16_t newval = w->get() + step;
    if(newval > max) newval -= max + 1;
    w->set(newval);
}
void rangedDec(Int16Widget *w, const int16_t min, const int16_t max, const int16_t step) {
    int16_t newval = w->get() - step;
    if(newval < min) newval += max + 1;
    w->set(newval);
}

//*************************************
// main loop
void loop() {
    unsigned long millisStart = millis();
    static bool running = false, paused = false, finished = false;
        /* meanings:
            running     is the timer currently active?
            paused      was the timer paused? false, if the timer should start with what was "originally" set by the widgets
                            (true implies: not running)
            finished    has the timer finished? false means there is a remainder it can continue with
                            (true implies: not paused & not running)
        */
    static uint8_t activeElement = AE_MINUTES;
    static uint16_t fullTimeS, remainingS;  // fullTimeS: original start value; remainingS: from where it was last started/continued
    static uint32_t startedAt;              // when (ms) the timer was last started/continued; compares to remainingS
    uint16_t widgetsTime = 999;

    M5.update();
    
    if(activeElement == AE_MAIN) {
        widgetsTime = minutesWidget.get() * 60 + secondsWidget.get();
        if(running) {
            int16_t nowRemainingS = remainingS - (millisStart - startedAt) / 1000;
            if(nowRemainingS != widgetsTime) {
                minutesWidget.set(nowRemainingS / 60);
                secondsWidget.set(nowRemainingS % 60);
                widgetsTime = nowRemainingS;
                if(nowRemainingS <= 0) {
                    finished = paused = true;
                    running = false;
                    AllWidgets[WI_BUTTON_B]->set("Reset");
                    M5.Speaker.beep();
                }
            }
        }
        
        if(M5.BtnA.wasPressed()) {
            AllWidgets[WI_BUTTON_A]->changeBackColor(COLOR_BUTTONS_ACTIVE, true);
            M5Stack_powerOFF(BUTTON_A_PIN);
        } else if(M5.BtnB.wasPressed()) {
            if(running) {
                paused = true;
                running = false;
                finished = widgetsTime == 0;
            } else if(finished) {
                // reset the timer to the previous start time
                minutesWidget.set(fullTimeS / 60);
                secondsWidget.set(fullTimeS % 60);
                paused = finished = false;
            } else {
                remainingS = paused ? widgetsTime : fullTimeS;
                if(remainingS) {    // prevent (re-)starting with 0 seconds
                    startedAt = millisStart;
                    paused = false;
                    running = true;
                }
            }
            AllWidgets[WI_BUTTON_B]->set(running ? "Pause" : (paused ? "Cont.":(remainingS ? "Start" : " ")));
            toFlash = WI_BUTTON_B;
        }
    } else {
        if(buttonPressed(M5.BtnA)) {
            if(activeElement == AE_MINUTES)
                 rangedDec((Int16Widget *)AllWidgets[WI_MINUTES], 0,99, 1);
            else rangedDec((Int16Widget *)AllWidgets[WI_SECONDS], 0,59, SECONDS_STEP);
            toFlash = WI_BUTTON_A;
        } else if(buttonPressed(M5.BtnB)) {
            if(activeElement == AE_MINUTES)
                 rangedInc((Int16Widget *)AllWidgets[WI_MINUTES], 0,99, 1);
            else rangedInc((Int16Widget *)AllWidgets[WI_SECONDS], 0,59, SECONDS_STEP);
            toFlash = WI_BUTTON_B;
        }
    }
    
    if(M5.BtnC.wasPressed()) {
        activeElement = (activeElement == LAST_AE) ? FIRST_AE : (activeElement + 1);
        switch(activeElement) {
            case AE_MINUTES:    // switch from "main" to minutes
                                minutesWidget.changeBackColor(COLOR_BUTTONS_ACTIVE);
                                AllWidgets[WI_BUTTON_A]->set("-");
                                AllWidgets[WI_BUTTON_B]->set("+");
                                break;
            case AE_SECONDS:    // switch from minutes to seconds
                                minutesWidget.changeBackColor(COLOR_BUTTONS);
                                secondsWidget.changeBackColor(COLOR_BUTTONS_ACTIVE);
                                break;
            case AE_MAIN:       // switch from seconds to "main"
                                secondsWidget.changeBackColor(COLOR_BUTTONS);
                                AllWidgets[WI_BUTTON_A]->set("Sleep");
                                fullTimeS = minutesWidget.get() * 60 + secondsWidget.get();
                                if(fullTimeS) {
                                    settings.minutes = minutesWidget.get();
                                    settings.seconds = secondsWidget.get();
                                    saveSettings();
                                }
                                AllWidgets[WI_BUTTON_B]->set(fullTimeS ? "Go!" : " ");
                                break;
        }
        toFlash = WI_BUTTON_C;
    }
    
    redraw();

    // wait for the rest of the <configured> period:
    unsigned long elapsed = millis() - millisStart;
    if(elapsed < SLEEP_PHASE)   {
        Serial.print("sleep for "); Serial.print(SLEEP_PHASE - elapsed); Serial.println("ms");
        delay(SLEEP_PHASE - elapsed);
    }
}

// go to deep sleep mode (cloned from M5Stack  libs)
void M5Stack_powerOFF(uint8_t wakeup_button) {

	// power off the Lcd
	M5.lcd.setBrightness(0);
	M5.lcd.sleep();

	// prepare wakeup
	esp_sleep_enable_ext0_wakeup((gpio_num_t)wakeup_button, LOW);

    // ensure wakeup button is not still being pressed (which might cause an immediate wakeup)
	while(digitalRead(wakeup_button) == LOW)
		delay(10);

	// ESP32 into deep sleep
	esp_deep_sleep_start();
}
