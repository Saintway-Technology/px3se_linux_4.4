#include "bluetoothscannerwidgets.h"

#include <QHBoxLayout>
#include <QLabel>
#include "global_value.h"
#include "base/cmessagebox.h"
#include "pairedinformationdialog.h"

#ifdef DEVICE_EVB
int bluetooth_switch_width = 100;
int bluetooth_switch_height = 40;
int bluetooth_refresh_size = 50;
int bottom_margin = 500;
#else
int bluetooth_switch_width = 50;
int bluetooth_switch_height = 23;
int bluetooth_refresh_size = 30;
int bottom_margin = 100;
#endif

BluetoothScannerWidgets::BluetoothScannerWidgets(QWidget *parent):BaseWidget(parent)
{
    initData();
    initLayout();
    initConnection();
}

void BluetoothScannerWidgets::initData()
{
    m_localDevice = new QBluetoothLocalDevice;

    /*
     * In case of multiple Bluetooth adapters it is possible to set adapter
     * which will be used. Example code:
     *
     * QBluetoothAddress address("XX:XX:XX:XX:XX:XX");
     * discoveryAgent = new QBluetoothDeviceDiscoveryAgent(address);
     *
     **/
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent();

    m_bluez5Helper = new Bluez5Helper(this);

    firstEnter = true;
}

void BluetoothScannerWidgets::initLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;

    // 1. bluetooth switch.
    switchTitle = new QLabel(this);
    switchTitle->setText(tr("Bluetooth(%1)").arg( m_localDevice->name()));
    switchTitle->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);

    m_bluetoothSwitch = new SwitchButton(this);
    m_bluetoothSwitch->setFixedSize(bluetooth_switch_width,bluetooth_switch_height);
    m_bluetoothSwitch->setChecked(m_localDevice->hostMode()!=QBluetoothLocalDevice::HostPoweredOff);

    QFrame *separator = new QFrame(this);
    separator->setFixedHeight(2);
    separator->setStyleSheet("QFrame{border:1px solid rgb(100,100,100,255);}");
    separator->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    QHBoxLayout *switchLayout = new QHBoxLayout;
    switchLayout->addWidget(switchTitle);
    switchLayout->addWidget(m_bluetoothSwitch);
    switchLayout->setContentsMargins(10,0,10,0);

    // 2.bluetooth device list
    availableTitle = new QLabel(this);
    availableTitle->setText(tr("Available Devices"));
    m_table = new BluetoothDeviceTable(this);

    m_refreshButton = new RotatableButton(":/image/setting/bluetooth_scan.png",this);
    m_refreshButton->setFixedSize(bluetooth_refresh_size,bluetooth_refresh_size);

    QHBoxLayout *refreshLayout = new QHBoxLayout;
    refreshLayout->addWidget(availableTitle);
    refreshLayout->addStretch(0);
    refreshLayout->addWidget(m_refreshButton);
    refreshLayout->setContentsMargins(10,0,20,0);

    QVBoxLayout *tableLayout = new QVBoxLayout;
    tableLayout->addLayout(refreshLayout);
    tableLayout->addWidget(m_table);

    layout->addSpacing(30);
    layout->addLayout(switchLayout);
    layout->addWidget(separator);
    layout->addLayout(tableLayout);
    layout->addSpacing(bottom_margin);

    setLayout(layout);
}

