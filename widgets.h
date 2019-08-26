/*

 "Widgets": simple GUI objects for displaying something like a button

 (c) Arnold Schommer 2019

 v0.0: start of development
 v0.5: first partially useful version
 v0.8: inherit "Int16Widget" from "Widget" - instead of including a Widget-instance into Int16Widget
 v0.9.2: make reDraw() virtual to unify redrawing of all widgets
 v1.0: adding methods to move the widgets and query the current position
 
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


#ifndef WIDGET_H
#define WIDGET_H

#include <M5Stack.h>    // https://github.com/m5stack/M5Stack
#include "config.h"     // after M5Stack.h because it references colors defined there

#define NUMWIDGETS_MAX_LENGTH 8
#define WCORNER_RADIUS 3

#define WALIGN_LEFT     TL_DATUM
#define WALIGN_CENTER   TC_DATUM
#define WALIGN_RIGHT    TR_DATUM

class Widget {
    public:
        inline Widget(void) { } // required to define global/static vars of this class
        Widget(const uint16_t cx, const uint16_t cy,                    // top/left corner
               const uint16_t cBackColor, const uint16_t cTextColor,    // color of the "Box" / text
               const uint8_t cFont, const uint8_t cTextSize = 1,        // font, size of font
               const uint8_t cAlignment = WALIGN_CENTER,                // alignment (WALIGN_*)
               const uint8_t cMaxChars = 0, const char *cLabel=NULL);   // max. chars (concerning "width"), initial text to display
        
        inline char *get(void)                              { return label; }
        inline void set(const char *newLabel)               { label = (char *)newLabel; reDraw(); }

        inline void changeBackColor(const uint16_t newColor, const bool immediateRedraw=false) { backColor = newColor; if(immediateRedraw) reDraw(); }
        inline void changeTextColor(const uint16_t newColor, const bool immediateRedraw=false) { textColor = newColor; if(immediateRedraw) reDraw(); }
        
        inline void setWidth(const uint16_t newWidth)     { w=newWidth; }
        inline void setHeight(const uint16_t newHeight)   { h=newHeight; }
        inline uint16_t getWidth(void)  { return w; }
        inline uint16_t getHeight(void) { return h; }
        inline void move(const uint16_t newX, const uint16_t newY)  { x0=newX; y0=newY; }
        inline uint16_t getX(void) { return x0; }
        inline uint16_t getY(void) { return y0; }

        virtual void reDraw(const bool force = false);
        inline void draw(void) { reDraw(true); }

    protected:
        int16_t textColor, drawnTextColor, backColor, drawnBackColor;

        void coreDraw(void);                    // helper method for reDraw(), relying on label, not checking for changes
                                                // mainly to be called for different datatype-"versions" of the class

//    private:
        void calcHW(const uint8_t maxChars);    // helper method to calculate/set width & height based on font & maxChars
        char *label, *drawnLabel;
        uint16_t x0, y0, w, h, font, textSize, textDatum;
};

class Int16Widget : public Widget {
    public:
        inline Int16Widget(void) { } // required to define global/static vars of this class
        Int16Widget(const uint16_t cx, const uint16_t cy,               // top/left corner
               const uint16_t cBackColor, const uint16_t cTextColor,    // color of the "Box" / text
               const uint8_t cFont, const uint8_t cTextSize = 1,        // font, size of font
               const uint8_t cAlignment = WALIGN_CENTER,                // alignment (WALIGN_*)
               const uint8_t cMaxChars = 0, const int16_t cValue = 0,   // max. chars (concerning "width"), initial number to display
               const char *cFormat = NULL);                             // format of the number
        
        inline int16_t get(void)                            { return value; }
        inline void set(const int16_t newValue)             { value = newValue; reDraw(); }

        virtual void reDraw(const bool force = false);

    protected:
        int16_t value, drawnValue;
        char *format, textValue[NUMWIDGETS_MAX_LENGTH];
    
        inline void doFormat(void) { sprintf(textValue, format, value); }
};

#endif  // WIDGET_H
