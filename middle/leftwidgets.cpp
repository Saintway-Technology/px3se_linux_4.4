#include "leftwidgets.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

#include "global_value.h"

LeftWidgets::LeftWidgets(QWidget *parent):BaseWidget(parent)
{
    // Set background color.
    setObjectName("LeftWidgets");
    setStyleSheet("#LeftWidgets{background:rgb(32,38,51)}");

    initLayout();
}

void LeftWidgets::initLayout()
{
    /* Add setting tab item here.
       You nend change 'RowCount'、append title list and icon list */
    m_funtionlist = new Funtiontablewidget(this);
    m_funtionlist->setRowCount(6);

    QStringList normaliconlist,selectediconlist,namelist;

    normaliconlist.append(":/image/setting/wifi_normal.png");
    normaliconlist.append(":/image/setting/bt_normal.png");
    normaliconlist.append(":/image/setting/brightness.png");
    normaliconlist.append(":/image/setting/calendar.png");
    normaliconlist.append(":/image/setting/speaker.png");
    normaliconlist.append(":/image/setting/update.png");

    selectediconlist.append(":/image/setting/wifi_seleted.png");
    selectediconlist.append(":/image/setting/bt_seleted.png");
    selectediconlist.append(":/image/setting/brightness.png");
    selectediconlist.append(":/image/setting/calendar.png");
    selectediconlist.append(":/image/setting/speaker.png");
    selectediconlist.append(":/image/setting/update.png");

    namelist.append(str_net_manager);
    namelist.append(str_bt_manager);
    namelist.append(str_brightness_setting);
    namelist.append(str_calendar_setting);
    namelist.append(str_volumn_setting);
    namelist.append(str_updater_setting);

    m_funtionlist->addFunctionItems(normaliconlist,selectediconlist,namelist);

    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addWidget(m_funtionlist);
    vmainlyout->setContentsMargins(0,0,0,0);
    vmainlyout->setSpacing(0);

    setLayout(vmainlyout);
}

LeftWidgets::~LeftWidgets()
{
    delete m_funtionlist;
}
