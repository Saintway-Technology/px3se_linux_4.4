#ifndef BLUETOOTHSCANNERWIDGETS_H
#define BLUETOOTHSCANNERWIDGETS_H

#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/qbluetoothglobal.h>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothAddress>
#include <QtBluetooth/QLowEnergyController>

#include "bluetoothdevicetable.h"
#include "base/basewidget.h"
#include "base/switchbutton.h"
#include "base/basepushbutton.h"

#include "bluez5/bluez5helper.h"

class BluetoothScannerWidgets:public BaseWidget
{
    Q_OBJECT
public:
    BluetoothScannerWidgets(QWidget *parent);
    ~BluetoothScannerWidgets(){}

private:
    bool firstEnter;

    QBluetoothLocalDevice *m_localDevice;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;

    RotatableButton *m_refreshButton;
    BluetoothDeviceTable *m_table;

    SwitchButton *m_bluetoothSwitch;

    Bluez5Helper *m_bluez5Helper;

    void initData();
    void initLayout();
    void initConnection();
    void clearDiscoveredDevices();

private slots:
    void slot_addDevice(const QBluetoothDeviceInfo&);
    void slot_scanFinished();
    void slot_hostModeStateChanged(QBluetoothLocalDevice::HostMode);
    void slot_onBluetoothSwitchCheckChanged(bool);
    void slot_reScan();
    void slot_cleanScanFirst();
    void slot_paringDone(const QBluetoothAddress&,QBluetoothLocalDevice::Pairing);
    void slot_onDeviceConnected(QBluetoothAddress);
    void slot_onDeviceDisconnected(QBluetoothAddress);
    void slot_onTableItemClicked(int,int);
    void slot_onTableItemlongPressed(int);

};

#endif // BLUETOOTHSCANNERWIDGETS_H
