#include "animatediconbuttons.h"

AnimatedIconButton::AnimatedIconButton(QWidget *parent) : QPushButton(parent) { installEventFilter(this); }

void AnimatedIconButton::setSvgIconPath(QString path)
{
    svgPath = path;
    updateIcon(defaultColor);
    getColorPaths();
}

bool AnimatedIconButton::eventFilter(QObject *watched, QEvent *event)
{
    auto button = qobject_cast<AnimatedIconButton *>(watched);
    if (!button || svgPath == "")
    {
        return QWidget::eventFilter(watched, event);
    }

    if (event->type() == QEvent::HoverEnter)
    {
        updateIcon(hoverColor);
    }
    else if (event->type() == QEvent::HoverLeave)
    {
        updateIcon(defaultColor);
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        updateIcon(pressedColor);
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        updateIcon(hoverColor);
    }
    else if (event->type() == QEvent::EnabledChange)
    {
        updateIcon(defaultColor);
    }

    return QWidget::eventFilter(watched, event);
}

const QMap<QString, QString> styleVariables = {
    // primary button
    {"button/primary/bg/default", "#4882EE"},
    {"button/primary/bg/hover", "#4074D4"},
    {"button/primary/bg/pressed", "#3662B3"},
    {"button/primary/bg/disabled", "#ECEDF2"},

    {"button/primary/label/default", "#FFFFFF"},
    {"button/primary/label/disabled", "#CDD2DB"},

    // secondary button
    {"button/secondary/bg/default", "#E9F1FF"},
    {"button/secondary/bg/hover", "#CDDEFF"},
    {"button/secondary/bg/pressed", "#B3CCFF"},
    {"button/secondary/bg/disabled", "#ECEDF2"},

    {"button/secondary/label/default", "#4882EE"},
    {"button/secondary/label/hover", "#4074D4"},
    {"button/secondary/label/pressed", "#3662B3"},
    {"button/secondary/label/disabled", "#CDD2DB"},

    {"button/arrows/label/default", "#6A7384"},
    {"button/arrows/label/hover", "#50607E"},
    {"button/arrows/label/pressed", "#3662B3"},
    {"button/arrows/label/disabled", "#CDD2DB"},

    // outline button
    {"button/outline/bg/default", "transparent"},

    {"button/outline/label/default", "#4882EE"},
    {"button/outline/label/hover", "#4074D4"},
    {"button/outline/label/pressed", "#3662B3"},
    {"button/outline/label/disabled", "#CDD2DB"},

    // templateFieldWidget close button
    {"button/CloseButton/label/default", "#6A7384"},
    {"button/CloseButton/label/hover", "#B4BAC4"},

    {"button/EditButton/label/default", "#6A7384"},
    {"button/EditButton/label/hover", "#4074D4"},
    {"button/EditButton/label/pressed", "#3662B3"},

    // line edit
    {"input/text/error", "#FF4D4F"},
    {"input/border/pressed", "#4882EE"}

};

void AnimatedIconButton::getColorPaths()
{
    QString btnVariant = property("Variant").toString();

    QString defaultKey = "button/" + btnVariant + "/label/default";
    QString hoverKey = "button/" + btnVariant + "/label/hover";
    QString pressedKey = "button/" + btnVariant + "/label/pressed";
    QString disabledKey = "button/" + btnVariant + "/label/disabled";

    if (styleVariables.contains(defaultKey))
        defaultColor = styleVariables[defaultKey];
    else
        qWarning() << defaultKey + " not passed";

    if (styleVariables.contains(hoverKey))
        hoverColor = styleVariables[hoverKey];
    else
        hoverColor = defaultColor;

    if (styleVariables.contains(pressedKey))
        pressedColor = styleVariables[pressedKey];
    else
        pressedColor = defaultColor;

    if (styleVariables.contains(disabledKey))
        disabledColor = styleVariables[disabledKey];
    else
        disabledColor = defaultColor;

    updateIcon(defaultColor);
}

void AnimatedIconButton::updateIcon(QString colorS)
{
    QColor color(colorS);
    if (!isEnabled())
        color = QColor(disabledColor);

    QIcon icon = createColorableIcon(svgPath, color);
    setIcon(icon);
}

QIcon AnimatedIconButton::createColorableIcon(const QString &filePath, const QColor &color)
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
AnimatedIconButton::~AnimatedIconButton() {}
