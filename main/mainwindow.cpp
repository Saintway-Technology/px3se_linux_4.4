#include "mainwindow.h"
#include "global_value.h"

MainWindow::MainWindow(QWidget *parent) :BaseWindow(parent)
{
    // Init global main class of 'MainWindow' for other widgets invokes.
    mainWindow = this;

    initLayout();
    initConnection();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initLayout(){
    QVBoxLayout *mainLayout = new QVBoxLayout;
    m_setttingsWid = new SettingWidgets(this);

    mainLayout->addWidget(m_setttingsWid);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);
}

void MainWindow::initConnection()
{
    connect(m_setttingsWid->m_topWid->m_btnmini,SIGNAL(clicked(bool)),this,SLOT(showMinimized()));
    connect(m_setttingsWid->m_topWid->m_btnexit,SIGNAL(clicked(bool)),this,SLOT(slot_appQuit()));
    connect(m_setttingsWid->m_topWid->m_btnreturn,SIGNAL(clicked(bool)),this,SLOT(slot_appQuit()));
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
