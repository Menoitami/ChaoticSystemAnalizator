#pragma once
#include <QPixmap>
#include <QPropertyAnimation>
#include <QWidget>

class RotatingImageWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)

  public:
    explicit RotatingImageWidget(QWidget *parent = nullptr);

    void setPixmap(const QPixmap &pixmap);

  public slots:
    void startRotation();
    void stopRotation();

    qreal rotation() const { return m_rotation; }
    void setRotation(qreal angle);

  protected:
    void paintEvent(QPaintEvent *event) override;

  private:
    QPixmap m_pixmap;
    qreal m_rotation = 0;
    QPropertyAnimation *m_animation = nullptr;
    bool m_showPixmap = false;
};
