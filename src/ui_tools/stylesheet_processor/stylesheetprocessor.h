#pragma once

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QString>
#include <QTextStream>

class StyleSheetProcessor
{

  public:
    StyleSheetProcessor();

  public:
    /*
     * обрабатывает содержимое первого уровня папки styles, заменяя включения
     * вида {{some.name}} на значения palette см colorpalette.h/cpp с
     * помощью processStyleSheet
     *
     * конкатенирует все файлы стилей и подключает их к проекту
     * */
    static void loadStyles(QApplication &app);


    static QString processStyleSheet(const QString &source);

    // применяет стиль из файла только к передаваемом виджету, использовать для assets/styles/special, проверяет на наличие hard-coded цветов
    static void applyStyle(const QString &qrcStyleResource, QWidget *wid);

  private:
    static void checkHardcodedColors(QString processed);
};
