#include "mainwindow.h"
#include "global_value.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :baseWindow(parent),mediaHasUpdate(false)

{
    mainwid=this;
    initLayout();

    connect(m_wid->m_topWid->m_btnmini,SIGNAL(clicked(bool)),this,SLOT(showMinimized()));
    connect(m_wid->m_topWid->m_btnexit,SIGNAL(clicked(bool)),this,SLOT(slot_appQuit()));
    connect(m_wid->m_topWid->m_btnreturn,SIGNAL(clicked(bool)),this,SLOT(slot_returnanimation()));



}

MainWindow::~MainWindow()
{

}

void MainWindow::initLayout(){
   QVBoxLayout *mainLayout = new QVBoxLayout;
    m_wid = new settingWidgets(this);
    mainLayout->addWidget(m_wid);
    setLayout(mainLayout);
    mainLayout->setContentsMargins(0,0,0,0);
}
void MainWindow::slot_appQuit()
{
    this->close();
}
void MainWindow::slot_returnanimation()
{

    this->close();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qDebug()<<"key value is:"<<event->key();
    switch(event->key())
    {
    // update musicPlayer and videoPlayer's volume by Key
    case Qt::Key_VolumeDown:
        QWidget::keyPressEvent(event);
        break;
    case Qt::Key_VolumeUp:
        QWidget::keyPressEvent(event);
        break;
    case Qt::Key_PowerOff:   // when key_power enter
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
