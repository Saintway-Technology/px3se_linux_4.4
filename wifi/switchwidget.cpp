#include "switchwidget.h"
#include "QHBoxLayout"

#ifdef DEVICE_EVB
int wifi_switch_width = 80;
int wifi_switch_height = 40;
#else
int wifi_switch_width = 40;
int wifi_switch_height = 20;
#endif

SwitchWidget::SwitchWidget(QWidget *parent,QString text):BaseWidget(parent)
{
    setFixedHeight(wifi_switch_height);

    QHBoxLayout *mainlyout = new QHBoxLayout;

    m_lblState = new QLabel(this);
    m_lblState->setText(text);

    m_btnSwitch = new CSwitchButton(this);
    m_btnSwitch->setFixedSize(wifi_switch_width,wifi_switch_height);

    mainlyout->addWidget(m_lblState);
    mainlyout->addWidget(m_btnSwitch);
    mainlyout->setContentsMargins(0,0,0,0);
    mainlyout->setSpacing(0);

    setLayout(mainlyout);
}
void SwitchWidget::setText(QString text){
    m_lblState->setText((text));
}
