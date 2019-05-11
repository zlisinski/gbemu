#include <iomanip>
#include <sstream>

#include "gbemu.h"
#include "Logger.h"
#include "NoiseChannel.h"


NoiseChannel::NoiseChannel() :
    active(false),
    counter(0),
    frequencyCounter(0),
    continuous(false),
    lengthCounter(0),
    masterVolume(0),
    volume(0),
    envelopeUp(false),
    envelopeLength(0),
    envelopeCounter(0),
    divRatio(0),
    byteWidth(false),
    shiftFrequency(0),
    shiftRegister(0)
{

}


NoiseChannel::~NoiseChannel()
{

}


void NoiseChannel::Tick(uint value)
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
                LogDebug("Channel 4 disabled");
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

        // Every frequency period, shift and XOR bits.
        frequencyCounter--;
        if (frequencyCounter == 0)
        {
            // XOR the lowest two bits.
            uint8_t newBit = (shiftRegister & 0x0001) ^ ((shiftRegister >> 1) & 0x0001);

            // Shift right, and set bit 14 to the result of previous XOR.
            shiftRegister = (shiftRegister >> 1) | (newBit << 14);

            // If using 7-bit shift register, also set bit 6.
            if (byteWidth)
                shiftRegister |= newBit << 6;

            ReloadFrequencyCounter();
        }
    }
}


int8_t NoiseChannel::GetSample()
{
    if (!active)
        return 0;

    // Low bit of shift register is the output value. Convert [0,1] to [-1,1].
    int8_t value = ((~shiftRegister & 0x0001) * 2) - 1;

    return value * volume;
}


void NoiseChannel::SetInitialize(bool init)
{
    if (init)
    {
        ReloadFrequencyCounter();
        ReloadLengthCounter();
        ReloadEnvelopeCounter();
        volume = masterVolume;
        shiftRegister = 0x7FFF;

        // Channel is disabled if the volume is 0, and the envelope is not set to amplify.
        if (volume == 0 && envelopeUp == false)
        {
            LogDebug("Init of channel 4, but volume is 0");
            return;
        }

        LogDebug("Channel 4 enabled");

        active = true;
    }
    else
    {
        active = false;
    }
}


void NoiseChannel::SetContinuous(bool cont)
{
    continuous = cont;
    //LogDebug("Channel 4 continuous=%d", cont);
}


void NoiseChannel::SetSoundLength(uint8_t length)
{
    soundLength = 64 - length;
    lengthCounter = soundLength;
    //LogDebug("Channel 4 length=%u", lengthCounter);
}


void NoiseChannel::SetVolume(uint8_t vol)
{
    masterVolume = vol;
    volume = vol;
}


void NoiseChannel::SetEnvelopeUp(bool up)
{
    envelopeUp = up;
}


void NoiseChannel::SetEnvelopeLength(uint8_t length)
{
    envelopeLength = length;
    ReloadEnvelopeCounter();
}


void NoiseChannel::SetDivRatio(uint8_t div)
{
    if (div == 0)
        divRatio = 8;
    else
        divRatio = div * 16;
}


void NoiseChannel::SetByteWidth(bool byte)
{
    byteWidth = byte;
}


void NoiseChannel::SetShiftFrequency(uint8_t freq)
{
    if ((freq & 0x0E) == 0x0E)
    {
        std::stringstream ss;
        ss << "Noise channel can't handle shift frequency 0x0E or 0x0F. Value given was 0x"
           << std::hex << std::setw(2) << std::setfill('0') << freq;
        throw std::range_error(ss.str());
    }

    shiftFrequency = freq;
}


void NoiseChannel::ReloadFrequencyCounter()
{
    frequencyCounter = (divRatio << shiftFrequency) / 4;
}


void NoiseChannel::ReloadLengthCounter()
{
    lengthCounter = soundLength;
}


void NoiseChannel::ReloadEnvelopeCounter()
{
    envelopeCounter = envelopeLength;
}