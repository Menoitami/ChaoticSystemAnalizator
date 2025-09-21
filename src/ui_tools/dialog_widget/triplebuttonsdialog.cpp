#include "triplebuttonsdialog.h"
#include "style_tools.h"
#include "ui_triplebuttonsdialog.h"

#include <QDebug>
#include <QFile>
#include <QGraphicsOpacityEffect>

TripleButtonsDialog::TripleButtonsDialog(QWidget *parent) : QWidget(parent), ui(new Ui::TripleButtonsDialog)
{
    qDebug() << __FUNCTION__;
    ui->setupUi(this);

    setAttribute(Qt::WA_StyledBackground);

    centerWidget();
    makeShadow(this);
}

TripleButtonsDialog::~TripleButtonsDialog()
{
    window()->setGraphicsEffect(nullptr);

    delete ui;
}

void TripleButtonsDialog::centerWidget()
{
    if (!window())
        return;

    QRect parentRect = window()->geometry();

    int x = (parentRect.width() - width()) / 2;
    int y = (parentRect.height() - height()) / 2;

    move(x, y);
}
void TripleButtonsDialog::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    centerWidget();
    adjustSize();
}
void TripleButtonsDialog::addContentWidget(QWidget *widget)
{
    ui->contentLayout->addWidget(widget);
    adjustSize();
    update();
}
void TripleButtonsDialog::setHeaderText(const QString &text) { ui->header->setText(text); }
