#include "customwindows.h"

void errorWindow(QString msg, QWidget *wid)
{
    QMessageBox msgBox(wid);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(msg);
    msgBox.setWindowTitle("Ошибка");
    msgBox.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}
