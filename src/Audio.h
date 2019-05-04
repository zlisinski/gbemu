#pragma once

#include "gbemu.h"
#include "AudioInterface.h"
#include "Memory.h"
#include "IoRegisterProxy.h"
#include "SquareWaveChannel.h"
#include "TimerObserver.h"
#include "WaveformChannel.h"


class Audio : public IoRegisterProxy, public TimerObserver
{
public:
    Audio(IoRegisterSubject *ioRegisterSubject, TimerSubject *timerSubject, AudioInterface *audioInterface);
    virtual ~Audio();

    // Inherited from IoRegisterProxy.
    virtual bool WriteByte(uint16_t address, uint8_t byte);
    virtual uint8_t ReadByte(uint16_t address) const;

    // Inherited from TimerObserver.
    virtual void UpdateTimer(uint value);

    void SetMasterVolume(uint32_t volume) {masterVolume = volume;}

private:
    AudioInterface *audioInterface;

    SquareWaveChannel channel1;
    SquareWaveChannel channel2;
    WaveformChannel channel3;

    uint32_t sampleCounter;

    std::array<uint8_t, AudioInterface::BUFFER_LEN> soundBuffer;
    uint16_t bufferSize;

    uint32_t clocksPerSample;

    uint32_t masterVolume;

    uint8_t *regNR10; // Sound mode 1, sweep
    uint8_t *regNR11; // Sound mode 1, length/wave pattern duty
    uint8_t *regNR12; // Sound mode 1, envelope
    uint8_t *regNR13; // Sound mode 1, frequency low
    uint8_t *regNR14; // Sound mode 1, frequency high
    uint8_t *regNR21; // Sound mode 2, length/wave pattern duty
    uint8_t *regNR22; // Sound mode 2, envelope
    uint8_t *regNR23; // Sound mode 2, frequency low
    uint8_t *regNR24; // Sound mode 2, frequency high
    uint8_t *regNR30; // Sound mode 3, on/off
    uint8_t *regNR31; // Sound mode 3, sound length
    uint8_t *regNR32; // Sound mode 3, select output level
    uint8_t *regNR33; // Sound mode 3, frequency low
    uint8_t *regNR34; // Sound mode 3, frequency high
    uint8_t *regNR41; // Sound mode 4, sound length
    uint8_t *regNR42; // Sound mode 4, envelope
    uint8_t *regNR43; // Sound mode 4, polynomial counter
    uint8_t *regNR44; // Sound mode 4, counter/consecutive
    uint8_t *regNR50; // Channel control, on/off, volume
    uint8_t *regNR51; // Sound output terminal
    uint8_t *regNR52; // Sound on/off
    uint8_t *regWave0; // Wave pattern RAM
    uint8_t *regWave1; // Wave pattern RAM
    uint8_t *regWave2; // Wave pattern RAM
    uint8_t *regWave3; // Wave pattern RAM
    uint8_t *regWave4; // Wave pattern RAM
    uint8_t *regWave5; // Wave pattern RAM
    uint8_t *regWave6; // Wave pattern RAM
    uint8_t *regWave7; // Wave pattern RAM
    uint8_t *regWave8; // Wave pattern RAM
    uint8_t *regWave9; // Wave pattern RAM
    uint8_t *regWaveA; // Wave pattern RAM
    uint8_t *regWaveB; // Wave pattern RAM
    uint8_t *regWaveC; // Wave pattern RAM
    uint8_t *regWaveD; // Wave pattern RAM
    uint8_t *regWaveE; // Wave pattern RAM
    uint8_t *regWaveF; // Wave pattern RAM
};