#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include <QLabel>
#include "cswitchbutton.h"
#include "basewidget.h"

class SwitchWidget:public BaseWidget
{
public:
    SwitchWidget(QWidget *parent,QString text);
    CSwitchButton* getSwitchButton(){return m_btnSwitch;}
    void setText(QString text);
private:
    QLabel *m_lblState;
    CSwitchButton *m_btnSwitch;
};

#endif // SWITCHWIDGET_H
