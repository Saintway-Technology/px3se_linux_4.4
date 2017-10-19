#ifndef SETTINGMIDDLEWIDGETS_H
#define SETTINGMIDDLEWIDGETS_H

#include <QStackedWidget>

#include "basewidget.h"
#include "leftwidgets.h"
#include "wifi/wifiwidgets.h"
#include "bluetooth/bluetoothwidgets.h"
#include "brightness/brightnesswidgets.h"
#include "calendar/calendarwidgets.h"
#include "volume/volumewidgets.h"
#include "updater/updaterwidgets.h"
#include "language/languagewidgets.h"

class WifiWidgets;

/**
 * On behalf of middle part of settings application.
 * It is made up of 'LeftWidgets' and a 'stackedWidget'
 *
 * Every stacked item has their own ui and logic processing.
 */
class SettingMiddleWidgets:public BaseWidget
{
    Q_OBJECT
public:
    SettingMiddleWidgets(QWidget *parent);
    ~SettingMiddleWidgets();
private:
    /* Right stacked widgets */
    QStackedWidget *m_stackedWid;
    LeftWidgets *m_leftWid;

    WifiWidgets *m_wifiWid;
    BluetoothWidgets *m_bluetoothWid;
    BrightnessWidgets *m_brightnessWid;
    CalendarWidgets *m_calendarWid;
    VolumeWidgets *m_volumnWid;
    UpdaterWidgets *m_updaterWid;
    LanguageWidgets *m_languageWid;

    void initLayout();
    void initConnection();
private slots:
    void slot_currentWidgetChanged(int index);
};

#endif // MIDDLEWIDGETS_H
