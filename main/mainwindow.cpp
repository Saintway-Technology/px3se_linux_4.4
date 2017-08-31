#include "mainwindow.h"
#include "global_value.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) :BaseWindow(parent)
{
    initData();
    initLayout();
}

void MainWindow::initData()
{
    // Initialize global main class of 'MainWindow' for other widgets invokes.
    mainWindow = this;
}

void MainWindow::initLayout(){
    QVBoxLayout *mainLayout = new QVBoxLayout;
    m_setttingsWid = new SettingWidgets(this);

    mainLayout->addWidget(m_setttingsWid);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);
}

void MainWindow::disableApplication()
{
    qDebug("disable settting application");
    this->setVisible(false);
}

void MainWindow::enableApplication()
{
    qDebug("enable setting application");
    this->setVisible(true);
}

void MainWindow::slot_appQuit()
{
    this->close();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<"Get key value input:"<<event->key();
    switch(event->key())
    {
    // When key_power enter.
    case Qt::Key_PowerOff:
        QTimer::singleShot(100, this, SLOT(slot_standby()));
        break;
    default:
        break;
    }
}

void MainWindow::slot_standby()
{
    system("echo mem > /sys/power/state");
}
