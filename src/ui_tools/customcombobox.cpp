#include "customcombobox.h"
#include "colorpalette.h"
#include "style_tools.h"
#include <qabstractitemview.h>
#include <qpainterpath.h>
#include <qsvgrenderer.h>

CustomComboBox::CustomComboBox(QWidget *parent) : QComboBox(parent)
{
    setItemDelegate(new CustomComboBoxDelegate(this));
}

CustomComboBoxDelegate::CustomComboBoxDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void CustomComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    painter->save();

    QString text = index.data(Qt::DisplayRole).toString();
    QColor textColor = Qt::black;

    if (option.state & QStyle::State_Selected)
    {
        textColor = ColorPalette::palette["accent_080"];
    }
    else if (option.state & QStyle::State_MouseOver)
    {
        textColor = ColorPalette::palette["accent_080"];
    }
    else if (index.data(Qt::ForegroundRole).canConvert<QColor>())
    {
        textColor = qvariant_cast<QColor>(index.data(Qt::ForegroundRole));
    }

    // Цвет фона
    QColor bgColor = Qt::white;
    if (option.state & QStyle::State_Selected)
    {
        bgColor = ColorPalette::palette["accent_010"];
    }
    else if (option.state & QStyle::State_MouseOver)
    {
        bgColor = ColorPalette::palette["accent_010"];
    }

    QPainterPath path;
    path.addRoundedRect(option.rect, 6, 6);
    painter->fillPath(path, bgColor);

    // Настройка шрифта
    QFont font = painter->font();
    font.setFamily("Inter");
    font.setPixelSize(14);
    painter->setFont(font);
    painter->setPen(textColor);

    // Отступы текста
    QRect textRect = option.rect.adjusted(12, 0, -12, 0);
    painter->drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, text);

    painter->restore();
}

QSize CustomComboBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    return QSize(size.width(), size.height() + 16);
}
