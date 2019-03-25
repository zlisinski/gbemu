#include <algorithm>
#include <QtGui/QPalette>

#include "DisassemblyModel.h"
#include "Opcode.h"


const QSet<uint8_t> DisassemblyModel::stopOpcodes = {
    0xC3, //JP nn
    0xE9, //JP (HL)
    0x18, //JR e
    0xCD, //CALL
    0xC9, //RET
    0xD9, //RETI
    0xC7, //RST 0x00
    0xCF, //RST 0x08
    0xD7, //RST 0x10
    0xDF, //RST 0x18
    0xE7, //RST 0x20
    0xEF, //RST 0x28
    0xF7, //RST 0x30
    0xFF  //RST 0x38
};


DisassemblyModel::DisassemblyModel(const QPalette &palette, QObject *parent /*= NULL*/) :
    QAbstractTableModel(parent),
    currentRow(-1),
    palette(palette),
    currentRowColor(palette.color(QPalette::AlternateBase))
{

}


DisassemblyModel::~DisassemblyModel()
{

}


void DisassemblyModel::AddRow(uint16_t pc, const uint8_t *memory)
{
    int curRowCount = opcodes.size();

    if (addresses.contains(pc))
        return;

    uint16_t origPc = pc;

    // Disassemble opcodes until we reach a previously reached address, or an unconditional jump/call/return.
    std::vector<Opcode> newRows;
    do
    {
        Opcode opcode = Opcode::GetOpcode(pc, memory + pc);
        newRows.push_back(opcode);
        addresses.insert(pc);

        // Stop if we reach an opcode that would jump/return.
        if (stopOpcodes.contains(memory[pc]))
            break;

        // Advance the program counter to the next instruction.
        pc += opcode.GetByteCount();

    } while (pc <= 0xFFFE && !addresses.contains(pc));
    
    // Find where to insert the new rows.
    int insertIndex = 0;
    auto it = std::find_if(opcodes.cbegin(), opcodes.cend(), [&origPc](const Opcode &item){return item.GetAddress() > origPc;});
    if (it == opcodes.cend())
        insertIndex = curRowCount;
    else
        insertIndex = std::distance(opcodes.cbegin(), it);

    beginInsertRows(QModelIndex(), insertIndex, insertIndex + newRows.size() - 1);
    
    opcodes.insert(it, newRows.cbegin(), newRows.cend());
    
    endInsertRows();
}


void DisassemblyModel::RemoveRows(uint16_t address, uint16_t len)
{
    // Find the first opcode that contains address.
    auto startIt = std::find_if(opcodes.cbegin(), opcodes.cend(),
        [&address](const Opcode &item)
        {
            return address >= item.GetAddress() && address < item.GetAddress() + item.GetByteCount();
        });

    if (startIt == opcodes.cend())
        return;

    // Find the first element past the end of address+len.
    auto endIt = startIt + 1;
    if (len > 1)
    {
        uint16_t endAddr = address + len;
        endIt = std::find_if(opcodes.cbegin(), opcodes.cend(),
            [&endAddr](const Opcode &item)
            {
                return item.GetAddress() + item.GetByteCount() > endAddr;
            });
    }

    int startIndex = std::distance(opcodes.cbegin(), startIt);
    int endIndex = std::distance(opcodes.cbegin(), endIt) - 1;

    for (auto it = startIt; it < endIt; ++it)
        addresses.remove(it->GetAddress());

    beginRemoveRows(QModelIndex(), startIndex, endIndex);
    opcodes.erase(startIt, endIt);
    endRemoveRows();
}


int DisassemblyModel::GetRowIndex(uint16_t pc) const
{
    auto it = std::find_if(opcodes.cbegin(), opcodes.cend(), [&pc](const Opcode &item){return item.GetAddress() == pc;});
    
    if (it == opcodes.cend())
        return -1;
    
    return std::distance(opcodes.cbegin(), it);
}


int DisassemblyModel::rowCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    Q_UNUSED(parent);

    return opcodes.size();
}


int DisassemblyModel::columnCount(const QModelIndex &parent /*= QModelIndex()*/) const
{
    Q_UNUSED(parent);

    return colCount;
}


QVariant DisassemblyModel::data(const QModelIndex &index, int role /*= Qt::DisplayRole*/) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= (int)opcodes.size())
        return QVariant();

    if (index.column() >= colCount)
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        const Opcode &item = opcodes[index.row()];
        switch (index.column())
        {
            case 0:
                return item.GetBytesStr();
            case 1:
                return item.GetOpcodeStr();
            case 2:
                return item.GetArg1Str();
            case 3:
                return item.GetArg2Str();
        }
    }
    else if (role == Qt::BackgroundRole)
    {
        if (index.row() == currentRow)
            return currentRowColor;
    }

    return QVariant();
}


QVariant DisassemblyModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    
    if (section >= (int)opcodes.size())
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        return QVariant();
    }
    else
    {
        uint16_t pc = opcodes[section].GetAddress();
        return QStringLiteral("%1").arg(pc, 4, 16, QChar('0')).toUpper();
    }
}