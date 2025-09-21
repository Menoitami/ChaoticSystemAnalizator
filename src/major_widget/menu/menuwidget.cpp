#include "menuwidget.h"
#include "stylesheetprocessor.h"
#include "ui_menuwidget.h"

MenuWidget::MenuWidget(QWidget *parent) : QWidget(parent), ui(new Ui::MenuWidget)
{
    ui->setupUi(this);
    this->raise();

    letiPickTopRight = new QLabel("", this);
    letiPickTopRight->setFixedSize(32, 32);
    letiPickTopRight->setScaledContents(true);
    letiPickTopRight->setAlignment(Qt::AlignCenter);
    letiPickTopRight->setPixmap(QPixmap("://icons/mainIcon/LetiIcon.png"));
    updateLetiPickPosition();

    tabWidget = std::make_unique<QTabWidget>(this);
    tabWidget->setTabPosition(QTabWidget::North);
    tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    setAttribute(Qt::WA_StyledBackground);

    this->layout()->addWidget(tabWidget.get());

    StyleSheetProcessor::applyStyle(":/styles/specific/MenuWidget.qss", this);

    connect(tabWidget.get(), &QTabWidget::currentChanged, this, &MenuWidget::currentTabChanged);
}

MenuWidget::~MenuWidget() { delete ui; }

std::shared_ptr<SectionWidget> MenuWidget::addSection( SA::Sections name, QString title)
{
    if (!activeSections.contains(name))
    {
        auto section = std::make_shared<SectionWidget>(this);
        activeSections[name] = section;

        tabWidget->addTab(section.get(), title);
        tabWidget->tabBar()->setExpanding(true);
        tabWidget->tabBar()->setStyleSheet("font-size:16px;font-family:Inter;");

        return section;
    }
    else
    {
        qCritical() << __FUNCTION__ << "activeSections already contains Sections name";
    }

    return nullptr;
}

std::shared_ptr<QToolButton> MenuWidget::addButton(const  SA::Sections &sec, const QString &subsec,
                                                   const QString &title, const QString &icon)
{

    if (activeSections.contains(sec))
    {
        std::shared_ptr<QToolButton> button;
        if (icon != "")
            button = activeSections[sec]->addButton(subsec, title, icon);
        else
            button = activeSections[sec]->addButton(subsec, title);
        return button;
    }
    else
    {
        qCritical() << __FUNCTION__ << "activeSections doesnt contain Sections name";
    }

    return nullptr;
}

void MenuWidget::setTabEnable( SA::Sections tab, bool state) { tabWidget->setTabEnabled(static_cast<int>(tab), state); }

void MenuWidget::setTabVisible( SA::Sections tab, bool state)
{
    tabWidget->setTabVisible(static_cast<int>(tab), state);
}

void MenuWidget::updateLetiPickPosition()
{
    if (!letiPickTopRight)
        return;

    int width = letiPickTopRight->width();
    int rightMargin = 8;
    int topMargin = 8;

    letiPickTopRight->move(this->width() - width - rightMargin, topMargin);
}

void MenuWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateLetiPickPosition();
}

int MenuWidget::getCurrentTabIndex() { return tabWidget->currentIndex(); }
