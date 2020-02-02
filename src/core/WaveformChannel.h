#pragma once

#include "gbemu.h"


class WaveformChannel
{
public:
    WaveformChannel();
    ~WaveformChannel();

    void Tick(uint value);
    int8_t GetSample();

    void SetInitialize(bool init);
    void SetContinuous(bool cont);
    void SetFrequency(uint16_t freq);

    void SetEnabled(bool enable);
    void SetSoundLength(uint8_t length);
    void SetVolume(uint8_t vol);

    void SetWaveTable(uint8_t *table) {waveTable = table;}

private:
    void ReloadFrequencyCounter();
    void ReloadLengthCounter();
    uint32_t GetFrequency() const;

    bool active;
    uint32_t counter;

    uint16_t frequency;
    uint16_t frequencyCounter;
    bool continuous;

    bool enabled;
    uint8_t soundLength;
    uint8_t lengthCounter;
    uint8_t volume;

    uint8_t sampleIndex;
    uint8_t *waveTable;
};