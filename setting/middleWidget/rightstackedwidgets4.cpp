#include "rightstackedwidgets4.h"
#include <QVBoxLayout>
rightStackedWidgets4::rightStackedWidgets4(QWidget *parent):baseWidget(parent)
{
    setObjectName("rightStackedWidgets4");
    setStyleSheet("#rightStackedWidgets4{background:rgb(33,36,43)}");
    initData();
    initLayout();
}

void rightStackedWidgets4::initData()
{
    m_volumnWidget = new VolumnWidget(this);
}

void rightStackedWidgets4::initLayout()
{
    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addSpacing(30);
    vmainlyout->addWidget(m_volumnWidget);
    vmainlyout->addSpacing(20);
    vmainlyout->addStretch(0);
    vmainlyout->setContentsMargins(0,0,0,0);

    QHBoxLayout *hmainlyout = new QHBoxLayout;
    hmainlyout->addStretch(1);
    hmainlyout->addLayout(vmainlyout,4);
    hmainlyout->addStretch(1);
    setLayout(hmainlyout);
}

