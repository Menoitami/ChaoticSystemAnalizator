#include "customdialog.h"
#include "style_tools.h"
#include "ui_customdialog.h"

#include <QDebug>
#include <QFile>
#include <QGraphicsOpacityEffect>

CustomDialog::CustomDialog(QWidget *parent) : QWidget(parent), ui(new Ui::CustomDialog)
{
    qDebug() << __FUNCTION__;
    ui->setupUi(this);

    setAttribute(Qt::WA_StyledBackground);

    centerWidget();
    makeShadow(this);
}

CustomDialog::~CustomDialog()
{
    window()->setGraphicsEffect(nullptr);

    delete ui;
}

void CustomDialog::centerWidget()
{
    if (!window())
        return;

    QRect parentRect = window()->geometry();

    int x = (parentRect.width() - width()) / 2;
    int y = (parentRect.height() - height()) / 2;

    move(x, y);
}
void CustomDialog::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    centerWidget();
    adjustSize();
}
void CustomDialog::addContentWidget(QWidget *widget)
{
    ui->contentLayout->addWidget(widget);
    adjustSize();
    update();
}

void CustomDialog::addContentSubLayout(QLayout *layout)
{
    ui->contentLayout->addLayout(layout, true);
    adjustSize();
    update();
}

void CustomDialog::setHeaderText(const QString &text) { ui->header->setText(text); }
