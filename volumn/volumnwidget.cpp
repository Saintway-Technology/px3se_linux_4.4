#include "volumnwidget.h"
#include "ui_volumnwidget.h"
#include <QDebug>

VolumnWidget::VolumnWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VolumnWidget)
{
    ui->setupUi(this);


}

VolumnWidget::~VolumnWidget()
{
    delete ui;
}


void VolumnWidget::onVolumnChange(int volumn){
    qDebug()<< "volumn:" << volumn;
    saveVolumn(volumn);
}

void VolumnWidget::on_m_VolumnDownPushButton_clicked()
{
   ui->m_VolumnHorizontalSlider->setValue(ui->m_VolumnHorizontalSlider->value()-ui->m_VolumnHorizontalSlider->pageStep());
}

void VolumnWidget::on_m_VolumnUpPushButton_clicked()
{
    ui->m_VolumnHorizontalSlider->setValue(ui->m_VolumnHorizontalSlider->value()+ui->m_VolumnHorizontalSlider->pageStep());
}

void VolumnWidget::saveVolumn(int volumn){

    QDir  settingsDir("/data/");

    if(settingsDir.exists()){
        volumnFile = new QFile("/data/volumn");
    }else{
        volumnFile = new QFile("/etc/volumn");
    }

    if (volumnFile->open(QFile::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(volumnFile);
        out <<volumn;
     }
}
