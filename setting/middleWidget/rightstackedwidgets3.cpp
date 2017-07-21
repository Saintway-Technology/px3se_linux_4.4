#include "rightstackedwidgets3.h"
#include <QVBoxLayout>
rightStackedWidgets3::rightStackedWidgets3(QWidget *parent):baseWidget(parent)
{
    setObjectName("rightStackedWidgets3");
    setStyleSheet("#rightStackedWidgets3{background:rgb(33,36,43)}");
    initData();
    initLayout();
}

void rightStackedWidgets3::initData()
{
    m_calendarWidget = new Widget(this);
}

void rightStackedWidgets3::initLayout()
{
    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addSpacing(30);
    vmainlyout->addWidget(m_calendarWidget);
    vmainlyout->addSpacing(20);
    vmainlyout->addStretch(0);
    vmainlyout->setContentsMargins(0,0,0,0);

    QHBoxLayout *hmainlyout = new QHBoxLayout;
    hmainlyout->addStretch(1);
    hmainlyout->addLayout(vmainlyout,4);
    hmainlyout->addStretch(1);
    setLayout(hmainlyout);
}

