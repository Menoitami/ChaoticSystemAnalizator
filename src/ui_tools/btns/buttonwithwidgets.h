#pragma once
#include <QLayout>
#include <QList>
#include <QMouseEvent>
#include <QWidget>

class ButtonWithWidgets : public QWidget
{
    Q_OBJECT

  public:
    explicit ButtonWithWidgets(QWidget *parent = nullptr);
    virtual ~ButtonWithWidgets();

    // Добавить виджет внутрь кнопки
    void addWidget(QWidget *widget);

    // Получить список виджетов (только для чтения)
    const QList<QWidget *> &widgets() const;

    // Установить layout (если нужен не QHBoxLayout)
    void setLayout(QLayout *layout);

    // Установить текст (если есть QLabel — обновит его)
    void setText(const QString &text);

  signals:
    void clicked(); // как у QPushButton

  protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

  private:
    void init(); // внутренняя инициализация

    bool m_pressed = false;
    bool m_hovered = false;

    QLayout *m_layout = nullptr;
    QList<QWidget *> m_widgets; // хранилище виджетов — доступ только для чтения

    void addLayout();

    // Опционально: если хотим быстро менять текст — можно хранить QLabel
    class QLabel *m_textLabel = nullptr;

  private slots:
};
