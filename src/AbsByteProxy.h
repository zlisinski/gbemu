#pragma once

#include "gbemu.h"

class AbsByteProxy
{
public:
    AbsByteProxy(bool extraCycles) :
        extraCycles(extraCycles)
    {}

    AbsByteProxy(const AbsByteProxy& copy) :
        extraCycles(copy.extraCycles)
    {}

    virtual ~AbsByteProxy() {}

    virtual uint8_t operator=(uint8_t value) = 0;

    virtual operator uint8_t() const = 0;
    virtual uint8_t Value() const = 0;

    bool ExtraCycles() const {return extraCycles;}

protected:
    bool extraCycles;
};