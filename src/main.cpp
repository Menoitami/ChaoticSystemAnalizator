#include "mainwindow.h"
#include "stylesheetprocessor.h"
#include "vtkAutoInit.h"
#include <QApplication>

VTK_MODULE_INIT(vtkRenderingOpenGL2); // VTK was built with vtkRenderingOpenGL2
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingContextOpenGL2)

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    QLocale::setDefault(QLocale::C);

    StyleSheetProcessor::loadStyles(a);

    auto w = std::make_shared<MainWindow>();
    w->show();

    return a.exec();
}
