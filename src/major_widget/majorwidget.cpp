#include "majorwidget.h"
#include "examplewidget.h"
#include "phaseportrait.h"
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

    registerWidgets();
    addMenu();
    connects();

    m_frontend = QSharedPointer<FrontendBase>::create("127.0.0.1", 8080);
    //m_frontend->start();
    QObject::connect(m_frontend.data(), &FrontendBase::dataReceived, [this](const QByteArray &data, const QHostAddress &from, quint16 port){
        qDebug() << "Got" << data.size() << "bytes";
        //m_frontend->sendMessage(data,from,port);
    });
    QObject::connect(m_frontend.data(), &FrontendBase::errorOccurred, [](const QString &e){
        qWarning() << e;
    });
    // // if (!m_frontend->start()) {
    // //     qFatal("Failed to start frontend");
    // // }
    // else qDebug() << "Front is listening";

}

MajorWidget::~MajorWidget() { delete ui; }

void MajorWidget::registerWidgets()
{
    FieldWidget::registerWidget<ExampleWidget>(SA::CustomWidgets::ExampleWidget);
    FieldWidget::registerWidget<SystemSettings>(SA::CustomWidgets::SystemSettings);
    FieldWidget::registerWidget<PhasePortrait>(SA::CustomWidgets::PhasePortrait);
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

    but = menuWid->addButton(SA::Sections::Graphics, "Графики", "Фазовый портрет");
    connectButtonAndBaseWidget(but, SA::CustomWidgets::PhasePortrait, mainField.get());
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
