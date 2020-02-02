#pragma once

#include "gbemu.h"
#include "AbsByteProxy.h"

class RegisterByteProxy : public AbsByteProxy
{
public:
    RegisterByteProxy(uint8_t *value) :
        AbsByteProxy(false),
        bytePtr(value)
    {}

    RegisterByteProxy(const RegisterByteProxy &copy) :
        AbsByteProxy(copy),
        bytePtr(copy.bytePtr)
    {}

    virtual ~RegisterByteProxy() {}

    virtual uint8_t operator=(uint8_t value)
    {
        *bytePtr = value;
        return *bytePtr;
    }

    virtual operator uint8_t() const
    {
        return *bytePtr;
    }

    virtual uint8_t Value() const
    {
        return *bytePtr;
    }

private:
    uint8_t *bytePtr;
};