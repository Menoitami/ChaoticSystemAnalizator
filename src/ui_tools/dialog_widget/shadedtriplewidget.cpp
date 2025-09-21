#include "shadedtriplewidget.h"
#include "stylesheetprocessor.h"
#include "triplebuttonsdialog.h"
#include "ui_triplebuttonsdialog.h"

#include <QDebug>
#include <QGraphicsOpacityEffect>

ShadedTripleWidget::ShadedTripleWidget(QWidget *parent) : QWidget(parent)
{
    overlay = std::make_unique<QWidget>(window());
    shadeWindow();

    dialog = std::make_unique<TripleButtonsDialog>(window());

    setAttribute(Qt::WA_StyledBackground);
    StyleSheetProcessor::applyStyle(":/styles/TripleButtonsDialog.qss", this);

    dialog->show();

    connect(dialog->ui->cancelButton, &QPushButton::clicked, this, [this]() { emit cancel(); });
    connect(dialog->ui->yesButton, &QPushButton::clicked, this, [this]() { emit accepted(); });
    connect(dialog->ui->noButton, &QPushButton::clicked, this, [this]() { emit rejected(); });
}

void ShadedTripleWidget::setShadeCoeff(int shade)
{
    if (shade < 0 || shade > 100)
    {
        qWarning() << "wrong shade coeff passed";
        return;
    }
    shadeCoeff = shade;
}

void ShadedTripleWidget::shadeWindow()
{
    overlay->setStyleSheet(QString("background-color: rgba(0, 0, 0, %1);").arg(shadeCoeff * 2.56));
    overlay->setGeometry(window()->rect());
    overlay->show();
}

void ShadedTripleWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    shadeWindow();
}

void ShadedTripleWidget::addContentWidget(QWidget *widget) { dialog->addContentWidget(widget); }

void ShadedTripleWidget::setHeaderText(const QString &text) { dialog->setHeaderText(text); }
