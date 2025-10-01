#include "schemechecker.h"
#include "config.h" // для PYTHON_SCHEME_SCRIPT
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

void SchemeChecker::check(const SystemData &data)
{
    if (data.scheme.trimmed().isEmpty())
    {
        emit checked(false, "Scheme cannot be empty");
    }
    if (data.h <= 0)
    {
        emit checked(false, "Integration step h must be positive");
    }
    if (data.inits.empty())
    {
        emit checked(false, "Initial conditions are not set");
    }
    if (data.params.empty())
    {
        emit checked(false, "Parameters a[] are not set");
    }

    // Debug output
    qDebug() << "startPos:" << data.inits;
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
    for (size_t i = 0; i < data.inits.size(); ++i)
    {
        if (i > 0)
            xStr += ",";
        xStr += QString::number(data.inits[i], 'g', 15);
    }

    QString hStr = QString::number(data.h, 'g', 15);

    // --- 3. Create temporary scheme file ---
    QTemporaryFile schemeFile(QDir::tempPath() + "/scheme_XXXXXX.txt");
    if (!schemeFile.open())
    {
        emit checked(false, "Failed to create temporary scheme file");
    }

    QTextStream out(&schemeFile);
    out << data.scheme.trimmed();
    schemeFile.close();

    // --- 4. Script path ---
    QString scriptPath = PYTHON_SCHEME_SCRIPT;
    if (!QFile::exists(scriptPath))
    {
        emit checked(false, "Validation script not found: " + scriptPath);
    }

    // --- 5. Launch Python script ---
    QProcess process;
    QStringList args;
    args << scriptPath << schemeFile.fileName() << aStr << hStr << xStr;

    qDebug() << "Launching command:" << "python" << args;

    process.start("py", args);

    if (!process.waitForStarted(5000))
    {
        emit checked(false, "Failed to start Python. Ensure it is installed and available in PATH.");
    }

    if (!process.waitForFinished(15000)) // Max 15 seconds
    {
        process.kill();
        process.waitForFinished(1000);
        emit checked(false, "Script execution timed out (possibly infinite loop).");
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
            emit checked(true, "Validation successful");
        }
        else
        {
            emit checked(false, "Script exited with code 0 but did not confirm success");
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
        emit checked(false, errorMsg);
    }
}

void SchemeChecker::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)

    QByteArray stdoutData = m_process->readAllStandardOutput();
    QByteArray stderrData = m_process->readAllStandardError();
    QString stdoutStr = QString::fromUtf8(stdoutData).trimmed();
    QString stderrStr = QString::fromUtf8(stderrData).trimmed();

    qDebug() << "Python stdout (thread):" << stdoutStr;
    qDebug() << "Python stderr (thread):" << stderrStr;

    bool success = false;
    QString message;

    if (exitCode == 0 && (stdoutStr.contains("CHECK SUCCESSFUL", Qt::CaseInsensitive) ||
                          stdoutStr.contains("SUCCESS", Qt::CaseInsensitive) ||
                          stdoutStr.contains("completed successfully", Qt::CaseInsensitive)))
    {
        success = true;
        message = "Validation successful";
    }
    else
    {
        message = stderrStr.isEmpty() ? QString("Script failed with exit code %1").arg(exitCode)
                                      : "Script execution failed:\n" + stderrStr;
    }

    emit checked(success, message);

    // Очистка
    delete m_process;
    m_process = nullptr;
    QFile::remove(m_tempFilePath);
}

void SchemeChecker::onProcessError(QProcess::ProcessError error)
{
    Q_UNUSED(error)
    emit checked(false, "Process error occurred during validation.");
    if (m_process)
    {
        delete m_process;
        m_process = nullptr;
    }
    QFile::remove(m_tempFilePath);
}