void BluetoothScannerWidgets::initConnection()
{
    connect(m_bluetoothSwitch,SIGNAL(checkedChanged(bool)),this,SLOT(slot_onBluetoothSwitchCheckChanged(bool)));

    connect(m_discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(slot_addDevice(QBluetoothDeviceInfo)));
    connect(m_discoveryAgent, SIGNAL(canceled()), this, SLOT(slot_scanFinished()));
    connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(slot_scanFinished()));
    connect(m_localDevice, SIGNAL(hostModeStateChanged(QBluetoothLocalDevice::HostMode)),
            this, SLOT(slot_hostModeStateChanged(QBluetoothLocalDevice::HostMode)));
    connect(m_localDevice,SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)),this,SLOT(slot_paringDone(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
    connect(m_localDevice,SIGNAL(deviceConnected(QBluetoothAddress)),this,SLOT(slot_onDeviceConnected(QBluetoothAddress)));
    connect(m_localDevice,SIGNAL(deviceDisconnected(QBluetoothAddress)),this,SLOT(slot_onDeviceDisconnected(QBluetoothAddress)));
    slot_hostModeStateChanged(m_localDevice->hostMode());

    connect(m_refreshButton,SIGNAL(clicked(bool)),this,SLOT(slot_reScan()));
    connect(m_table,SIGNAL(cellClicked(int,int)),this,SLOT(slot_onTableItemClicked(int,int)));
    connect(m_table,SIGNAL(longPressedEvent(int)),this,SLOT(slot_onTableItemlongPressed(int)));

    connect(mainWindow,SIGNAL(retranslateUi()),this,SLOT(retranslateUi()));
}

void BluetoothScannerWidgets::retranslateUi(){
    availableTitle->setText(tr("Available Devices"));
    switchTitle->setText(tr("Bluetooth(%1)").arg( m_localDevice->name()));
}

void BluetoothScannerWidgets::slot_onBluetoothSwitchCheckChanged(bool checked)
{
    if(checked){
        m_localDevice->powerOn();
    }else{
        m_localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    }

}

void BluetoothScannerWidgets::slot_addDevice(const QBluetoothDeviceInfo &info)
{
    qDebug("get bluetooth item: %s, rssid: %d",qPrintable(info.name()),info.rssi());
    QList<QTableWidgetItem*> items = m_table->findItems(info.address().toString(),Qt::MatchContains);
    if(items.empty()){
        QBluetoothLocalDevice::Pairing pairingStatus = m_localDevice->pairingStatus(info.address());
        if(pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired){
            if(m_bluez5Helper->isDeviceConnected(info.address())){
                m_table->insertIntoTable(info.name(),info.address().toString(),"Connected");
            }else{
                m_table->insertIntoTable(info.name(),info.address().toString(),"Paired");
            }
        }else{
            m_table->insertIntoTable(info.name(),info.address().toString(),"");
        }
    }
}

void BluetoothScannerWidgets::slot_onTableItemClicked(int row,int)
{
    int result;
    QString name = m_table->getItemName(row);
    QString address = m_table->getItemAddress(row);
    QBluetoothAddress bluetoothAddress(address);

    QBluetoothLocalDevice::Pairing pairingStatus = m_localDevice->pairingStatus(bluetoothAddress);
    if(pairingStatus == QBluetoothLocalDevice::Paired || pairingStatus == QBluetoothLocalDevice::AuthorizedPaired ){
        if(!m_bluez5Helper->isDeviceConnected(bluetoothAddress)){
            result = CMessageBox::showCMessageBox(mainWindow,"connect with " + name.append("?"),"Connect","Cancel");
            if(result == RESULT_CONFIRM){
                m_bluez5Helper->connectDevice(bluetoothAddress);
            }
        }else{
            result = CMessageBox::showCMessageBox(mainWindow,"disconnect with " + name.append("?"),"Disconnect","Cancel");
            if(result == RESULT_CONFIRM){
                m_bluez5Helper->disconnectDevice(bluetoothAddress);
            }
        }
    }else{
        result = CMessageBox::showCMessageBox(mainWindow,"pair with "+ name.append("?"),"Pair","Cancel");
        if(result == RESULT_CONFIRM){
            m_localDevice->requestPairing(bluetoothAddress, QBluetoothLocalDevice::Paired);
            m_table->updateItemState(row,"Pairing..");
        }
    }
}

void BluetoothScannerWidgets::slot_onTableItemlongPressed(int row)
{
    QString name = m_table->getItemName(row);
    QString address = m_table->getItemAddress(row);
    QBluetoothAddress bluetoothAddress(address);

    if(m_localDevice->pairingStatus(bluetoothAddress) != QBluetoothLocalDevice::Unpaired){
        int result = PairedInformationDialog::showInformationDialog(this,name,address);
        if(result == PairedInformationDialog::RESULT_CANCELSAVE){
            m_localDevice->requestPairing(bluetoothAddress, QBluetoothLocalDevice::Unpaired);
        }
    }
}

void BluetoothScannerWidgets::slot_scanFinished()
{
    m_refreshButton->stopAnimation();
    qDebug("scan finished");
}

void BluetoothScannerWidgets::clearDiscoveredDevices()
{
    QList<QBluetoothDeviceInfo> discoveredDevices = m_discoveryAgent->discoveredDevices();
    qDebug("discovery devices list.size: %d",discoveredDevices.size());
    for(int i=0;i<discoveredDevices.size();i++){
        m_bluez5Helper->removeUnpairedDevice(discoveredDevices.at(i).address());
    }
}

void BluetoothScannerWidgets::slot_reScan()
{   
    if(m_discoveryAgent->isActive()){
        return;
    }
    qDebug("start scan.");
    m_refreshButton->startAnimation();
    m_table->clearTable();
    clearDiscoveredDevices();
    m_discoveryAgent->start();

    if(firstEnter){
        // when first enter application discovered devices become 0,so clean again when
        // scan first.
        qDebug("scan later.");
        QTimer::singleShot(500,this,SLOT(slot_cleanScanFirst()));
        firstEnter = false;
    }
}

void BluetoothScannerWidgets::slot_cleanScanFirst()
{
    if(m_discoveryAgent->isActive()){
        m_discoveryAgent->stop();
    }
    m_refreshButton->startAnimation();
    m_table->clearTable();
    clearDiscoveredDevices();
    m_discoveryAgent->start();
}

void BluetoothScannerWidgets::slot_paringDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing)
{
    qDebug("slot_paringDone: address: %s,pairing: %d",address.toString().toLatin1().data(),pairing);
    QList<QTableWidgetItem*> items = m_table->findItems(address.toString(),Qt::MatchContains);

    if(pairing == QBluetoothLocalDevice::Paired || pairing == QBluetoothLocalDevice::AuthorizedPaired ){
        for(int var = 0;var < items.count();++var){
            m_bluez5Helper->connectDevice(address);
            if(m_bluez5Helper->isDeviceConnected(address)){
                m_table->updateItemState(items.at(var)->row(),"Connected");
            }else{
                m_table->updateItemState(items.at(var)->row(),"Paired");
            }
        }
    }else{
        for(int var = 0;var < items.count();++var){
            m_table->updateItemState(items.at(var)->row(),"");
        }
    }
    m_table->sortTable();
}

