#ifndef RIGHTSTACKEDWIDGETS3_H
#define RIGHTSTACKEDWIDGETS3_H

#include <basewidget.h>
#include "widget.h"



class rightStackedWidgets3:public baseWidget
{
    Q_OBJECT
public:
    rightStackedWidgets3(QWidget *parent);

private:
    Widget *m_calendarWidget;

    void initData();
    void initLayout();
};



#endif // RIGHTSTACKEDWIDGETS3_H
