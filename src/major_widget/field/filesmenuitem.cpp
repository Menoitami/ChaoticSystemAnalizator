#include "filesmenuitem.h"
#include "colorpalette.h"
#include "ui_filesmenuitem.h"
#include <QMouseEvent>

FilesMenuItem::FilesMenuItem(const QString &name, QWidget *parent, FileType ft, State state)
    : QWidget(parent), ui(new Ui::FilesMenuItem), ft_(ft)
{
    fileName_ = name;
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground);
    ui->name->setText(fileName());
    ui->removeBtn->setSvgIconPath(":/trash_20");
    ui->removeBtn->setIconSize(QSize{20, 20});
    ui->label->setFixedSize(24, 24);

    setState(state);

    connect(ui->removeBtn, &QPushButton::clicked, this, [this]() { emit removeFile(this); });
}

FilesMenuItem::~FilesMenuItem() { delete ui; }

FilesMenuItem::FileType FilesMenuItem::getType() { return ft_; }

void FilesMenuItem::setType(FilesMenuItem::FileType type)
{
    ft_ = type;
    setState(state_);
}

FilesMenuItem::State FilesMenuItem::getState() { return state_; }

void FilesMenuItem::changeSelectedStyle()
{
    QPalette palette = ui->name->palette();
    if (selected_)
    {
        setProperty("Selected", "True");
        palette.setColor(QPalette::WindowText, ColorPalette::palette["accent_060"]);
    }
    else
    {
        setProperty("Selected", "False");
        palette.setColor(QPalette::WindowText, ColorPalette::palette["neutral_090"]);
    }

    ui->name->setPalette(palette);
    style()->unpolish(this);
    style()->polish(this);
}

const QString FilesMenuItem::fileName() const { return fileName_; }

void FilesMenuItem::setState(State state)
{
    if (state == state_ && !ui->label->pixmap(Qt::ReturnByValueConstant::ReturnByValue).isNull())
        return;

    state_ = state;

    QString pixmap;

    switch (state_)
    {
    case NotProcessed:
        pixmap = ":/NotProcessed";
        break;

    case InProcess:
        pixmap = ":/InProcess";
        break;

    case Processed:
        pixmap = ":/Processed";
        break;
    }
    if (ft_ == Recogn)
        pixmap += "_rec";

    ui->label->setPixmap(pixmap);
}

bool FilesMenuItem::selected() { return selected_; }

void FilesMenuItem::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    selected_ = !selected_;
    emit selectedChanged(selected_);
    changeSelectedStyle();
}
