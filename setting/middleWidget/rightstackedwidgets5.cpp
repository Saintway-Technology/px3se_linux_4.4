#include "rightstackedwidgets5.h"
#include <QVBoxLayout>
rightStackedWidgets5::rightStackedWidgets5(QWidget *parent):baseWidget(parent)
{
    setObjectName("rightStackedWidgets5");
    setStyleSheet("#rightStackedWidgets5{background:rgb(33,36,43)}");
    initData();
    initLayout();
}

void rightStackedWidgets5::initData()
{
    m_updaterWidget = new UpdaterWidget(this);
}

void rightStackedWidgets5::initLayout()
{
    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addSpacing(30);
    vmainlyout->addWidget(m_updaterWidget);
    vmainlyout->addSpacing(20);
    vmainlyout->addStretch(0);
    vmainlyout->setContentsMargins(0,0,0,0);

    QHBoxLayout *hmainlyout = new QHBoxLayout;
    hmainlyout->addStretch(1);
    hmainlyout->addLayout(vmainlyout,4);
    hmainlyout->addStretch(1);
    setLayout(hmainlyout);
}

