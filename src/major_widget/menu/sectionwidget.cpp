#include "sectionwidget.h"
#include "stylesheetprocessor.h"
#include "ui_sectionwidget.h"

SectionWidget::SectionWidget(QWidget *parent) : QWidget(parent), ui(new Ui::SectionWidget)
{
    ui->setupUi(this);

    StyleSheetProcessor::applyStyle(":/styles/specific/SectionWidget.qss", this);
    this->layout()->setAlignment(Qt::AlignLeft);
    this->setContentsMargins(0, 0, 0, 0);
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->setSpacing(0);
}

SectionWidget::~SectionWidget() { delete ui; }

std::shared_ptr<QToolButton> SectionWidget::addButton(const QString &subsec, const QString &title, const QString &icon)
{
    if (!sections.contains(subsec))
    {
        QLabel *label = new QLabel(subsec, this);
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

        QVBoxLayout *subsecLayout = new QVBoxLayout();
        subsecLayout->setContentsMargins(0, 0, 20, 0);
        subsecLayout->setSpacing(0);
        subsecLayout->setAlignment(Qt::AlignTop);

        subsecLayout->addWidget(label);
        this->layout()->addItem(subsecLayout);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->setAlignment(Qt::AlignLeft);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(0);
        subsecLayout->addItem(buttonLayout);

        sections[subsec] = {label, buttonLayout};
    }

    auto buttonLayout = sections[subsec].buttonLayout;

    auto button = std::make_shared<QToolButton>(this);
    button->setProperty("iconPath", icon);
    button->setProperty("hasIcon", true);
    button->setProperty("defaultColor", QColor(38, 45, 56));
    button->setProperty("hoverColor", QColor(64, 116, 212));
    button->setProperty("pressedColor", QColor(54, 98, 179));
    button->setIcon(recolorSvg(icon, QColor(38, 45, 56, 255)));
    button->setIconSize(QSize(32, 32));
    button->setText(title);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    button->installEventFilter(this);

    activeButtons.append(button);
    buttonLayout->addWidget(button.get());

    return button;
}

std::shared_ptr<QToolButton> SectionWidget::addButton(const QString &subsec, const QString &title)
{
    if (!sections.contains(subsec))
    {
        QLabel *label = new QLabel(subsec, this);
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

        QVBoxLayout *subsecLayout = new QVBoxLayout();
        subsecLayout->setContentsMargins(0, 0, 20, 0);
        subsecLayout->setSpacing(0);
        subsecLayout->setAlignment(Qt::AlignTop);

        subsecLayout->addWidget(label);

        this->layout()->addItem(subsecLayout);

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->setAlignment(Qt::AlignLeft);
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        buttonLayout->setSpacing(0);
        subsecLayout->addItem(buttonLayout);

        sections[subsec] = {label, buttonLayout};
    }

    auto buttonLayout = sections[subsec].buttonLayout;

    auto button = std::make_shared<QToolButton>(this);
    button->setText(title);
    button->setProperty("hasIcon", false);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    activeButtons.append(button);
    buttonLayout->addWidget(button.get());

    return button;
}

QIcon SectionWidget::recolorSvg(const QString &filePath, const QColor &color)
{
    QSvgRenderer renderer(filePath);
    QPixmap pixmap(renderer.defaultSize());
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    renderer.render(&painter);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    painter.end();

    return QIcon(pixmap);
}

bool SectionWidget::eventFilter(QObject *watched, QEvent *event)
{
    auto button = qobject_cast<QToolButton *>(watched);
    if (!button || !button->property("iconPath").isValid())
    {
        return QWidget::eventFilter(watched, event);
    }

    if (!button->isEnabled())
    {
        return QWidget::eventFilter(watched, event);
    }

    QString iconPath = button->property("iconPath").toString();
    QColor defaultColor = button->property("defaultColor").value<QColor>();
    QColor hoverColor = button->property("hoverColor").value<QColor>();
    QColor pressedColor = button->property("pressedColor").value<QColor>();

    if (event->type() == QEvent::HoverEnter)
    {
        button->setIcon(recolorSvg(iconPath, hoverColor));
    }
    else if (event->type() == QEvent::HoverLeave)
    {
        button->setIcon(recolorSvg(iconPath, defaultColor));
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        button->setIcon(recolorSvg(iconPath, pressedColor));
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        button->setIcon(recolorSvg(iconPath, hoverColor));
    }

    return QWidget::eventFilter(watched, event);
}
