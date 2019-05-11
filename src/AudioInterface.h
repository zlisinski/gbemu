#pragma once
#include "gbemu.h"

#include <array>

class AudioInterface
{
public:
    struct Channels
    {
        bool channel1;
        bool channel2;
        bool channel3;
        bool channel4;
    };

    static const int BUFFER_LEN = 4096;

    virtual void AudioDataReady(const std::array<int16_t, BUFFER_LEN> &data) = 0;
    virtual int GetAudioSampleRate() = 0;
    virtual bool GetAudioEnabled() = 0;
    virtual Channels GetEnabledAudioChannels() = 0;
    virtual uint8_t GetAudioVolume() = 0;

protected:
    ~AudioInterface() {}
};