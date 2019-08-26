/*

 config data

 */

#ifndef CONFIG_H

#define PROJECT_LABEL   "timer" // for filenames etc.

// baudrate for serial output
#define SERIAL_BAUDS    115200

// how long (x ms) should "ticks" take?
#define SLEEP_PHASE 100

// what time to start with if no previous setting is found in NVS?
#define FALLBACK_INITIAL_MINUTES    5
#define FALLBACK_INITIAL_SECONDS    0

// one step in seconds is: <n> seconds. (for me, its is more desirable to adjust the seconds quickly than to be  able to choose *any* starting value)
#define SECONDS_STEP    5

//*************************************
// display settings

// brightness
#define M5STACK_BRIGHTNESS  50

// colors to be used
// general background
#define COLOR_BACK  BLACK
// background of title bar & buttons (identical in the lib)
#define COLOR_BUTTONS NAVY
// background an active control
#define COLOR_BUTTONS_ACTIVE OLIVE
// (general) text colors
#define COLOR_TEXT  WHITE
// colors for list headers
#define COLOR_TITLE_BG      DARKGREY
#define COLOR_TITLE_TEXT    COLOR_TEXT

// fonts/font sizes: the timer display itself ...
#define TIMER_FONT      8
#define TIMER_TEXTSIZE  1
// ... and the buttons/headline:
#define LABELS_FONT     2
#define LABELS_TEXTSIZE 1

//*************************************
// some gfx dimensions
#define GFX_WIDTH  M5.lcd.width()
#define GFX_HEIGHT M5.lcd.height()
#define GFX_MAXX (GFX_WIDTH-1)
#define GFX_MAXY (GFX_HEIGHT-1)

#define BUTTONS_Y   (GFX_HEIGHT-28)
#define LBUTTON_X   40
#define MBUTTON_X   134
#define RBUTTON_X   228

// main area (caption & buttons row excluded)
#define MAIN_LEFT   0
#define MAIN_TOP    32
#define MAIN_WIDTH  GFX_WIDTH
#define MAIN_HEIGHT GFX_HEIGHT-64
#define MAIN_RIGHT  (MAIN_LEFT + MAIN_WIDTH - 1)
#define MAIN_BOTTOM (MAIN_TOP + MAIN_HEIGHT - 1)

// from when on (ms) handle a held button like repeated pressing it?
#define BUTTON_REPEAT_AFTER 400

#define CONFIG_H
#endif CONFIG_H
