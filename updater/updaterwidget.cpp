#include "updaterwidget.h"
#include "ui_updaterwidget.h"
#include <QFileDialog>
#include <QDebug>

UpdaterWidget::UpdaterWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdaterWidget)
{
    ui->setupUi(this);
}

UpdaterWidget::~UpdaterWidget()
{
    delete ui;
}

void UpdaterWidget::on_m_selectFilePushButton_clicked()
{
    QFileDialog *dialog = new QFileDialog(this,"selete file");
    if(dialog->exec()==QFileDialog::Accepted)
    {
        QStringList files = dialog->selectedFiles();
        if(files.isEmpty())
            return;
        for(int i=0;i<files.count();i++)
        {
            file= new QFileInfo(files[i]);
            ui->m_fileLineEdit->setText(file->absoluteFilePath());
        }
    }
}

void UpdaterWidget::on_m_updatePushButton_clicked()
{
    if(file&&file->exists()){
        qDebug()<< file->absoluteFilePath();
    }
}
