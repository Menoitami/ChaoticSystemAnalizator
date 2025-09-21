#include "shadeddialogwidget.h"
#include "customdialog.h"
#include "stylesheetprocessor.h"
#include "ui_customdialog.h"

#include <QDebug>
#include <QGraphicsOpacityEffect>

ShadedDialogWidget::ShadedDialogWidget(QWidget *parent) : QWidget(parent)
{
    overlay = std::make_unique<QWidget>(window());
    shadeWindow();

    dialog = std::make_unique<CustomDialog>(window());

    setAttribute(Qt::WA_StyledBackground);
    StyleSheetProcessor::applyStyle(":/styles/CustomDialog.qss", this);

    dialog->show();

    connect(dialog->ui->yesButton, &QPushButton::clicked, this, [this]() { emit accepted(); });
    connect(dialog->ui->noButton, &QPushButton::clicked, this, [this]() { emit rejected(); });
}

void ShadedDialogWidget::setShadeCoeff(int shade)
{
    if (shade < 0 || shade > 100)
    {
        qWarning() << "wrong shade coeff passed";
        return;
    }
    shadeCoeff = shade;
    shadeWindow();
}

void ShadedDialogWidget::shadeWindow()
{
    overlay->setStyleSheet(QString("background-color: rgba(0, 0, 0, %1);").arg(shadeCoeff * 2.56));
    overlay->setGeometry(window()->rect());
    overlay->show();
}

void ShadedDialogWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    shadeWindow();
}

void ShadedDialogWidget::addContentWidget(QWidget *widget) { dialog->addContentWidget(widget); }

void ShadedDialogWidget::setHeaderText(const QString &text) { dialog->setHeaderText(text); }

void ShadedDialogWidget::setSaveEnabled(bool st) { dialog->ui->yesButton->setEnabled(st); }
