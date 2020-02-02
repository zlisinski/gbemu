#pragma once

#include <QtCore/QString>

#include "core/gbemu.h"

class Opcode
{
public:
    static Opcode GetOpcode(uint16_t pc, const uint8_t *memory);

    QString ToString() const;
    QString GetBytesStr() const {return bytesStr;}
    QString GetOpcodeStr() const {return opcodeStr;}
    QString GetArg1Str() const {return arg1Str;}
    QString GetArg2Str() const {return arg2Str;}
    uint16_t GetAddress() const {return address;}
    int GetByteCount() const {return byteCount;}

private:
    Opcode(int byteCount, const QString &opcodeStr, const QString &arg1Str = "", const QString &arg2Str = "");
    void FormatArg(QString& str, uint16_t pc, const uint8_t *memory);

    QString bytesStr;
    QString opcodeStr;
    QString arg1Str;
    QString arg2Str;
    uint16_t address;
    int byteCount;

    static const Opcode opcodes[256];
    static const Opcode cbOpcodes[256];
};
