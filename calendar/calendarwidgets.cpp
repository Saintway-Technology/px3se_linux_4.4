#include "calendarwidgets.h"
#include <QVBoxLayout>

CalendarWidgets::CalendarWidgets(QWidget *parent):BaseWidget(parent)
{
    // Set background color.
    setObjectName("CalendarWidgets");
    setStyleSheet("#CalendarWidgets{background:rgb(33,36,43)}");

    initData();
    initLayout();
}

void CalendarWidgets::initData()
{
    m_calendarWidget = new Widget(this);
}

void CalendarWidgets::initLayout()
{
    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addSpacing(30);
    vmainlyout->addWidget(m_calendarWidget);
    vmainlyout->addSpacing(20);
    vmainlyout->addStretch(0);
    vmainlyout->setContentsMargins(0,0,0,0);

    // Set layout in middle.
    QHBoxLayout *hmainlyout = new QHBoxLayout;
    hmainlyout->addStretch(1);
    hmainlyout->addLayout(vmainlyout,4);
    hmainlyout->addStretch(1);
    setLayout(hmainlyout);
}

CalendarWidgets::~CalendarWidgets()
{
    delete m_calendarWidget;
}

