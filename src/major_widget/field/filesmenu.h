#pragma once

#include "filesmenuitem.h"
#include <QVector>
#include <QWidget>

namespace Ui
{
class FilesMenu;
}

class FilesMenu : public QWidget
{
    Q_OBJECT

  public:
    explicit FilesMenu(QWidget *parent = nullptr);
    ~FilesMenu();
    void addNewFile(const QString &);
    void removeFile(FilesMenuItem *file);
    void clear();
    FilesMenuItem *getMenuItem(const QString &);

    QVector<FilesMenuItem *> getSelectedFiles();

    QVector<FilesMenuItem *> getProcessedFiles();

  signals:
    void filesAdded();
    void fileRemoved(FilesMenuItem *file);
    void selectedChanged();

  private:
    Ui::FilesMenu *ui;
    QVector<FilesMenuItem *> files;

  private:
    bool isExistFile(const QString &);
};
