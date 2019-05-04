#include "Audio.h"
#include "AudioInterface.h"
#include "Logger.h"

// NR10
enum NR10Mask
{
    eNR10SweepShift = 0x07,
    eNR10SweepIncrease = 0x08,
    eNR10SweepTime = 0x70
};

// NR11, NR21
enum NRx1Mask
{
    eNRx1SoundLength = 0x3F,
    eNRx1DutyCycle = 0xC0
};

// NR12, NR22, NR42
enum NRx2Mask
{
    eNRx2EnvelopeLength = 0x07,
    eNRx2EnvelopeUpDown = 0x08,
    eNRx2EnvelopeValue = 0xF0
};

// NR14, NR24, NR34
enum NRx4Mask
{
    eNRx4Frequency = 0x07,
    eNRx4Continuous = 0x40,
    eNRx4Initialize = 0x80
};

// NR30
enum NR30Mask
{
    eNR30SoundOff = 0x80
};

// NR32
enum NR32Mask
{
    eNR32Level = 0x60
};

// NR41
enum NR41Mask
{
    eNR41Length = 0x3F
};

// NR43
enum NR43Mask
{
    eNR43DivRatio = 0x07,
    eNR43Steps = 0x08,
    eNR43ShiftClock = 0xf0
};

// NR44
enum NR44Mask
{
    eNR44Counter = 0x40,
    eNR44Initialize = 0x80
};

// NR50
enum NR50Mask
{
    eNR50SO1Level = 0x07,
    eNR50VinSO1On = 0x08,
    eNR50SO2Level = 0x70,
    eNR50VinSO2On = 0x80
};

// NR51
enum NR51Mask
{
    eNR51Ch1SO1 = 0x01,
    eNR51Ch2SO1 = 0x02,
    eNR51Ch3SO1 = 0x04,
    eNR51Ch4SO1 = 0x08,
    eNR51Ch1SO2 = 0x10,
    eNR51Ch2SO2 = 0x20,
    eNR51Ch3SO2 = 0x40,
    eNR51Ch4SO2 = 0x80
};

// NR52
enum NR52Mask
{
    eNR52Ch1On = 0x01,
    eNR52Ch2On = 0x02,
    eNR52Ch3On = 0x04,
    eNR52Ch4On = 0x08,
    eNR52AllSoundOn = 0x80
};


