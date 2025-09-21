#pragma once

#include "majorwidget.h"
#include <QDebug>
#include <QMainWindow>
#include <qstackedwidget.h>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
};
