#include "brightness.h"
#include "ui_brightness.h"
#include <QDebug>
#include <QFile>
#include <QDir>

Brightness::Brightness(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Brightness)
{
    ui->setupUi(this);    


    QFile brightnessPath("/sys/class/backlight/rk28_bl/brightness");
    brightnessPath.open(QFile::ReadOnly | QIODevice::Truncate);
    QByteArray readAll= brightnessPath.readAll();
    QString brightnessString(readAll);
    int brightnessInt= brightnessString.toInt();
    qDebug()<<"read brightness from sys:"<<brightnessInt;
    saveBrightness(brightnessInt);
    ui->m_BrightnessHorizontalSlider->setValue(brightnessInt);

}

Brightness::~Brightness()
{
    delete ui;
}

void Brightness::on_m_BrightnessHorizontalSlider_sliderMoved(int position)
{
    onBrightnessChange(position);
}

void Brightness::on_m_BrightnessHorizontalSlider_valueChanged(int value)
{
    onBrightnessChange(value);
}

void Brightness::on_m_BrightnessHorizontalSlider_actionTriggered(int action)
{

}

void Brightness::onBrightnessChange(int brightness){
    qDebug()<< "brightness:" << brightness;
#ifdef DEVICE_EVB
    QString cmd= QString("echo %1 > %2").arg(brightness).arg("/sys/devices/platform/backlight/backlight/backlight/brightness");
#else
    QString cmd= QString("echo %1 > %2").arg(brightness).arg("/sys/class/backlight/rk28_bl/brightness");
#endif
    system(cmd.toLatin1().data());
    saveBrightness(brightness);
}

void Brightness::saveBrightness(int brightness){
    QDir  settingsDir("/data/");

    if(settingsDir.exists()){
        brightnessFile = new QFile("/data/brightness");
    }else{
        brightnessFile = new QFile("/etc/brightness");
    }

    if (brightnessFile->open(QFile::WriteOnly | QIODevice::Truncate)) {
        QTextStream out(brightnessFile);
        out <<brightness;
     }
}

void Brightness::on_m_BrightnessDownPushButton_clicked()
{
    ui->m_BrightnessHorizontalSlider->setValue(ui->m_BrightnessHorizontalSlider->value()-ui->m_BrightnessHorizontalSlider->pageStep());
}

void Brightness::on_m_BrightnessUpPushButton_clicked()
{
    ui->m_BrightnessHorizontalSlider->setValue(ui->m_BrightnessHorizontalSlider->value()+ui->m_BrightnessHorizontalSlider->pageStep());
}