Audio::Audio(IoRegisterSubject *ioRegisterSubject, TimerSubject *timerSubject, AudioInterface *audioInterface) :
    audioInterface(audioInterface),
    channel1("1"),
    channel2("2"),
    channel3(),
    sampleCounter(0),
    bufferSize(0),
    clocksPerSample(CLOCKS_PER_SECOND / audioInterface->GetAudioSampleRate()),
    masterVolume(0),
    regNR10(ioRegisterSubject->AttachIoRegister(eRegNR10, this)),
    regNR11(ioRegisterSubject->AttachIoRegister(eRegNR11, this)),
    regNR12(ioRegisterSubject->AttachIoRegister(eRegNR12, this)),
    regNR13(ioRegisterSubject->AttachIoRegister(eRegNR13, this)),
    regNR14(ioRegisterSubject->AttachIoRegister(eRegNR14, this)),
    regNR21(ioRegisterSubject->AttachIoRegister(eRegNR21, this)),
    regNR22(ioRegisterSubject->AttachIoRegister(eRegNR22, this)),
    regNR23(ioRegisterSubject->AttachIoRegister(eRegNR23, this)),
    regNR24(ioRegisterSubject->AttachIoRegister(eRegNR24, this)),
    regNR30(ioRegisterSubject->AttachIoRegister(eRegNR30, this)),
    regNR31(ioRegisterSubject->AttachIoRegister(eRegNR31, this)),
    regNR32(ioRegisterSubject->AttachIoRegister(eRegNR32, this)),
    regNR33(ioRegisterSubject->AttachIoRegister(eRegNR33, this)),
    regNR34(ioRegisterSubject->AttachIoRegister(eRegNR34, this)),
    regNR41(ioRegisterSubject->AttachIoRegister(eRegNR41, this)),
    regNR42(ioRegisterSubject->AttachIoRegister(eRegNR42, this)),
    regNR43(ioRegisterSubject->AttachIoRegister(eRegNR43, this)),
    regNR44(ioRegisterSubject->AttachIoRegister(eRegNR44, this)),
    regNR50(ioRegisterSubject->AttachIoRegister(eRegNR50, this)),
    regNR51(ioRegisterSubject->AttachIoRegister(eRegNR51, this)),
    regNR52(ioRegisterSubject->AttachIoRegister(eRegNR52, this)),
    regWave0(ioRegisterSubject->AttachIoRegister(eRegWave0, this)),
    regWave1(ioRegisterSubject->AttachIoRegister(eRegWave1, this)),
    regWave2(ioRegisterSubject->AttachIoRegister(eRegWave2, this)),
    regWave3(ioRegisterSubject->AttachIoRegister(eRegWave3, this)),
    regWave4(ioRegisterSubject->AttachIoRegister(eRegWave4, this)),
    regWave5(ioRegisterSubject->AttachIoRegister(eRegWave5, this)),
    regWave6(ioRegisterSubject->AttachIoRegister(eRegWave6, this)),
    regWave7(ioRegisterSubject->AttachIoRegister(eRegWave7, this)),
    regWave8(ioRegisterSubject->AttachIoRegister(eRegWave8, this)),
    regWave9(ioRegisterSubject->AttachIoRegister(eRegWave9, this)),
    regWaveA(ioRegisterSubject->AttachIoRegister(eRegWaveA, this)),
    regWaveB(ioRegisterSubject->AttachIoRegister(eRegWaveB, this)),
    regWaveC(ioRegisterSubject->AttachIoRegister(eRegWaveC, this)),
    regWaveD(ioRegisterSubject->AttachIoRegister(eRegWaveD, this)),
    regWaveE(ioRegisterSubject->AttachIoRegister(eRegWaveE, this)),
    regWaveF(ioRegisterSubject->AttachIoRegister(eRegWaveF, this))
{
    timerSubject->AttachObserver(this);
    soundBuffer.fill(0);
    channel3.SetWaveTable(regWave0);
}


Audio::~Audio()
{

}


