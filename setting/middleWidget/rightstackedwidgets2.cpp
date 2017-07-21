#include "rightstackedwidgets2.h"
#include <QVBoxLayout>
rightStackedWidgets2::rightStackedWidgets2(QWidget *parent):baseWidget(parent)
{
    setObjectName("rightStackedWidgets2");
    setStyleSheet("#rightStackedWidgets2{background:rgb(33,36,43)}");
    initData();
    initLayout();
}

void rightStackedWidgets2::initData()
{
    m_brightnessWidget = new Brightness(this);
}

void rightStackedWidgets2::initLayout()
{
    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addSpacing(30);
    vmainlyout->addWidget(m_brightnessWidget);
    vmainlyout->addSpacing(20);
    vmainlyout->addStretch(0);
    vmainlyout->setContentsMargins(0,0,0,0);

    QHBoxLayout *hmainlyout = new QHBoxLayout;
    hmainlyout->addStretch(1);
    hmainlyout->addLayout(vmainlyout,4);
    hmainlyout->addStretch(1);
    setLayout(hmainlyout);
}

