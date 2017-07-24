#include "settingmiddlewidgets.h"
#include <QHBoxLayout>

settingMiddleWidgets::settingMiddleWidgets(QWidget *parent):baseWidget(parent)
{
    initLayout();
    initConnection();
}

void settingMiddleWidgets::initLayout()
{
    QHBoxLayout *hmainlyout = new QHBoxLayout;

    m_leftWid = new leftWidgets(this);
    m_stackedWid = new QStackedWidget(this);

    m_wifiWid = new rightStackedWidgets0(m_stackedWid);
    m_otherWid = new rightStackedWidgets1(m_stackedWid);
    m_brightnessWid = new rightStackedWidgets2(m_stackedWid);
    m_calendarWid = new rightStackedWidgets3(m_stackedWid);
    m_volumnWid = new rightStackedWidgets4(m_stackedWid);
    m_updaterWid = new rightStackedWidgets5(m_stackedWid);

    m_stackedWid->addWidget(m_wifiWid);
    m_stackedWid->addWidget(m_otherWid);
    m_stackedWid->addWidget(m_brightnessWid);
    m_stackedWid->addWidget(m_calendarWid);
    m_stackedWid->addWidget(m_volumnWid);
    m_stackedWid->addWidget(m_updaterWid);

    hmainlyout->addWidget(m_leftWid,1);
    hmainlyout->addWidget(m_stackedWid,4);    

    hmainlyout->setContentsMargins(0,0,0,0);
    hmainlyout->setSpacing(0);
    setLayout(hmainlyout);
}

void settingMiddleWidgets::initConnection()
{
    connect(m_leftWid->m_funtionlist,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_currentWidgetChanged(int)));
    slot_currentWidgetChanged(0);  // 初始化当前界面在Wifi管理类
}

void settingMiddleWidgets::slot_currentWidgetChanged(int index)
{
    m_leftWid->m_funtionlist->setCurrentCell(index,0);
    switch(index){
    case 0:
        m_stackedWid->setCurrentIndex(0);
        break;
    case 1:
        m_stackedWid->setCurrentIndex(1);
        break;
    case 2:
        m_stackedWid->setCurrentIndex(2);
        break;
    case 3:
        m_stackedWid->setCurrentIndex(3);
        break;
    case 4:
        m_stackedWid->setCurrentIndex(4);
        break;
    case 5:
        m_stackedWid->setCurrentIndex(5);
        break;
    default:
        m_stackedWid->setCurrentIndex(1);
        break;
    }
}
