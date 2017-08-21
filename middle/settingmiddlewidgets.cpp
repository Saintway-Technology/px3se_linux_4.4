#include "settingmiddlewidgets.h"
#include <QHBoxLayout>

SettingMiddleWidgets::SettingMiddleWidgets(QWidget *parent):BaseWidget(parent)
{
    initLayout();
    initConnection();

    /* Initial widget in wifi part */
    slot_currentWidgetChanged(0);
}

void SettingMiddleWidgets::initLayout()
{
    QHBoxLayout *hmainlyout = new QHBoxLayout;

    m_leftWid = new LeftWidgets(this);
    m_stackedWid = new QStackedWidget(this);

    m_wifiWid = new WifiWidgets(m_stackedWid);
    m_bluetoothWid = new BluetoothWidgets(m_stackedWid);
    m_brightnessWid = new BrightnessWidgets(m_stackedWid);
    m_calendarWid = new CalendarWidgets(m_stackedWid);
    m_volumnWid = new VolumeWidgets(m_stackedWid);
    m_updaterWid = new UpdaterWidgets(m_stackedWid);

    m_stackedWid->addWidget(m_wifiWid);
    m_stackedWid->addWidget(m_bluetoothWid);
    m_stackedWid->addWidget(m_brightnessWid);
    m_stackedWid->addWidget(m_calendarWid);
    m_stackedWid->addWidget(m_volumnWid);
    m_stackedWid->addWidget(m_updaterWid);

#ifdef DEVICE_EVB
    hmainlyout->addWidget(m_leftWid,1);
    hmainlyout->addWidget(m_stackedWid,3);
#else
    hmainlyout->addWidget(m_leftWid,1);
    hmainlyout->addWidget(m_stackedWid,4);
#endif

    hmainlyout->setContentsMargins(0,0,0,0);
    hmainlyout->setSpacing(0);
    setLayout(hmainlyout);
}

void SettingMiddleWidgets::initConnection()
{
    connect(m_leftWid->getList(),SIGNAL(currentIndexChanged(int)),this,SLOT(slot_currentWidgetChanged(int)));
}

void SettingMiddleWidgets::slot_currentWidgetChanged(int index)
{
    m_leftWid->getList()->setCurrentCell(index,0);
    switch(index){
    case 0:
        m_stackedWid->setCurrentWidget(m_wifiWid);
        break;
    case 1:
        m_stackedWid->setCurrentWidget(m_bluetoothWid);
        break;
    case 2:
        m_stackedWid->setCurrentWidget(m_brightnessWid);
        break;
    case 3:
        m_stackedWid->setCurrentWidget(m_calendarWid);
        break;
    case 4:
        m_stackedWid->setCurrentWidget(m_volumnWid);
        break;
    case 5:
        m_stackedWid->setCurrentWidget(m_updaterWid);
        break;
    default:
        m_stackedWid->setCurrentWidget(m_wifiWid);
        break;
    }
}

SettingMiddleWidgets::~SettingMiddleWidgets()
{
}