bool Audio::WriteByte(uint16_t address, uint8_t byte)
{
    LogInstruction("Audio::WriteByte %04X, %02X", address, byte);

    switch (address)
    {
        case eRegNR10:
            // Bit 7 is unused.
            *regNR10 = byte | 0x80;
            LogDebug("Sweep write %02X", byte);
            channel1.SetSweepTime((byte & eNR10SweepTime) >> 4);
            channel1.SetSweepUp(~byte & eNR10SweepIncrease);
            channel1.SetSweepShift(byte & eNR10SweepShift);
            return true;
        case eRegNR11:
            *regNR11 = byte;
            channel1.SetSoundLength(byte & eNRx1SoundLength);
            channel1.SetDutyCycle(static_cast<DutyCycle>(byte >> 6));
            return true;
        case eRegNR12:
            *regNR12 = byte;
            channel1.SetVolume(byte >> 4);
            channel1.SetEnvelopeUp(byte & eNRx2EnvelopeUpDown);
            channel1.SetEnvelopeLength(byte & eNRx2EnvelopeLength);
            return true;
        case eRegNR13:
            *regNR13 = byte;
            channel1.SetFrequency(((*regNR14 & eNRx4Frequency) << 8) | *regNR13);
            return true;
        case eRegNR14:
            // Bits 3-5 are unused.
            *regNR14 = byte | 0x38;
            channel1.SetFrequency(((*regNR14 & eNRx4Frequency) << 8) | *regNR13);
            channel1.SetContinuous(~byte & eNRx4Continuous);
            channel1.SetInitialize(byte & eNRx4Initialize);
            return true;
        case eRegNR21:
            *regNR21 = byte;
            channel2.SetSoundLength(byte & eNRx1SoundLength);
            channel2.SetDutyCycle(static_cast<DutyCycle>(byte >> 6));
            return true;
        case eRegNR22:
            *regNR22 = byte;
            channel2.SetVolume(byte >> 4);
            channel2.SetEnvelopeUp(byte & eNRx2EnvelopeUpDown);
            channel2.SetEnvelopeLength(byte & eNRx2EnvelopeLength);
            return true;
        case eRegNR23:
            *regNR23 = byte;
            channel2.SetFrequency(((*regNR24 & eNRx4Frequency) << 8) | *regNR23);
            return true;
        case eRegNR24:
            // Bits 3-5 are unused.
            *regNR24 = byte | 0x38;
            channel2.SetFrequency(((*regNR24 & eNRx4Frequency) << 8) | *regNR23);
            channel2.SetContinuous(~byte & eNRx4Continuous);
            channel2.SetInitialize(byte & eNRx4Initialize);
            return true;
        case eRegNR30:
            // Only bit 7 is used.
            *regNR30 = byte | 0x7F;
            channel3.SetEnabled(byte & eNR30SoundOff);
            return true;
        case eRegNR31:
            *regNR31 = byte;
            channel3.SetSoundLength(byte);
            return true;
        case eRegNR32:
            // Only bits 5 and 6 are used.
            *regNR32 = byte | 0x9F;
            channel3.SetVolume((byte & eNR32Level) >> 5);
            return true;
        case eRegNR33:
            *regNR33 = byte;
            channel3.SetFrequency(((*regNR34 & eNRx4Frequency) << 8) | *regNR33);
            return true;
        case eRegNR34:
            // Bits 3-5 are unused.
            *regNR34 = byte | 0x38;
            channel3.SetFrequency(((*regNR34 & eNRx4Frequency) << 8) | *regNR33);
            channel3.SetContinuous(~byte & eNRx4Continuous);
            channel3.SetInitialize(byte & eNRx4Initialize);
            return true;
        case eRegNR41:
            // Top 2 bits are unused.
            *regNR41 = byte | 0xC0;
            return true;
        case eRegNR42:
            *regNR42 = byte;
            return true;
        case eRegNR43:
            *regNR43 = byte;
            return true;
        case eRegNR44:
            // Only top 2 bits are used.
            *regNR44 = byte | 0x3F;
            return true;
        case eRegNR50:
            *regNR50 = byte;
            return true;
        case eRegNR51:
            *regNR51 = byte;
            return true;
        case eRegNR52:
            // Bits 4-6 are unused.
            *regNR52 = byte | 0x70;
            return true;
        case eRegWave0:
            *regWave0 = byte;
            return true;
        case eRegWave1:
            *regWave1 = byte;
            return true;
        case eRegWave2:
            *regWave2 = byte;
            return true;
        case eRegWave3:
            *regWave3 = byte;
            return true;
        case eRegWave4:
            *regWave4 = byte;
            return true;
        case eRegWave5:
            *regWave5 = byte;
            return true;
        case eRegWave6:
            *regWave6 = byte;
            return true;
        case eRegWave7:
            *regWave7 = byte;
            return true;
        case eRegWave8:
            *regWave8 = byte;
            return true;
        case eRegWave9:
            *regWave9 = byte;
            return true;
        case eRegWaveA:
            *regWaveA = byte;
            return true;
        case eRegWaveB:
            *regWaveB = byte;
            return true;
        case eRegWaveC:
            *regWaveC = byte;
            return true;
        case eRegWaveD:
            *regWaveD = byte;
            return true;
        case eRegWaveE:
            *regWaveE = byte;
            return true;
        case eRegWaveF:
            *regWaveF = byte;
            return true;
        default:
            return false;
    }
}