void BluetoothScannerWidgets::slot_onDeviceConnected(QBluetoothAddress address)
{
    qDebug("device connected: %s",address.toString().toLatin1().data());
    QList<QTableWidgetItem*> items = m_table->findItems(address.toString(),Qt::MatchContains);
    for(int var = 0;var < items.count();++var){
        m_table->updateItemState(items.at(var)->row(),"Connected");
    }
    m_table->sortTable();
}

void BluetoothScannerWidgets::slot_onDeviceDisconnected(QBluetoothAddress address)
{
    qDebug("device disconnected: %s",address.toString().toLatin1().data());
    QList<QTableWidgetItem*> items = m_table->findItems(address.toString(),Qt::MatchContains);

    QBluetoothLocalDevice::Pairing pairing = m_localDevice->pairingStatus(address);
    if(pairing == QBluetoothLocalDevice::Paired || pairing == QBluetoothLocalDevice::AuthorizedPaired ){
        for(int var = 0;var < items.count();++var){
            m_table->updateItemState(items.at(var)->row(),"Paired");
        }
    }else{
        for(int var = 0;var < items.count();++var){
            m_table->updateItemState(items.at(var)->row(),"");
        }
    }
    m_table->sortTable();
}

void BluetoothScannerWidgets::slot_hostModeStateChanged(QBluetoothLocalDevice::HostMode mode)
{
    qDebug("slot_hostModeStateChanged: %d",mode);
    switch(mode){
    case QBluetoothLocalDevice::HostPoweredOff:
        m_table->clearTable();
        m_refreshButton->stopAnimation();
        if(m_discoveryAgent->isActive()){
            qDebug("stop discovery");
            m_discoveryAgent->stop();
        }
        break;
    case QBluetoothLocalDevice::HostConnectable:
    case QBluetoothLocalDevice::HostDiscoverable:
    case QBluetoothLocalDevice::HostDiscoverableLimitedInquiry:
        slot_reScan();
        break;
    default:
        break;
    }
}
