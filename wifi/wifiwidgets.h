#ifndef RIGHTSTACKEDWIDGETS0_H
#define RIGHTSTACKEDWIDGETS0_H

#include <QTabWidget>
#include <QLabel>
#include <QThread>
#include <QLineEdit>
#include <QComboBox>
#include <QTimer>

#include "networklist.h"
#include "wlanlisttable.h"
#include "wpa_supplicant/wpamanager.h"
#include "switchwidget.h"
#include "basewidget.h"
#include "basepushbutton.h"
#include "netconfigdialog.h"
#include "global_value.h"
#include "tabcurrentstatus.h"

class TabScanResult;
class TabNetworkManager;
class NetThread;
class netConfigDialog;

struct netWork
{
    QString ssid;
    QString bssid;
    QString frequence;
    QString signal;
    QString flags;
};

/**
 * Wifi moudle contains wifi station(wireless) and hostapd(hotspot).
 * This potted widget contain all layout of ui.
 */
class WifiWidgets:public BaseWidget
{
    Q_OBJECT
public:
    WifiWidgets(QWidget *parent);
    ~WifiWidgets();
private:
    // Used for control wpa_supplicant service.
    WpaManager *m_netManager;

    SwitchWidget *m_wifiSwitch;
    // The main tab Widget related to wifi.
    // Include 'scan result' and 'current status'.
    QTabWidget *m_tab;

    // Related to HOSTAP
    SwitchWidget *m_hostAPSwitch;
    QLineEdit *m_hostAPName;
    QLineEdit *m_hostAPPassword;

    QLabel *textStatus;
    QLabel *apNameLabel;
    QLabel *apPasswordLabel;
    QLabel *statusLabel;

    netConfigDialog *editdialog;
    netConfigDialog *showdialog;
public:
    TabCurrentStatus *m_tabCurrentStatus;
    TabScanResult *m_tabScanResult;
    TabNetworkManager *m_tabNetworkManager;

    /**
     * allocate ip when success connect to wpa_supplicant.
     */
    void getIPAddress();

    void setStatusText(QString text){textStatus->setText(text);}
    QString getStatusText(){return textStatus->text();}
private:
    // Used for LAN connection.
    QTimer *m_workTimer;

    void initData();
    void initLayout();
    void initConnection();

    void wifiStationOpen();
    void wifiStationClose();
    void wifiHostapdOpen();
    void wifiHostapdClose();
public slots:
    void slot_showItemDetail(int,int);
    void slot_onWifiToggled(bool isChecked);
    void slot_onHostapdToggled(bool isChecked);
    void slot_removeListedNetwork();
    void slot_editListedNetwork();

    void setHostapdUnchcked();
    void setWifiUnchecked();

    void slot_checkLanConnection();

    void retranslateUi();
signals:

};



class TabScanResult:public BaseWidget
{
public:
    TabScanResult(QWidget *parent);

    QVBoxLayout *vmainlyout;
    QHBoxLayout *lyout1;
    WlanListTable *m_table;
    QPushButton *scanButton;

    struct netWork* m_netWorks = new struct netWork[200];

    void clearTable();
    void insertIntoTable(QString name,QString lock,QString siganl,QString detail);
    void setText(QString text);
};

/**
 * Used for modify or remove configured network.
 */
class TabNetworkManager:public BaseWidget{
public:
    TabNetworkManager(QWidget *parent);

    NetworkList *m_networkList;
    QPushButton *removeButton;
    QPushButton *editButton;
};

/**
 * This Thread is for start wpa_supplicant service.
 */
class NetThread:public QThread
{
public:
    NetThread(QObject *parent = 0): QThread(parent){}
protected:
    virtual void run();
};

#endif // RIGHTSTACKEDWIDGETS0_H