uint8_t Audio::ReadByte(uint16_t address) const
{
    LogInstruction("Audio::ReadByte %04X", address);

    switch (address)
    {
        case eRegNR10:
            // Bit 7 is unused.
            return *regNR10 | 0x80;
        case eRegNR11:
            // Only bits 6 and 7 are readable.
            return *regNR11 | 0x3F;
        case eRegNR12:
            return *regNR12;
        case eRegNR13:
            // Write-only.
            return 0xFF;
        case eRegNR14:
            // Only bit 6 is readable.
            return *regNR14 | 0xBF;
        case eRegNR21:
            // Only bits 6 and 7 are readable.
            return *regNR21 | 0x3F;
        case eRegNR22:
            return *regNR22;
        case eRegNR23:
            // Write-only.
            return 0xFF;
        case eRegNR24:
            // Only bit 6 is readable.
            return *regNR24 | 0xBF;
        case eRegNR30:
            // Only bit 7 is used.
            return *regNR30 | 0x7F;
        case eRegNR31:
            return 0xFF;
        case eRegNR32:
            // Only bits 5 and 6 are used.
            return *regNR32 | 0x9F;
        case eRegNR33:
            // Write-only.
            return 0xFF;
        case eRegNR34:
            // Only bit 6 is readable.
            return *regNR34 | 0xBF;
        case eRegNR41:
            // Top 2 bits are unused.
            return *regNR41 | 0xC0;
        case eRegNR42:
            return *regNR42;
        case eRegNR43:
            return *regNR43;
        case eRegNR44:
            // Only bit 6 is readable.
            return *regNR44 | 0xBF;
        case eRegNR50:
            return *regNR50;
        case eRegNR51:
            return *regNR51;
        case eRegNR52:
            // Bits 4-6 are unused.
            return *regNR52 | 0x70;
        case eRegWave0:
            return *regWave0;
        case eRegWave1:
            return *regWave1;
        case eRegWave2:
            return *regWave2;
        case eRegWave3:
            return *regWave3;
        case eRegWave4:
            return *regWave4;
        case eRegWave5:
            return *regWave5;
        case eRegWave6:
            return *regWave6;
        case eRegWave7:
            return *regWave7;
        case eRegWave8:
            return *regWave8;
        case eRegWave9:
            return *regWave9;
        case eRegWaveA:
            return *regWaveA;
        case eRegWaveB:
            return *regWaveB;
        case eRegWaveC:
            return *regWaveC;
        case eRegWaveD:
            return *regWaveD;
        case eRegWaveE:
            return *regWaveE;
        case eRegWaveF:
            return *regWaveF;
        default:
            std::stringstream ss;
            ss << "Audio doesnt handle reads to 0x" << std::hex << std::setw(4) << std::setfill('0') << address;
            throw std::range_error(ss.str());
    }
}


void Audio::UpdateTimer(uint value)
{
    sampleCounter += value;

    if (!(*regNR52 & eNR52AllSoundOn))
        return;

    if (sampleCounter >= clocksPerSample)
    {
        channel1.Tick(sampleCounter);
        channel2.Tick(sampleCounter);
        channel3.Tick(sampleCounter);

        sampleCounter = 0;

        const uint8_t leftVolume = (*regNR50 & eNR50SO2Level) >> 4;
        const uint8_t rightVolume = (*regNR50 & eNR50SO1Level);

        const uint8_t ch1Value = channel1.GetSample();
        const uint8_t ch2Value = channel2.GetSample();
        const uint8_t ch3Value = channel3.GetSample();

        uint8_t leftSample = 0;
        uint8_t rightSample = 0;

        // Mix left channel.
        if (*regNR51 & eNR51Ch1SO2)
            leftSample += ch1Value * leftVolume / 4;
        if (*regNR51 & eNR51Ch2SO2)
            leftSample += ch2Value * leftVolume / 4;
        if (*regNR51 & eNR51Ch3SO2)
            leftSample += ch3Value * leftVolume / 4;
        soundBuffer[bufferSize] = leftSample;
        bufferSize++;

        // Mix right channel.
        if (*regNR51 & eNR51Ch1SO1)
            rightSample += ch1Value * rightVolume / 4;
        if (*regNR51 & eNR51Ch2SO1)
            rightSample += ch2Value * rightVolume / 4;
        if (*regNR51 & eNR51Ch3SO1)
            rightSample += ch3Value * rightVolume / 4;
        soundBuffer[bufferSize] = rightSample;
        bufferSize++;

        if (bufferSize >= AudioInterface::BUFFER_LEN)
        {
            audioInterface->AudioDataReady(soundBuffer);
            soundBuffer.fill(0);
            bufferSize = 0;
        }
    }
}