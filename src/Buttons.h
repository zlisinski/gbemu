#pragma once

#include "gbemu.h"

class Buttons
{
public:
    enum Button
    {
        eButtonUp = 0x01,
        eButtonRight = 0x02,
        eButtonDown = 0x04,
        eButtonLeft = 0x08,
        eButtonSelect = 0x10,
        eButtonStart = 0x20,
        eButtonB = 0x40,
        eButtonA = 0x80
    };

    Buttons() : data(0) {}

    bool IsUpPressed() {return data & Button::eButtonUp;}
    bool IsRightPressed() {return data & eButtonRight;}
    bool IsDownPressed() {return data & eButtonDown;}
    bool IsLeftPressed() {return data & eButtonLeft;}
    bool IsSelectPressed() {return data & eButtonSelect;}
    bool IsStartPressed() {return data & eButtonStart;}
    bool IsBPressed() {return data & eButtonB;}
    bool IsAPressed() {return data & eButtonA;}

    uint8_t data;
};