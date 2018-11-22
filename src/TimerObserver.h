#pragma once

#include "gbemu.h"

class TimerSubject;

class TimerObserver
{
public:
    TimerObserver() : timerSubject(NULL) {}

    virtual void AttachToTimerSubject(std::shared_ptr<TimerSubject>) = 0;
    
    virtual void UpdateTimer(uint value) = 0;

protected:
    std::shared_ptr<TimerSubject> timerSubject;
};