#include "basewidget.h"
#include "templatefieldwidget.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QTimer>

#include <QPushButton>

BaseWidget::BaseWidget( SA::CustomWidgets name_, QWidget *parent) : QWidget(parent), name(name_) {}

BaseWidget::~BaseWidget() {}

void BaseWidget::connectAllWidgetsToSaveBtn(QWidget *widgetContainer)
{
    if (!widgetContainer)
        return;

    std::function<void(QWidget *)> connectWidgets;

    connectWidgets = [this, &connectWidgets](QWidget *container)
    {
        const auto &children = container->findChildren<QWidget *>();

        for (auto *child : children)
        {
            if (QLineEdit *le = qobject_cast<QLineEdit *>(child))
            {
                connect(le, &QLineEdit::textChanged, this, [this]() { emit enableSave(true); });
            }
            else if (QTextEdit *te = qobject_cast<QTextEdit *>(child))
            {
                connect(te, &QTextEdit::textChanged, this, [this]() { emit enableSave(true); });
            }
            else if (QComboBox *cb = qobject_cast<QComboBox *>(child))
            {
                connect(cb, &QComboBox::currentTextChanged, this, [this]() { emit enableSave(true); });
            }
            else if (QSpinBox *sb = qobject_cast<QSpinBox *>(child))
            {
                connect(sb, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int) { emit enableSave(true); });
            }
            else if (QDoubleSpinBox *dsb = qobject_cast<QDoubleSpinBox *>(child))
            {
                connect(dsb, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
                        [this](double) { emit enableSave(true); });
            }
            else if (QCheckBox *cb = qobject_cast<QCheckBox *>(child))
            {
                connect(cb, &QCheckBox::checkStateChanged, this, [this](int) { emit enableSave(true); });
            }
            else if (QRadioButton *rb = qobject_cast<QRadioButton *>(child))
            {
                connect(rb, &QRadioButton::toggled, this, [this](bool) { emit enableSave(true); });
            }
            else if (QSlider *sl = qobject_cast<QSlider *>(child))
            {
                connect(sl, &QSlider::valueChanged, this, [this](int) { emit enableSave(true); });
            }
            else if (QDateEdit *de = qobject_cast<QDateEdit *>(child))
            {
                connect(de, &QDateEdit::dateChanged, this, [this](const QDate &) { emit enableSave(true); });
            }
            else if (QTimeEdit *te = qobject_cast<QTimeEdit *>(child))
            {
                connect(te, &QTimeEdit::timeChanged, this, [this](const QTime &) { emit enableSave(true); });
            }
            else if (QDateTimeEdit *dte = qobject_cast<QDateTimeEdit *>(child))
            {
                connect(dte, &QDateTimeEdit::dateTimeChanged, this,
                        [this](const QDateTime &) { emit enableSave(true); });
            }

            if (QWidget *childWidget = qobject_cast<QWidget *>(child))
            {
                connectWidgets(childWidget);
            }
        }
    };

    connectWidgets(widgetContainer);
}
