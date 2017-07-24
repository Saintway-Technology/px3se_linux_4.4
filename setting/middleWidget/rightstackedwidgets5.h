#ifndef RIGHTSTACKEDWIDGETS5_H
#define RIGHTSTACKEDWIDGETS5_H

#include <basewidget.h>
#include "updaterwidget.h"



class rightStackedWidgets5:public baseWidget
{
    Q_OBJECT
public:
    rightStackedWidgets5(QWidget *parent);

private:
    UpdaterWidget *m_updaterWidget;

    void initData();
    void initLayout();
};



#endif // RIGHTSTACKEDWIDGETS5_H
