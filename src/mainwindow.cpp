#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qstackedwidget.h>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumSize(1280, 720);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    auto *majorWidget = new MajorWidget(this);
    stackedWidget->addWidget(majorWidget);

    setWindowIcon(QIcon("://icons/mainIcon/LetiIcon.png"));


}

MainWindow::~MainWindow() { delete ui; }
