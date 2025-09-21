#include "gridtable.h"
#include <QDebug>
#include <QLabel>
#include <QLayoutItem>
#include <QPushButton>
#include <QStyle>

GridTable::GridTable(const QVector<ColumnInfo> &columnInfos, bool enableRemoveColumn, QWidget *parent)
    : QWidget(parent), gridLayout(new QGridLayout(this)), columnInfos(columnInfos),
      m_enableRemoveColumn(enableRemoveColumn)
{
    setAttribute(Qt::WA_StyledBackground);
    setObjectName("GridTable");
    gridLayout->setSpacing(0);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(gridLayout);

    setupHeaders();
    setupStyling();
}

void GridTable::setupHeaders()
{
    // Добавляем заголовки колонок
    for (int i = 0; i < columnInfos.size(); ++i)
    {
        QLabel *headerLabel = new QLabel(columnInfos[i].title);
        headerLabel->setProperty("Table", "header");
        headerLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(headerLabel, 0, i);
        gridLayout->setColumnMinimumWidth(i, columnInfos[i].width);
    }

    // Добавляем заголовок "Действия" только если включено
    if (m_enableRemoveColumn)
    {
        QLabel *actionHeader = new QLabel("");
        actionHeader->setProperty("Table", "header");
        actionHeader->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        gridLayout->addWidget(actionHeader, 0, columnInfos.size());
        gridLayout->setColumnMinimumWidth(columnInfos.size(), 36); // Ширина колонки с кнопкой
    }
}

void GridTable::setupStyling()
{
    // Можно применить стили через внешний QSS
    // Например: StyleSheetProcessor::applyStyle(...)
}

void GridTable::addRow(const QVector<QVariant> &rowData)
{
    int insertIndex = findInsertIndex(rowData);

    auto createLabel = [](const QVariant &data)
    {
        QLabel *label = new QLabel(data.toString());
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        return label;
    };

    QVector<QLabel *> labels;
    for (int i = 0; i < rowData.size(); ++i)
    {
        labels.append(createLabel(rowData[i]));
    }

    // Цвет фона по чётности строки
    QString tableStyle = (nextRowId % 2 == 0) ? "accent" : "default";
    for (auto label : labels)
    {
        label->setProperty("Table", tableStyle);
        label->style()->unpolish(label);
        label->style()->polish(label);
    }

    // Вставляем ячейки данных
    for (int i = 0; i < labels.size(); ++i)
    {
        gridLayout->addWidget(labels[i], insertIndex, i);
    }

    // Добавляем кнопку удаления, если разрешено
    if (m_enableRemoveColumn)
    {
        QPushButton *removeButton = new QPushButton("🗑️");
        removeButton->setFlat(true);
        removeButton->setStyleSheet("font-size: 16px; color: red;");
        connectRemoveButton(removeButton, nextRowId);

        gridLayout->addWidget(removeButton, insertIndex, columnInfos.size());
    }

    nextRowId++;
}

int GridTable::findInsertIndex(const QVector<QVariant> &rowData) const
{
    return gridLayout->rowCount(); // Простая вставка в конец
}

void GridTable::clearRows()
{
    int totalCols = columnInfos.size() + (m_enableRemoveColumn ? 1 : 0);

    for (int row = 1; row < gridLayout->rowCount(); ++row)
    {
        for (int col = 0; col < totalCols; ++col)
        {
            QLayoutItem *item = gridLayout->itemAtPosition(row, col);
            if (item && item->widget())
            {
                delete item->widget();
                gridLayout->removeItem(item);
                delete item;
            }
        }
    }

    nextRowId = 1;
}

void GridTable::connectRemoveButton(QPushButton *button, int rowId)
{
    connect(button, &QPushButton::clicked,
            [this, rowId]()
            {
                removeRow(rowId);
                emit rowRemoved(rowId);
            });
}

void GridTable::removeRow(int rowId)
{
    for (int row = 1; row < gridLayout->rowCount(); ++row)
    {
        QLayoutItem *item = gridLayout->itemAtPosition(row, 0); // Первый столбец — ID
        if (item && item->widget())
        {
            QLabel *label = qobject_cast<QLabel *>(item->widget());
            if (label && label->text() == QString::number(rowId))
            {
                int totalCols = columnInfos.size() + (m_enableRemoveColumn ? 1 : 0);
                for (int col = 0; col < totalCols; ++col)
                {
                    QLayoutItem *cellItem = gridLayout->itemAtPosition(row, col);
                    if (cellItem && cellItem->widget())
                    {
                        gridLayout->removeItem(cellItem);
                        delete cellItem->widget();
                        delete cellItem;
                    }
                }
                break;
            }
        }
    }
}

int GridTable::rowCount()
{ return gridLayout->rowCount();
}
