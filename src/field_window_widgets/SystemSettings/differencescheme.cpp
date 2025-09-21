#include "differencescheme.h"
#include "ui_differencescheme.h"

DifferenceScheme::DifferenceScheme(QWidget *parent) : QWidget(parent), ui(new Ui::DifferenceScheme)
{
    ui->setupUi(this);
}

DifferenceScheme::~DifferenceScheme() { delete ui; }

QTextEdit *const DifferenceScheme::getTextEdit() const { return ui->textEdit; }

QString DifferenceScheme::getScheme() const { return ui->textEdit->toPlainText().trimmed(); }
