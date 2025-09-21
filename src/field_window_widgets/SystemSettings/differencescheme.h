#pragma once

#include <QTextEdit>
#include <QWidget>

namespace Ui
{
class DifferenceScheme;
}

class DifferenceScheme : public QWidget
{
    Q_OBJECT

  public:
    explicit DifferenceScheme(QWidget *parent = nullptr);
    ~DifferenceScheme();
    QTextEdit *const getTextEdit() const;
    QString getScheme() const;

  private:
    Ui::DifferenceScheme *ui;
};
