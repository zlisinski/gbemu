#pragma once

#include <QtCore/QAbstractTableModel>
#include <QtCore/QSet>
#include <vector>

#include "../../gbemu.h"
#include "Opcode.h"

class DisassemblyModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    DisassemblyModel(const QPalette &palette, QObject *parent = NULL);
    virtual ~DisassemblyModel();

    void AddRow(uint16_t pc, const uint8_t *memory);
    int GetRowIndex(uint16_t pc);
    void SetCurrentRow(int row) {currentRow = row;}

    // Overrides for QAbstractTableModel.
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    static const int colCount = 4;
    static const QSet<uint8_t> stopOpcodes;

    std::vector<Opcode> opcodes;
    QSet<uint16_t> addresses;

    int currentRow;
    const QPalette &palette;
    const QColor &currentRowColor;
};