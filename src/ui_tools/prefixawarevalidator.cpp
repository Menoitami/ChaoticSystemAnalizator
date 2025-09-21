#include "prefixawarevalidator.h"

PrefixAwareValidator::PrefixAwareValidator(const QString &prefix, QValidator *wrappedValidator, QObject *parent)
    : QValidator(parent), prefix_(prefix), wrappedValidator_(wrappedValidator)
{
}

QValidator::State PrefixAwareValidator::validate(QString &input, int &pos) const
{
    if (input.startsWith(prefix_))
    {
        QString userPart = input.mid(prefix_.length());
        int userPos = (pos > prefix_.length()) ? pos - prefix_.length() : 0;

        return wrappedValidator_->validate(userPart, userPos);
    }

    return QValidator::Invalid;
}
