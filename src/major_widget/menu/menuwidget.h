#pragma once

#include "constants.hpp"
#include "sectionwidget.h"
#include <QDebug>
#include <QMap>
#include <QPushButton>
#include <QTabBar>
#include <QTabWidget>
#include <QWidget>
#include <memory>

namespace Ui
{
class MenuWidget;
}

class MenuWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit MenuWidget(QWidget *parent = nullptr);
    ~MenuWidget();

    std::shared_ptr<SectionWidget> addSection( SA::Sections, QString);
    std::shared_ptr<QToolButton> addButton(const  SA::Sections &sec, const QString &subsec, const QString &title,
                                           const QString &icon = "");

    void setupTabs();
    void setTabEnable( SA::Sections, bool state);
    void setTabVisible( SA::Sections, bool state);
    int getCurrentTabIndex();

  signals:
    void createWidget( SA::CustomWidgets);
    void createFlightMode();
    void currentTabChanged(int index);

  private:
    Ui::MenuWidget *ui;

    std::unique_ptr<QTabWidget> tabWidget;

    QMap< SA::Sections, QString> titles;
    QMap< SA::Sections, std::shared_ptr<SectionWidget>> activeSections;

    QLabel *letiPickTopRight = nullptr;

    void updateLetiPickPosition();

  private slots:

  protected:
    void resizeEvent(QResizeEvent *event) override;
};
