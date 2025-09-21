// prefixawarevalidator.h
#pragma once

#include <QString>
#include <QValidator>

class PrefixAwareValidator : public QValidator
{
    Q_OBJECT
    /*
     используется для LineEdit, чтобы была возможность добавлять restrictInputValidator в поля ввода с префиксом
     */

  public:
    PrefixAwareValidator(const QString &prefix, QValidator *wrappedValidator, QObject *parent = nullptr);

    State validate(QString &input, int &pos) const override;

  private:
    QString prefix_;
    QValidator *wrappedValidator_;
};
