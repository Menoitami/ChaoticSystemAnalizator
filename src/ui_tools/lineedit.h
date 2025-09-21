#pragma once

#include <QLabel>
#include <QLineEdit>
#include <memory>

namespace Ui
{
class LineEdit;
}

class LineEdit : public QWidget
{
    Q_OBJECT

  public:
    explicit LineEdit(QWidget *parent = nullptr);

    void setRestrictInputValidator(std::shared_ptr<QValidator> restrictInputValidator, bool showErrorState = false);
    void addErrorValidator(QString error, std::shared_ptr<QValidator> showErrorValidator);

    using CrossValidator = std::function<bool()>;
    void addCrossValidator(QString error, CrossValidator validator);

    void setText(const QString &text);
    void setLabel(const QString &labelText);
    void setPlaceholder(const QString &text);
    void setMinFieldHeight(int size);

    QString text() const;
    void hideError();
    void setConstantPrefix(QString prefixStr);

    ~LineEdit();
    bool valid();

  signals:
    void textChanged();

  protected:
    void focusInEvent(QFocusEvent *event) override;

  private:
    Ui::LineEdit *ui;
    std::optional<QString> prefix;
    bool showErrorStateInIntermidiate_ = false;

    std::shared_ptr<QValidator> restrictInputValidator_;
    QVector<QPair<QString, std::shared_ptr<QValidator>>> errorsAndValidators_;
    QVector<QPair<QString, CrossValidator>> crossValidators_;

  private slots:
    void validate(const QString &s);

  private:
    void showError(const QString &error);
    void setErrorState(bool err = true);
};
