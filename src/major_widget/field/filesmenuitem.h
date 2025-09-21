#pragma once

#include <QFileInfo>
#include <QWidget>

namespace Ui
{
class FilesMenuItem;
}

class FilesMenuItem : public QWidget
{
    Q_OBJECT

  public:
    enum State
    {
        NotProcessed,
        InProcess,
        Processed,
    };

    enum FileType
    {
        Recogn,
        Default
    };
    explicit FilesMenuItem(const QString &path, QWidget *parent = nullptr, FileType ft = Default,
                           State state = NotProcessed);
    ~FilesMenuItem();

    FileType getType();
    void setType(FileType type);
    State getState();
    void setState(State state);
    const QString fileName() const;

    bool selected();

  protected:
    void mousePressEvent(QMouseEvent *event) override;

  private:
    Ui::FilesMenuItem *ui;
    State state_;
    FileType ft_;
    QString fileName_;
    bool selected_ = false;

  private:
    void changeSelectedStyle();

  signals:
    void removeFile(FilesMenuItem *item);
    void selectedChanged(bool isSelected);
};
