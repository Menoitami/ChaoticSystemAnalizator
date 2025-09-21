#pragma once

#include <qcombobox.h>

class CustomComboBox : public QComboBox
{
    Q_OBJECT
  public:
    CustomComboBox(QWidget *parent = nullptr);
};

#include <QPainter>
#include <QStyledItemDelegate>

class CustomComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

  public:
    explicit CustomComboBoxDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
