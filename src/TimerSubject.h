#pragma once

#include <list>

#include "gbemu.h"
#include "TimerObserver.h"

// std::vector was a significant slowdown. Using a static array of pointers speeds this up significantly,
// since NotifyObservers() is called at least once per opcode.
class TimerSubject
{
public:
    TimerSubject() : timerObserverCount(0)
    {
        for (int i = 0; i < timerObserversMax; i++)
            timerObservers[i] = NULL;
    }

    virtual void AttachObserver(TimerObserver* observer)
    {
        timerObservers[timerObserverCount++] = observer;
    }

    virtual void DetachObserver(TimerObserver *observer)
    {
        // Don't worry about this for now, since the only time this is called is when
        // the application exits anyway.
        (void)observer; // Stop warnings about unused variables.
    }

    virtual void NotifyObservers(uint value)
    {
        for (int i = 0; i < timerObserverCount; i++)
        {
            if (timerObservers[i])
            {
                timerObservers[i]->UpdateTimer(value);
            }
        }
    }

protected:
    static const int timerObserversMax = 3;
    int timerObserverCount;
    TimerObserver* timerObservers[timerObserversMax];
};