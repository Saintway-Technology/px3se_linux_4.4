#ifndef SETTINGMIDDLEWIDGETS_H
#define SETTINGMIDDLEWIDGETS_H

#include "basewidget.h"
#include "leftwidgets.h"
#include "rightstackedwidgets0.h"
#include "rightstackedwidgets1.h"
#include "rightstackedwidgets2.h"
#include "rightstackedwidgets3.h"
#include "rightstackedwidgets4.h"
#include "rightstackedwidgets5.h"

#include <QStackedWidget>

class settingMiddleWidgets:public baseWidget
{
    Q_OBJECT
public:
    settingMiddleWidgets(QWidget *parent);
private:
    void initLayout();
    void initConnection();

    leftWidgets *m_leftWid;
    QStackedWidget *m_stackedWid;
    rightStackedWidgets0 *m_wifiWid;
    rightStackedWidgets1 *m_otherWid;
    rightStackedWidgets2 *m_brightnessWid;
    rightStackedWidgets3 *m_calendarWid;
    rightStackedWidgets4 *m_volumnWid;
    rightStackedWidgets5 *m_updaterWid;
private slots:
    void slot_currentWidgetChanged(int index);
signals:


};

#endif // MIDDLEWIDGETS_H
