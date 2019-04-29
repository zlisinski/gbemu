#pragma once
#include "gbemu.h"

#include <array>

class AudioInterface
{
public:
    static const int BUFFER_LEN = 4096;

    virtual void AudioDataReady(const std::array<uint8_t, BUFFER_LEN> &data) = 0;
    virtual int GetAudioSampleRate() = 0;

protected:
    ~AudioInterface() {}
};