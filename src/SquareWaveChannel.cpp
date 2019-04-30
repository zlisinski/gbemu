#include "gbemu.h"
#include "Logger.h"
#include "SquareWaveChannel.h"


const uint8_t WAVEFORM[4][8] = {
    {0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 0}
};


SquareWaveChannel::SquareWaveChannel(const char *name) :
    name(name),
    active(false),
    counter(0),
    frequency(0),
    frequencyCounter(0),
    continuous(false),
    lengthCounter(0),
    dutyCycle(eDutyCycle50),
    dutySequence(0),
    masterVolume(0),
    volume(0),
    envelopeUp(false),
    envelopeLength(0),
    envelopeCounter(0),
    sweepTime(0),
    sweepUp(false),
    sweepShift(0),
    sweepCounter(0)
{

}


SquareWaveChannel::~SquareWaveChannel()
{

}


void SquareWaveChannel::Tick(uint value)
{
    if (active == false)
        return;

    for (uint i = 0; i < value; i += CLOCKS_PER_CYCLE)
    {
        counter += CLOCKS_PER_CYCLE;

        // Every 256Hz check the length counter and deactivate channel if it reaches zero.
        if ((counter & 0x3FFF) == 0 && continuous == false && lengthCounter != 0)
        {
            //LogDebug("%08X, counter=%u", counter, ch1LengthCounter);
            lengthCounter--;
            if (lengthCounter == 0)
            {
                // This is supposed to also clear NR52 Channel X On bit.
                LogDebug("Channel %s disabled", name);
                active = false;
            }
        }

        // Every 64Hz perform envelope amplification/attenuation.
        if ((counter & 0xFFFF) == 0 && envelopeLength != 0)
        {
            envelopeCounter--;
            if (envelopeCounter == 0)
            {
                if (envelopeUp && volume < 15)
                {
                    volume++;
                    if (volume >= 15)
                        envelopeLength = 0;
                }
                else if (!envelopeUp && volume > 0)
                {
                    volume--;
                    if (volume == 0)
                        envelopeLength = 0;
                }

                ReloadEnvelopeCounter();
            }
        }

        // Every 128Hz shift the frequency if sweep is enabled.
        if ((counter & 0x7FFF) == 0 && sweepTime != 0 && sweepShift != 0)
        {
            sweepCounter--;
            if (sweepCounter == 0)
            {
                if (sweepUp)
                {
                    uint newFreq = frequency + (frequency >> sweepShift);
                    if (newFreq > 0x7FF)
                    {
                        // This is supposed to also clear NR52 Channel 1 On bit.
                        active = false;
                        frequency = 0x7FF;
                    }
                    else
                        frequency = newFreq;
                }
                else
                {
                    int newFreq = frequency - (frequency >> sweepShift);
                    if (newFreq >= 0)
                        frequency = newFreq;
                }

                ReloadSweepCounter();
            }
        }
        
        // Every frequency period, advance the duty sequence.
        frequencyCounter--;
        if (frequencyCounter == 0)
        {
            dutySequence = (dutySequence + 1) & 0x07;

            ReloadFrequencyCounter();
        }
    }
}


uint8_t SquareWaveChannel::GetSample()
{
    if (!active)
        return 0;

    return WAVEFORM[dutyCycle][dutySequence] * volume;
}


void SquareWaveChannel::SetInitialize(bool init)
{
    if (init)
    {
        dutySequence = 0;
        ReloadFrequencyCounter();
        ReloadLengthCounter();
        ReloadEnvelopeCounter();
        ReloadSweepCounter();
        volume = masterVolume;

        // Channel is disabled if the volume is 0, and the envelope is not set to amplify.
        if (volume == 0 && envelopeUp == false)
        {
            LogDebug("Init of channel %s, but volume is 0", name);
            return;
        }

        if (active == true)
            LogDebug("Channel %s enabled while already enabled %uhz counter=%u", name, GetFrequency(), lengthCounter);
        else
            LogDebug("Channel %s enabled %uhz", name, GetFrequency());

        active = true;
    }
    else
    {
        active = false;
    }
}


void SquareWaveChannel::SetContinuous(bool cont)
{
    continuous = cont;
    //LogDebug("Channel %s continuous=%d", name, cont);
}


void SquareWaveChannel::SetFrequency(uint16_t freq)
{
    frequency = freq;
    ReloadFrequencyCounter();
}


void SquareWaveChannel::SetDutyCycle(DutyCycle duty)
{
    dutyCycle = duty;
}


void SquareWaveChannel::SetLengthCounter(uint8_t length)
{
    soundLength = length;
    lengthCounter = length;
    //LogDebug("Channel %s length=%u", name, lengthCounter);
}


void SquareWaveChannel::SetVolume(uint8_t vol)
{
    masterVolume = vol;
    volume = vol;
}


void SquareWaveChannel::SetEnvelopeUp(bool up)
{
    envelopeUp = up;
}


void SquareWaveChannel::SetEnvelopeLength(uint8_t length)
{
    envelopeLength = length;
    ReloadEnvelopeCounter();
}


void SquareWaveChannel::SetSweepTime(uint8_t time)
{
    sweepTime = time;
    ReloadSweepCounter();
}


void SquareWaveChannel::SetSweepUp(bool up)
{
    sweepUp = up;
}


void SquareWaveChannel::SetSweepShift(uint8_t shift)
{
    sweepShift = shift;
}


void SquareWaveChannel::ReloadFrequencyCounter()
{
    frequencyCounter = 2048 - frequency;
}


void SquareWaveChannel::ReloadLengthCounter()
{
    lengthCounter = soundLength;
}


void SquareWaveChannel::ReloadEnvelopeCounter()
{
    envelopeCounter = envelopeLength;
}


void SquareWaveChannel::ReloadSweepCounter()
{
    sweepCounter = sweepTime;
}


uint32_t SquareWaveChannel::GetFrequency() const
{
    return CLOCKS_PER_SECOND / (8 * (2048 - frequency));
}