#include "majorwidget.h"
#include "examplewidget.h"
#include "phaseportrait2d.h"
#include "phaseportrait3d.h"
#include "systemsettings.h"
#include "ui_majorwidget.h"

MajorWidget::MajorWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MajorWidget)
{
    ui->setupUi(this);

    mainField = std::make_unique<FieldWidget>(this);
    menuWid = std::make_unique<MenuWidget>(this);

    // === Контейнер для prepareFieldWid ===
    QWidget *container1 = new QWidget();
    QVBoxLayout *layout1 = new QVBoxLayout(container1);
    layout1->addWidget(mainField.get());
    layout1->setContentsMargins(10, 18, 10, 18);
    layout1->setSpacing(0);

    // === Создаём стек и добавляем контейнеры ===
    mainStackedLayout = new QStackedLayout();
    mainStackedLayout->addWidget(container1);

    // === Вставляем в основной layout ===
    ui->verticalLayout->addWidget(menuWid.get());
    ui->verticalLayout->addLayout(mainStackedLayout);

    systemManager = SystemManager::instance();
    m_frontend = FrontendBase::instance();

    connect(systemManager, &SystemManager::sendSystemToBack_sig, m_frontend, &FrontendBase::sendMessage);

    registerWidgets();
    addMenu();
    connects();
}

MajorWidget::~MajorWidget() { delete ui; }

void MajorWidget::registerWidgets()
{
    FieldWidget::registerWidget<ExampleWidget>(SA::CustomWidgets::ExampleWidget);
    FieldWidget::registerWidget<SystemSettings>(SA::CustomWidgets::SystemSettings);
    FieldWidget::registerWidget<PhasePortrait3D>(SA::CustomWidgets::PhasePortrait3D);

    FieldWidget::registerWidget<PhasePortrait2D>(SA::CustomWidgets::PhasePortrait2D);
}

void MajorWidget::addMenu()
{
    menuWid->addSection(SA::Sections::Project, "Проект");
    menuWid->addSection(SA::Sections::Graphics, "Графики");
    auto but = menuWid->addButton(SA::Sections::Graphics, "Настройки", "Example");
    connectButtonAndBaseWidget(but, SA::CustomWidgets::ExampleWidget, mainField.get());
    but = menuWid->addButton(SA::Sections::Graphics, "Настройки", "Схема и\nпараметры",
                             "://icons/buttonIcons/system.svg");
    connectButtonAndBaseWidget(but, SA::CustomWidgets::SystemSettings, mainField.get());

    but = menuWid->addButton(SA::Sections::Graphics, "Графики", "Фазовый портрет 3D");
    connectButtonAndBaseWidget(but, SA::CustomWidgets::PhasePortrait3D, mainField.get());

    but = menuWid->addButton(SA::Sections::Graphics, "Графики", "Фазовый портрет 2D");
    connectButtonAndBaseWidget(but, SA::CustomWidgets::PhasePortrait2D, mainField.get());
}

void MajorWidget::connectButtonAndBaseWidget(const std::shared_ptr<QToolButton> but, SA::CustomWidgets widget,
                                             FieldWidget *field)
{
    connect(but.get(), &QPushButton::clicked, field,
            [this, widget, field]()
            {
                std::shared_ptr<TemplateFieldWidget> wid = field->createWidget(widget);
                if (widget == SA::CustomWidgets::SystemSettings)
                {
                    systemManager->connectWidgets(wid->baseWidget());
                }
            });
}

void MajorWidget::connects()
{
    connect(menuWid.get(), &MenuWidget::createWidget, mainField.get(), &FieldWidget::createWidget);
}
