#pragma once

#include "gbemu.h"

class Globals
{
public:
    static Globals& getInstance()
    {
        static Globals instance;
        return instance;
    }

    bool showFps;
    bool capFps;

    // Don't allow copy and assignment.
    Globals(const Globals&) = delete;
    void operator=(const Globals&) = delete;

private:
    Globals() :
        showFps(true),
        capFps(false)
    {}
};