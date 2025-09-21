#pragma once

#include <QDebug>
#include <QEvent>
#include <QFile>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QStyleOptionButton>
#include <QSvgRenderer>
#include <QWidget>

class AnimatedIconButton : public QPushButton
{
    /*
     * кнопка, которая умеет менять цвет иконок в соответствии с цветом текста.
     * у кнопки должен быть динамический параметр Variable
     * путь передаваемый по setSvgIconPath должен указывать на qrc ресурс svg
     * иконки
     *
     * TODO: KBA-48
     * в .cpp styleVariables дожны быть оперделены константы:
     *  "button/" + Variable + "/label/default"
     *  "button/" + Variable + "/label/hover";
     *  "button/" + Variable + "/label/pressed";
     *  "button/" + Variable + "/label/disabled";
     *
     *  если не определена для default, то значение по умолчанию black
     *  если не определены hover, pressed или disabled, им присваивается default
     */

    Q_OBJECT
  public:
    AnimatedIconButton(QWidget *parent = nullptr);

    virtual ~AnimatedIconButton();

    void setSvgIconPath(QString path);

  private:
    QString svgPath = "";

    QString defaultColor = "#fff";
    QString hoverColor = "#fff";
    QString pressedColor = "#fff";
    QString disabledColor = "#fff";

  protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

  private:
    void getColorPaths();
    void updateIcon(QString colorS);

    QIcon createColorableIcon(const QString &filePath, const QColor &color);
};
