/*

 "Widgets": simple GUI objects for displaying something like a button

 (c) Arnold Schommer 2019

 v0.0: start of development
 v0.5: first partially useful version
 v0.8: inherit "Int16Widget" from "Widget" - instead of including a Widget-instance into Int16Widget
 v0.9.2: make reDraw() virtual to unify redrawing of all widgets
 
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


#include "widgets.h"
#include <string.h>

Widget::Widget(const uint16_t cx, const uint16_t cy,                                    // top/left corner
               const uint16_t cBackColor, const uint16_t cTextColor,                    // color of the "Box" / text
               const uint8_t cFont, const uint8_t cTextSize, const uint8_t cAlignment,  // font, size of font, alignment (WALIGN_*)
               const uint8_t cMaxChars, const char *cLabel) {                           // max. chars (concerning "width"), iniitial text to display
    x0 = cx;
    y0 = cy;
    backColor = cBackColor;
    textColor = cTextColor;
    font = cFont;
    textSize = cTextSize;
    textDatum = cAlignment;
    label = (char *) cLabel;
    drawnLabel = "\001";      // force redraw (first == next) time

    calcHW(cMaxChars);
}

// helper method to calculate/set width & height based on font & maxChars (or, if left zero, label)
void Widget::calcHW(const uint8_t maxChars) {
    h = M5.lcd.fontHeight(font) * textSize + 8;
    if(maxChars > 0)            w = maxChars;
    else if(label && *label)    w = strlen(label);
    else                        w = 3;
    w = w * M5.lcd.textWidth("8", font) * textSize + 8;
}

// strcmp also working if either string is NULL:
int xstrcmp(const char *s1, const char *s2) {
    if((s1 == NULL) || !*s1) {
        if((s2 == NULL) || !*s2)    return 0;
        else                        return -1;
    } else if((s2 == NULL) || !*s2) return 1;
    else                            return strcmp(s1, s2);
}

// helper method for reDraw(), relying on label, not checking for changes
// mainly to be called for different datatype-"versions" of the class
void Widget::coreDraw(void) {
//static int serial=0;
//Serial.printf("Widget@%x.coreDraw#%d(x0/y0=%d/%d, text='%s'@%x); w/h=%d/%d\n", this, serial++, x0,y0, label,label, w,h);
    uint16_t cx = x0+w/2;

    M5.lcd.fillRoundRect(x0, y0, w, h, WCORNER_RADIUS, backColor);
    M5.lcd.setTextColor(textColor);
    M5.lcd.setTextSize(textSize);
    M5.lcd.setTextDatum(textDatum);
    M5.lcd.drawString(label, cx,y0+4, font);
    drawnTextColor = textColor;
    drawnBackColor = backColor;
    drawnLabel = label;
}

void Widget::reDraw(const bool force) {
//static int serial=0;
//Serial.printf("Widget@%x.reDraw#%d(%d)\n", this, serial++, force);
//Serial.printf("\t[drawn]label=[%s@%x] %s@%x\n", 
//            drawnLabel, drawnLabel, label, label);
//Serial.printf("\t[drawn]textColor=[%d] %d, [drawn]backColor=[%d] %d;\t[drawn]label=[%s] %s\n", 
//            drawnTextColor, textColor, drawnBackColor, backColor, drawnLabel, label);
    
    if(!force &&
       (textColor == drawnTextColor) && 
       (backColor == drawnBackColor) && 
       ((xstrcmp(label, drawnLabel) == 0)) )
        return;
//Serial.println("\tyes, (re-)draw it!");

    coreDraw();
}

Int16Widget::Int16Widget(const uint16_t cx, const uint16_t cy,                  // top/left corner
                         const uint16_t cBackColor, const uint16_t cTextColor,  // color of the "Box" / text
                         const uint8_t cFont, const uint8_t cTextSize,          // font, size of font
                         const uint8_t cAlignment,                              // alignment (WALIGN_*)
                         const uint8_t cMaxChars, const int16_t cValue,         // max. chars (concerning "width"), initial number to display
                         const char *cFormat) :                                 // format of the number
                         Widget(cx,cy, cBackColor,cTextColor, cFont,cTextSize, cAlignment, cMaxChars) {
    static const char defaultFormat[] = "%d";

    value     = cValue;
    drawnValue = ~value;    // force redraw (first == next) time
    format    = (char *) (cFormat ? cFormat : defaultFormat);
    Widget::set(textValue);
}

void Int16Widget::reDraw(const bool force) {
//static int serial=0;
//Serial.printf("Int16Widget@%x.reDraw#%d(%d) @ x=%d|y=%d\n", this, serial++, force, x0,y0);
//Serial.printf("\t[drawn]value=[%d] %d\n", drawnValue, value);
//Serial.printf("\t[drawn]textColor=[%u] %u, [drawn]backColor=[%u] %u;\t[drawn]value=[%d] %d\n", 
//            drawnTextColor, textColor, drawnBackColor, backColor, drawnValue, value);
    
    if(!force &&
       (textColor == drawnTextColor) && 
       (backColor == drawnBackColor) && 
       (value     == drawnValue) )
        return;
//Serial.println("\tyes, (re-)draw it!");

    doFormat();
//Serial.printf("\t%d => '%s'@%x\n", value, textValue,textValue);
    coreDraw();

    drawnValue = value;
}
