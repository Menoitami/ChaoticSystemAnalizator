#include "mainwindow.h"
#include "stylesheetprocessor.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QLocale::setDefault(QLocale::C);

    StyleSheetProcessor::loadStyles(a);

    auto w = std::make_shared<MainWindow>();
    w->show();

    return a.exec();
}
