#pragma once

#include "gbemu.h"


class NoiseChannel
{
public:
    NoiseChannel();
    ~NoiseChannel();

    void Tick(uint value);
    int8_t GetSample();

    void SetInitialize(bool init);
    void SetContinuous(bool cont);

    void SetSoundLength(uint8_t length);

    void SetVolume(uint8_t vol);
    void SetEnvelopeUp(bool up);
    void SetEnvelopeLength(uint8_t length);

    void SetDivRatio(uint8_t div);
    void SetByteWidth(bool byte);
    void SetShiftFrequency(uint8_t freq);

private:
    void ReloadFrequencyCounter();
    void ReloadLengthCounter();
    void ReloadEnvelopeCounter();
    void ReloadSweepCounter();

    bool active;
    uint32_t counter;

    uint32_t frequencyCounter;
    bool continuous;

    uint8_t soundLength;
    uint8_t lengthCounter;

    uint8_t masterVolume;
    uint8_t volume;
    bool envelopeUp;
    uint8_t envelopeLength;
    uint8_t envelopeCounter;

    uint8_t divRatio;
    bool byteWidth;
    uint8_t shiftFrequency;

    uint16_t shiftRegister;
};