#include "stylesheetprocessor.h"

#include "colorpalette.h"

#include <QDebug>
#include <QRegularExpression>

StyleSheetProcessor::StyleSheetProcessor() {}

void StyleSheetProcessor::loadStyles(QApplication &app)
{
    QDir stylesheetsDir(":/styles");
    QFileInfoList entries = stylesheetsDir.entryInfoList();

    QString completeStylesheet;

    foreach (const QFileInfo &fileInfo, entries)
    {
        QFile file(":/styles/" + fileInfo.fileName());

        qDebug() << __FUNCTION__ << fileInfo.fileName();

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        QTextStream in(&file);
        QString content = in.readAll();
        file.close();

        completeStylesheet += processStyleSheet(content);
    }

    app.setStyleSheet(completeStylesheet);
}

QString StyleSheetProcessor::processStyleSheet(const QString &source)
{
    QString processed = source;
    checkHardcodedColors(processed);

    const QHash<QString, QString> &variables = ColorPalette::palette;

    // регулярное выражение для поиска {{key}}
    static const QRegularExpression varRegex("\\{\\{([^\\}]+)\\}\\}");

    QStringList result;
    int pos = 0;
    QRegularExpressionMatchIterator i = varRegex.globalMatch(processed);

    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();

        if (match.hasMatch())
        {
            int start = match.capturedStart();
            int end = match.capturedEnd();
            QStringView key = match.capturedView(1);

            result.append(processed.mid(pos, start - pos));

            if (variables.contains(key.toString()))
            {
                result.append(variables[key.toString()]);
            }
            else
            {
                qWarning() << key << " not found in palette";
                result.append(match.captured());
            }

            pos = end;
        }
    }

    result.append(processed.mid(pos));

    return result.join(QString());
}
void StyleSheetProcessor::applyStyle(const QString &qrcStyleResource, QWidget *wid)
{
    QFile file(qrcStyleResource);
    file.open(QFile::ReadOnly);

    qDebug() << __FUNCTION__ << file.fileName();

    wid->setStyleSheet(wid->styleSheet() + "" + processStyleSheet(file.readAll()));
}

void StyleSheetProcessor::checkHardcodedColors(QString processed)
{
    static const QRegularExpression hexColorRegex(
        "#(?:[0-9a-fA-F]{3}|[0-9a-fA-F]{4}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})\\b");
    static const QRegularExpression rgbRegex("\\b(?:rgb|rgba)$(?:[^$]+)$");
    static const QRegularExpression colorKeywordRegex(
        "\\b(?:red|green|blue|black|white|gray|cyan|magenta|yellow|orange|purple|brown|pink|lime|teal|maroon|navy|"
        "olive|silver|gold|indigo|violet|aqua|fuchsia)\\b",
        QRegularExpression::CaseInsensitiveOption);

    bool hasHexColors = hexColorRegex.match(processed).hasMatch();
    bool hasRgbColors = rgbRegex.match(processed).hasMatch();
    bool hasKeywords = colorKeywordRegex.match(processed).hasMatch();

    if (hasHexColors || hasRgbColors || hasKeywords)
    {
        qCritical() << "ERROR: Hardcoded colors detected in stylesheet!";
        if (hasHexColors)
            qWarning() << "- Hex colors found";
        if (hasRgbColors)
            qWarning() << "- RGB/RGBA functions found";
        if (hasKeywords)
            qWarning() << "- Color keywords found";
    }
}
