#include "lineedit.h"
#include "prefixawarevalidator.h"
#include "ui_lineedit.h"
#include <QDebug>
#include <QStyle>
#include <QVBoxLayout>
#include <QValidator>

LineEdit::LineEdit(QWidget *parent) : QWidget(parent), ui(new Ui::LineEdit)
{
    ui->setupUi(this);
    ui->error->hide();
    setErrorState(false);

    connect(ui->lineEdit, &QLineEdit::textChanged, this, &LineEdit::textChanged);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &LineEdit::validate);
}

LineEdit::~LineEdit() { delete ui; }

bool LineEdit::valid()
{
    validate(ui->lineEdit->text());
    return ui->lineEdit->property("State") != "Error";
}

void LineEdit::setRestrictInputValidator(std::shared_ptr<QValidator> restrictInputValidator,
                                         bool showErrorStateInIntermidiate)
{
    restrictInputValidator_ = restrictInputValidator;
    showErrorStateInIntermidiate_ = showErrorStateInIntermidiate;

    if (prefix.has_value())
    {
        auto *wrapped = new PrefixAwareValidator(prefix.value(), restrictInputValidator.get(), this);
        ui->lineEdit->setValidator(wrapped);
    }
    else
    {
        ui->lineEdit->setValidator(restrictInputValidator.get());
    }
}

void LineEdit::addErrorValidator(QString error, std::shared_ptr<QValidator> showErrorValidator)
{
    errorsAndValidators_.append({error, showErrorValidator});
}

void LineEdit::setLabel(const QString &labelText)
{
    auto label = new QLabel(this);
    label->setText(labelText);
    label->setObjectName("label");

    ui->verticalLayout->insertWidget(0, label);
}

void LineEdit::validate(const QString &fullText)
{
    if (prefix.has_value() && !fullText.startsWith(prefix.value()))
    {
        setText("");
        return;
    }

    QString userText = prefix.has_value() ? fullText.mid(prefix.value().length()) : fullText;

    QString shownError;
    int dummy;

    if (restrictInputValidator_)
    {
        auto result = restrictInputValidator_->validate(userText, dummy);
        if (showErrorStateInIntermidiate_ && result == QValidator::Intermediate)
        {
            setErrorState();
            return;
        }
    }

    for (const auto &[error, validator] : std::as_const(errorsAndValidators_))
    {
        if (validator->validate(userText, dummy) == QValidator::Invalid)
        {
            shownError = error;
            break;
        }
    }

    if (shownError.isEmpty())
    {
        for (const auto &[error, validator] : crossValidators_)
        {
            if (!validator())
            {
                shownError = error;
                break;
            }
        }
    }

    showError(shownError);
}

void LineEdit::showError(const QString &error)
{
    if (error.isEmpty())
    {
        ui->error->hide();
        setStyleSheet("");
        setErrorState(false);
    }
    else
    {
        ui->error->setText(error);
        ui->error->show();
        setErrorState();
    }
}

void LineEdit::setErrorState(bool err)
{
    if (err)
    {
        ui->lineEdit->setProperty("State", "Error");
    }
    else
    {
        ui->lineEdit->setProperty("State", "Normal");
    }
    ui->lineEdit->style()->unpolish(ui->lineEdit);
    ui->lineEdit->style()->polish(ui->lineEdit);
}

void LineEdit::addCrossValidator(QString error, CrossValidator validator)
{
    crossValidators_.append({error, validator});
}

void LineEdit::setText(const QString &inputText)
{
    QString finalText = prefix.has_value() ? prefix.value() + inputText : inputText;
    ui->lineEdit->setText(finalText);
}

QString LineEdit::text() const
{
    QString full = ui->lineEdit->text();
    if (prefix.has_value() && full.startsWith(prefix.value()))
    {
        return full.mid(prefix.value().length());
    }
    return full;
}

void LineEdit::setPlaceholder(const QString &text) { ui->lineEdit->setPlaceholderText(text); }

void LineEdit::hideError() { ui->error->hide(); }

void LineEdit::setConstantPrefix(QString prefixStr)
{
    prefix = prefixStr;
    QLineEdit *le = ui->lineEdit;

    if (!le->text().startsWith(prefixStr))
    {
        le->blockSignals(true);
        le->setText(prefixStr);
        le->setCursorPosition(le->text().length());
        le->blockSignals(false);
    }

    if (restrictInputValidator_)
    {
        auto *wrapped = new PrefixAwareValidator(prefixStr, restrictInputValidator_.get(), this);
        le->setValidator(wrapped);
    }
}

void LineEdit::setMinFieldHeight(int size) { ui->lineEdit->setMinimumHeight(size); }
void LineEdit::focusInEvent(QFocusEvent *event)
{
    ui->lineEdit->setFocus();
    ui->lineEdit->setCursorPosition(ui->lineEdit->text().length());
}
