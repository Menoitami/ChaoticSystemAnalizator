#include "filesmenu.h"
#include "stylesheetprocessor.h"
#include "ui_filesmenu.h"

FilesMenu::FilesMenu(QWidget *parent) : QWidget(parent), ui(new Ui::FilesMenu)
{
    setAttribute(Qt::WA_StyledBackground);
    ui->setupUi(this);

    ui->addBtn->setSvgIconPath(":Plus_24");
    update();
    StyleSheetProcessor::applyStyle(":/styles/specific/FilesMenu.qss", this);
    // connect(ui->addBtn, &QPushButton::clicked, this, &FilesMenu::addNewFile);
    connect(ui->addBtn, &QPushButton::clicked, this, &FilesMenu::filesAdded);
}

FilesMenu::~FilesMenu() { delete ui; }

void FilesMenu::addNewFile(const QString &filename)
{
    if (isExistFile(filename))
        return;
    auto newOne = new FilesMenuItem(filename, this);
    ui->filesLayout->addWidget(newOne);
    files.append(newOne);
    connect(newOne, &FilesMenuItem::removeFile, this, &FilesMenu::fileRemoved);
    connect(newOne, &FilesMenuItem::selectedChanged, this, &FilesMenu::selectedChanged);
}

bool FilesMenu::isExistFile(const QString &filename)
{
    for (auto file : std::as_const(files))
    {
        if (file->fileName() == filename)
            return true;
    }
    return false;
}

void FilesMenu::removeFile(FilesMenuItem *file)
{
    ui->filesLayout->removeWidget(file);
    files.removeAll(file);
    file->deleteLater();
}

void FilesMenu::clear()
{
    for (int i = files.size() - 1; i >= 0; --i)
    {
        removeFile(files[i]);
    }
}

FilesMenuItem *FilesMenu::getMenuItem(const QString &filename)
{
    for (auto file : std::as_const(files))
    {
        if (file->fileName() == filename)
            return file;
    }
    return nullptr;
}

QVector<FilesMenuItem *> FilesMenu::getSelectedFiles()
{
    QVector<FilesMenuItem *> selectedFiles;
    for (auto file : std::as_const(files))
    {
        if (file->selected())
            selectedFiles.push_back(file);
    }
    return selectedFiles;
}

QVector<FilesMenuItem *> FilesMenu::getProcessedFiles()
{
    QVector<FilesMenuItem *> processedFiles;
    for (auto file : std::as_const(files))
    {
        if (file->getState() == FilesMenuItem::State::Processed)
            processedFiles.push_back(file);
    }
    return processedFiles;
}
