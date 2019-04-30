#pragma once

#include "gbemu.h"

enum DutyCycle
{
    eDutyCycle12,
    eDutyCycle25,
    eDutyCycle50,
    eDutyCycle75
};

class SquareWaveChannel
{
public:
    SquareWaveChannel(const char *name);
    ~SquareWaveChannel();

    void Tick(uint value);
    uint8_t GetSample();

    void SetInitialize(bool init);
    void SetContinuous(bool cont);
    void SetFrequency(uint16_t freq);
    
    void SetDutyCycle(DutyCycle duty);
    void SetLengthCounter(uint8_t length);
    
    void SetVolume(uint8_t vol);
    void SetEnvelopeUp(bool up);
    void SetEnvelopeLength(uint8_t length);

    void SetSweepTime(uint8_t time);
    void SetSweepUp(bool up);
    void SetSweepShift(uint8_t shift);

private:
    void ReloadFrequencyCounter();
    void ReloadLengthCounter();
    void ReloadEnvelopeCounter();
    void ReloadSweepCounter();
    uint32_t GetFrequency() const;

    const char *name;

    bool active;
    uint32_t counter;

    uint16_t frequency;
    uint16_t frequencyCounter;
    bool continuous;
    
    uint8_t soundLength;
    uint8_t lengthCounter;
    DutyCycle dutyCycle;
    uint8_t dutySequence;

    uint8_t masterVolume;
    uint8_t volume;
    bool envelopeUp;
    uint8_t envelopeLength;
    uint8_t envelopeCounter;

    uint8_t sweepTime;
    bool sweepUp;
    uint8_t sweepShift;
    uint8_t sweepCounter;
};