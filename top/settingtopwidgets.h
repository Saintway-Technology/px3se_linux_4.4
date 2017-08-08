#ifndef SETTINGTOPWIDGETS_H
#define SETTINGTOPWIDGETS_H

#include "basewidget.h"
#include "basepushbutton.h"

#include <QLabel>
#include <QHBoxLayout>

class SettingTopWidgets:public BaseWidget
{
public:
    SettingTopWidgets(QWidget *parent=0);
    ~SettingTopWidgets();
public:
    FourStateButton *m_btnreturn;
    FlatButton *m_btnicon;
    FlatButton *m_btnmobile;
    FlatButton *m_btnsetting;
    FlatButton *m_btnmini;
    FlatButton *m_btnexit;
private:
    void initLayout();
};

#endif // SETTINGTOPWIDGETS_H
