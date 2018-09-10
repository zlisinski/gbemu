#pragma once

#include <list>
#include <memory>
#include <unordered_map>

#include "gbemu.h"
#include "MemoryByteObserver.h"

class MemoryByteSubject
{
public:
    virtual void AttachObserver(uint16_t addr, std::weak_ptr<MemoryByteObserver> observer)
    {
        observers.insert({addr, observer});
    }

    virtual void DetachObserver(uint16_t addr, MemoryByteObserver *observer)
    {
        typedef std::unordered_multimap<uint16_t, std::weak_ptr<MemoryByteObserver>>::iterator iter;
        std::pair<iter, iter> range = observers.equal_range(addr);
        for (iter it = range.first; it != range.second; ++it)
        {
            if (it->second.lock().get() == observer)
            {
                observers.erase(it);
                break;
            }
        }
    }

    virtual bool NotifyObservers(uint16_t addr, uint8_t value)
    {
        bool notified = false;

        typedef std::unordered_multimap<uint16_t, std::weak_ptr<MemoryByteObserver>>::iterator iter;
        std::pair<iter, iter> range = observers.equal_range(addr);
        for (iter it = range.first; it != range.second; ++it)
        {
            std::shared_ptr<MemoryByteObserver> observer = it->second.lock();
            if (observer)
            {
                observer->UpdateMemoryAddr(addr, value);
                notified = true;
            }
        }

        return notified;
    }

protected:
    std::unordered_multimap<uint16_t, std::weak_ptr<MemoryByteObserver>> observers;
};