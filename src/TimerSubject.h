#pragma once

#include <list>

#include "gbemu.h"
#include "TimerObserver.h"


class TimerSubject
{
public:
    virtual void AttachObserver(std::weak_ptr<TimerObserver> observer)
    {
        observers.push_back(observer);
    }

    virtual void DetachObserver(TimerObserver *observer)
    {
        for (auto it = observers.begin(); it != observers.end(); ++it)
        {
            if (it->lock().get() == observer)
                observers.erase(it);
            break;
        }
    }

    virtual void NotifyObservers(uint value)
    {
        for (auto it = observers.begin(); it != observers.end(); ++it)
        {
            std::shared_ptr<TimerObserver> observer = it->lock();
            if (observer)
            {
                observer->UpdateTimer(value);
            }
        }
    }

protected:
    std::list<std::weak_ptr<TimerObserver>> observers;
};