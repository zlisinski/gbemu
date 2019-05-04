#include "gbemu.h"
#include "Logger.h"
#include "WaveformChannel.h"

WaveformChannel::WaveformChannel() :
    active(false),
    counter(0),
    frequency(0),
    frequencyCounter(0),
    continuous(false),
    enabled(false),
    soundLength(0),
    lengthCounter(0),
    volume(0),
    sampleIndex(0),
    waveTable(NULL)
{

}


WaveformChannel::~WaveformChannel()
{

}


void WaveformChannel::Tick(uint value)
{
    if (active == false)
        return;

    for (uint i = 0; i < value; i += CLOCKS_PER_CYCLE)
    {
        counter += CLOCKS_PER_CYCLE;

        // Every 256Hz check the length counter and deactivate channel if it reaches zero.
        if ((counter & 0x3FFF) == 0 && continuous == false && lengthCounter != 0)
        {
            lengthCounter--;
            if (lengthCounter == 0)
            {
                // This is supposed to also clear NR52 Channel 3 On bit.
                LogDebug("Channel 3 disabled");
                active = false;
            }
        }

        // Every frequency period, advance the duty sequence.
        frequencyCounter--;
        if (frequencyCounter == 0)
        {
            sampleIndex = (sampleIndex + 1) & 0x1F;

            ReloadFrequencyCounter();
        }
    }
}


uint8_t WaveformChannel::GetSample()
{
    if (!active || !enabled)
        return 0;

    // Select the byte in the wave table using index/2.
    uint8_t byte = waveTable[sampleIndex >> 1];

    // Select nybble based on lowest bit of index.
    byte = (byte >> ((~sampleIndex & 0x01) * 4)) & 0x0F;

    // Decrease value by volume, possibly muting.
    byte >>= volume;

    return byte;
}


void WaveformChannel::SetInitialize(bool init)
{
    if (init)
    {
        sampleIndex = 0;
        ReloadFrequencyCounter();
        ReloadLengthCounter();

        if (active == true)
            LogDebug("Channel 3 enabled while already enabled %uHz counter=%u", GetFrequency(), lengthCounter);
        else
            LogDebug("Channel 3 enabled %uHz", GetFrequency());

        active = true;
    }
    else
    {
        active = false;
    }
}


void WaveformChannel::SetContinuous(bool cont)
{
    continuous = cont;
    //LogDebug("Channel 3 continuous=%d", cont);
}


void WaveformChannel::SetFrequency(uint16_t freq)
{
    frequency = freq;
    ReloadFrequencyCounter();
}


void WaveformChannel::SetEnabled(bool enable)
{
    enabled = enable;
    //LogDebug("Channel 3 enabled=%d", enable);
}


void WaveformChannel::SetSoundLength(uint8_t length)
{
    soundLength = 256 - length;
    lengthCounter = soundLength;
    //LogDebug("Channel 3 length=%u", lengthCounter);
}


void WaveformChannel::SetVolume(uint8_t vol)
{
    // volume is used to right-shift the value, so mute should shift all 4 bits out.
    if (vol == 0)
        volume = 4;
    else
        volume = vol - 1;
}


void WaveformChannel::ReloadFrequencyCounter()
{
    frequencyCounter = (2048 - frequency) / 2;
}


void WaveformChannel::ReloadLengthCounter()
{
    lengthCounter = soundLength;
}


uint32_t WaveformChannel::GetFrequency() const
{
    return CLOCKS_PER_SECOND / (8 * (2048 - frequency));
}