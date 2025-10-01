#pragma once
#include "ConnectionTypes.hpp"
#include <QObject>
#include <QProcess>
#include <QTemporaryFile>

class SchemeChecker : public QObject
{
    Q_OBJECT

  public:
    explicit SchemeChecker(QObject *parent = nullptr) : QObject(parent) {}

  public slots:
    void check(const SystemData &data);

  signals:
    void checked(bool success, QString message);

  private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);

  private:
    SystemData m_data;
    QString m_tempFilePath;
    QProcess *m_process = nullptr;
};
