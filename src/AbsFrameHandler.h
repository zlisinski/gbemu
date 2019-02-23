#pragma once
#include "gbemu.h"

class AbsFrameHandler
{
public:
    AbsFrameHandler() {}
    virtual ~AbsFrameHandler() {}

    virtual void DrawFrame(uint32_t *frameBuffer) = 0;
    virtual void MessageBox(const std::string &message) = 0;
};