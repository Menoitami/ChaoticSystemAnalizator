#include "FrontendBase.hpp"
#include <QCoreApplication>

void FrontendBase::cleanupFrontendBase() { delete FrontendBase::instance(); }

FrontendBase *FrontendBase::instance()
{
    static FrontendBase *self = nullptr;
    if (!self)
    {
        self = new FrontendBase(qApp);
        qAddPostRoutine(cleanupFrontendBase);
    }
    return self;
}

FrontendBase::FrontendBase(QObject *parent) : QObject(parent) {}

FrontendBase::~FrontendBase() {}

void FrontendBase::processMessage(MessageType type, QByteArray &data) {}
