#ifndef SETTINGTOPWIDGETS_H
#define SETTINGTOPWIDGETS_H

#include "basewidget.h"
#include "basepushbutton.h"

#include <QLabel>
#include <QHBoxLayout>

class SettingTopWidgets:public BaseWidget
{
    Q_OBJECT
public:
    SettingTopWidgets(QWidget *parent=0);
    ~SettingTopWidgets();
private:
    void initLayout();
    void initConnection();

    FourStateButton *m_btnReturn;
    FlatButton *m_btnIcon;
signals:
    void returnClicked();
};

#endif // SETTINGTOPWIDGETS_H
