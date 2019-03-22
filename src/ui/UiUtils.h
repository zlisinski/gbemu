#pragma once

#include <stdint.h>
#include <QtCore/QString>

class UiUtils
{
public:
    static QString FormatHexByte(uint8_t num)
    {
        return QStringLiteral("%1").arg(num, 2, 16, QChar('0')).toUpper();
    }

    static QString FormatHexWord(uint16_t num)
    {
        return QStringLiteral("%1").arg(num, 4, 16, QChar('0')).toUpper();
    }
};