#pragma once

#include <QPushButton>
#include <QWidget>
#include <memory>

#include "fieldwidget.h"
#include "menuwidget.h"
#include "systemmanager.h"
#include <QStackedLayout>
#include <FrontendBase.hpp>

namespace Ui
{
class MajorWidget;
}

class MajorWidget : public QWidget
{
    Q_OBJECT

  public:
    explicit MajorWidget(QWidget *parent = nullptr);
    ~MajorWidget();

  signals:
    void setTitle(const QString &);

  private:
    Ui::MajorWidget *ui;

    std::unique_ptr<FieldWidget> mainField;
    std::unique_ptr<MenuWidget> menuWid;

    std::shared_ptr<QToolButton> createBtn;
    std::shared_ptr<QToolButton> openBtn;
    std::shared_ptr<QToolButton> saveBtn;
    std::shared_ptr<QToolButton> closeBtn;
    QStackedLayout *mainStackedLayout;

    SystemManager *systemManager;
    QSharedPointer<FrontendBase> m_frontend;

  private:
    void registerWidgets();
    void addMenu();
    void connectButtonAndBaseWidget(const std::shared_ptr<QToolButton> but, const SA::CustomWidgets widget,
                                    FieldWidget *field);
    void connects();

  private slots:

    friend class MainWindow;
};
