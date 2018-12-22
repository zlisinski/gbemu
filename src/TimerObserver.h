#pragma once

#include "gbemu.h"

class TimerSubject;

class TimerObserver
{
public:
    TimerObserver() : timerSubject(NULL) {}

    virtual void AttachToTimerSubject(TimerSubject* subject) = 0;
    
    virtual void UpdateTimer(uint value) = 0;

protected:
    TimerSubject* timerSubject;
};