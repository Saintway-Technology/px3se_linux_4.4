#ifndef RIGHTSTACKEDWIDGETS2_H
#define RIGHTSTACKEDWIDGETS2_H

#include <basewidget.h>
#include "brightness.h"



class rightStackedWidgets2:public baseWidget
{
    Q_OBJECT
public:
    rightStackedWidgets2(QWidget *parent);

private:
    Brightness *m_brightnessWidget;

    void initData();
    void initLayout();
};



#endif // RIGHTSTACKEDWIDGETS2_H
