#ifndef RIGHTSTACKEDWIDGETS4_H
#define RIGHTSTACKEDWIDGETS4_H

#include <basewidget.h>
#include "volumnwidget.h"



class rightStackedWidgets4:public baseWidget
{
    Q_OBJECT
public:
    rightStackedWidgets4(QWidget *parent);

private:
    VolumnWidget *m_volumnWidget;

    void initData();
    void initLayout();
};



#endif // RIGHTSTACKEDWIDGETS4_H
