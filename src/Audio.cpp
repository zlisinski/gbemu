#include "Audio.h"
#include "Logger.h"


Audio::Audio(IoRegisterSubject *ioRegisterSubject) :
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

}


Audio::~Audio()
{

}


bool Audio::WriteByte(uint16_t address, uint8_t byte)
{
    LogInstruction("Display::WriteByte %04X, %02X", address, byte);

    switch (address)
    {
        case eRegNR10:
            // Bit 7 is unused.
            *regNR10 = byte | 0x80;
            return true;
        case eRegNR11:
            *regNR11 = byte;
            return true;
        case eRegNR12:
            *regNR12 = byte;
            return true;
        case eRegNR13:
            *regNR13 = byte;
            return true;
        case eRegNR14:
            // Bits 3-5 are unused.
            *regNR14 = byte | 0x38;
            return true;
        case eRegNR21:
            *regNR21 = byte;
            return true;
        case eRegNR22:
            *regNR22 = byte;
            return true;
        case eRegNR23:
            *regNR23 = byte;
            return true;
        case eRegNR24:
            // Bits 3-5 are unused.
            *regNR24 = byte | 0x38;
            return true;
        case eRegNR30:
            // Only bit 7 is used.
            *regNR30 = byte | 0x7F;
            return true;
        case eRegNR31:
            *regNR31 = byte;
            return true;
        case eRegNR32:
            // Only bits 5 and 6 are used.
            *regNR32 = byte | 0x9F;
            return true;
        case eRegNR33:
            *regNR33 = byte;
            return true;
        case eRegNR34:
            // Bits 3-5 are unused.
            *regNR34 = byte | 0x38;
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
            return *regNR31;
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
