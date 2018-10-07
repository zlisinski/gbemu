#pragma once

#include "gbemu.h"

class TimerSubject;

class TimerObserver
{
public:
    TimerObserver() : subject(NULL) {}

    virtual void AttachToSubject(std::shared_ptr<TimerSubject>) = 0;
    
    virtual void UpdateTimer(uint value) = 0;

protected:
    std::shared_ptr<TimerSubject> subject;
};