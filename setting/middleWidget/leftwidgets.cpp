#include "leftwidgets.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "global_value.h"

leftWidgets::leftWidgets(QWidget *parent):baseWidget(parent)
{
    setStyleSheet("QWidget{background:rgb(32,38,51)}");
    initLayout();
}

void leftWidgets::initLayout()
{
    m_funtionlist = new funtiontablewidget(this);
    m_funtionlist->setRowCount(5);
    QStringList normaliconlist,selectediconlist,namelist;
    normaliconlist.append(":/image/setting/wifi_normal.png");
    normaliconlist.append(":/image/setting/bt_normal.png");
    normaliconlist.append(":/image/setting/brightness.png");
    normaliconlist.append(":/image/setting/calendar.png");
    normaliconlist.append(":/image/setting/speaker.png");

    selectediconlist.append(":/image/setting/wifi_seleted.png");
    selectediconlist.append(":/image/setting/bt_seleted.png");
    selectediconlist.append(":/image/setting/brightness.png");
    selectediconlist.append(":/image/setting/calendar.png");
    selectediconlist.append(":/image/setting/speaker.png");

    namelist.append(str_net_manager);
    namelist.append(str_bt_manager);
    namelist.append(str_brightness_setting);
    namelist.append(str_calendar_setting);
    namelist.append(str_volumn_setting);

    m_funtionlist->addFunctionItems(normaliconlist,selectediconlist,namelist);


    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addWidget(m_funtionlist);
    vmainlyout->setContentsMargins(0,0,0,0);

    setLayout(vmainlyout);
}
