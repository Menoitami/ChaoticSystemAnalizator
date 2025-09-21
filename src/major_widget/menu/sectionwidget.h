#pragma once

#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QMap>
#include <QPainter>
#include <QSvgRenderer>
#include <QToolButton>
#include <QWidget>
#include <memory>

namespace Ui
{
class SectionWidget;
}

class SectionWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit SectionWidget(QWidget *parent = nullptr);
    ~SectionWidget();

    std::shared_ptr<QToolButton> addButton(const QString &subsec, const QString &title, const QString &icon);
    std::shared_ptr<QToolButton> addButton(const QString &subsec, const QString &title);

  private:
    Ui::SectionWidget *ui;

    QIcon recolorSvg(const QString &filePath, const QColor &color);

    struct SectionLayout
    {
        QLabel *label;
        QHBoxLayout *buttonLayout;
    };

    QMap<QString, SectionLayout> sections;
    QList<std::shared_ptr<QToolButton>> activeButtons;

  protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
};
