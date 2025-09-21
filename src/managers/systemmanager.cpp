#include "SystemManager.h"
#include "QDebug"
#include "config.h"
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QTemporaryFile>

SystemManager *SystemManager::m_instance = nullptr;

SystemManager::SystemManager(QObject *parent) : QObject(parent) {}

SystemManager::~SystemManager() { qDebug() << __FUNCTION__; }

SystemManager *SystemManager::instance()
{
    if (!m_instance)
    {
        m_instance = new SystemManager(qApp);
    }
    return m_instance;
}

void SystemManager::setSystem(SA::SystemData &data)
{
    auto [validSystem, message] = checkScheme(data);
    qDebug() << validSystem << message;

    sysData = data;
}

std::pair<bool, QString> SystemManager::checkScheme(SA::SystemData &data)
{
    if (data.scheme.trimmed().isEmpty())
    {
        return {false, "Scheme cannot be empty"};
    }
    if (data.h <= 0)
    {
        return {false, "Integration step h must be positive"};
    }
    if (data.startPos.empty())
    {
        return {false, "Initial conditions are not set"};
    }
    if (data.params.empty())
    {
        return {false, "Parameters a[] are not set"};
    }

    // Debug output
    qDebug() << "startPos:" << data.startPos;
    qDebug() << "params:" << data.params;
    qDebug() << "h:" << data.h;

    // --- 2. Prepare data for script ---
    QString aStr;
    for (size_t i = 0; i < data.params.size(); ++i)
    {
        if (i > 0)
            aStr += ",";
        aStr += QString::number(data.params[i], 'g', 15);
    }

    QString xStr;
    for (size_t i = 0; i < data.startPos.size(); ++i)
    {
        if (i > 0)
            xStr += ",";
        xStr += QString::number(data.startPos[i], 'g', 15);
    }

    QString hStr = QString::number(data.h, 'g', 15);

    // --- 3. Create temporary scheme file ---
    QTemporaryFile schemeFile(QDir::tempPath() + "/scheme_XXXXXX.txt");
    if (!schemeFile.open())
    {
        return {false, "Failed to create temporary scheme file"};
    }

    QTextStream out(&schemeFile);
    out << data.scheme.trimmed();
    schemeFile.close();

    // --- 4. Script path ---
    QString scriptPath = PYTHON_SCHEME_SCRIPT;
    if (!QFile::exists(scriptPath))
    {
        return {false, "Validation script not found: " + scriptPath};
    }

    // --- 5. Launch Python script ---
    QProcess process;
    QStringList args;
    args << scriptPath << schemeFile.fileName() << aStr << hStr << xStr;

    qDebug() << "Launching command:" << "python" << args;

    process.start("python", args);

    if (!process.waitForStarted(5000))
    {
        return {false, "Failed to start Python. Ensure it is installed and available in PATH."};
    }

    if (!process.waitForFinished(15000)) // Max 15 seconds
    {
        process.kill();
        process.waitForFinished(1000);
        return {false, "Script execution timed out (possibly infinite loop)."};
    }

    // --- 6. Analyze result ---
    int exitCode = process.exitCode();
    QByteArray stdoutData = process.readAllStandardOutput();
    QByteArray stderrData = process.readAllStandardError();

    QString stdoutStr = QString::fromUtf8(stdoutData).trimmed();
    QString stderrStr = QString::fromUtf8(stderrData).trimmed();

    qDebug() << "Python stdout:" << stdoutStr;
    qDebug() << "Python stderr:" << stderrStr;

    // --- 7. Check success ---
    if (exitCode == 0)
    {
        // Success — if output contains key phrases
        if (stdoutStr.contains("CHECK SUCCESSFUL", Qt::CaseInsensitive) ||
            stdoutStr.contains("SUCCESS", Qt::CaseInsensitive) ||
            stdoutStr.contains("completed successfully", Qt::CaseInsensitive))
        {
            return {true, "Validation successful"};
        }
        else
        {
            return {false, "Script exited with code 0 but did not confirm success"};
        }
    }
    else
    {
        // Compilation/runtime error
        QString errorMsg = "Script execution failed:\n" + stderrStr;
        if (stderrStr.isEmpty())
        {
            errorMsg = "Script failed with exit code " + QString::number(exitCode);
        }
        return {false, errorMsg};
    }
}

void SystemManager::connectWidgets(std::shared_ptr<BaseWidget> wid)
{

    switch (wid->getName())
    {
    case (SA::CustomWidgets::SystemSettings):
        connectSystemSettings(std::dynamic_pointer_cast<SystemSettings>(wid));
        break;

    default:
        break;
    };
}

void SystemManager::connectSystemSettings(std::shared_ptr<SystemSettings> schemeWid_)
{
    if (schemeWid_ == nullptr)
        return;

    schemeWid = schemeWid_.get();

    connect(schemeWid, &SystemSettings::setSystem, this, &SystemManager::setSystem);
    connect(schemeWid, &QWidget::destroyed, this, [this]() { schemeWid = nullptr; });
}
