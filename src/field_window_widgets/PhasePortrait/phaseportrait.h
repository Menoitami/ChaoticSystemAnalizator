#include "basewidget.h"
#include <QWidget>

namespace Ui
{
class PhasePortrait;
}

class PhasePortrait : public BaseWidget
{
    Q_OBJECT

  public:
    explicit PhasePortrait(QWidget *parent = nullptr);
    ~PhasePortrait();

    QStringList getTitle() const override;

  private:
    Ui::PhasePortrait *ui;
};
