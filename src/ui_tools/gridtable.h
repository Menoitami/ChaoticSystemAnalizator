#pragma once
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QWidget>

class GridTable : public QWidget
{
    Q_OBJECT

  public:
    struct ColumnInfo
    {
        QString title;
        int width;
        bool sortable = true;

        ColumnInfo(const QString &t, int w, bool s = true) : title(t), width(w), sortable(s) {}
    };

    explicit GridTable(const QVector<ColumnInfo> &columnInfos, bool enableRemoveColumn = false,
                       QWidget *parent = nullptr);

    void addRow(const QVector<QVariant> &rowData);
    void clearRows();
    void removeRow(int rowId); // Удаление строки по ID
    int rowCount();

  private:
    QGridLayout *gridLayout;
    QVector<ColumnInfo> columnInfos;
    bool m_enableRemoveColumn;
    int nextRowId = 1;

    void setupHeaders();
    void setupStyling();
    int findInsertIndex(const QVector<QVariant> &rowData) const;
    void connectRemoveButton(QPushButton *button, int rowId);

  signals:
    void rowRemoved(int rowId); // Сигнал при удалении строки
};
