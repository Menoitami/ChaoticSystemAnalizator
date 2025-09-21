#include "phaseportrait.h"
#include "ui_phaseportrait.h"

PhasePortrait::PhasePortrait(QWidget *parent)
    : BaseWidget(SA::CustomWidgets::PhasePortrait, parent), ui(new Ui::PhasePortrait)
{
    ui->setupUi(this);
}

PhasePortrait::~PhasePortrait() { delete ui; }

QStringList PhasePortrait::getTitle() const { return {"Графики", "Фазовый портрет"}; }
