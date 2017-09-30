#include "bluetoothwidgets.h"
#include <QVBoxLayout>

#ifdef DEVICE_EVB
int layout_stretch = 15;
#else
int layout_stretch = 8;
#endif

BluetoothWidgets::BluetoothWidgets(QWidget *parent):BaseWidget(parent)
{
    //Set background color.
    setObjectName("BluetoothWidgets");
    setStyleSheet("#BluetoothWidgets{background:rgb(33,36,43)}");

    initData();
    initLayout();
}

void BluetoothWidgets::initData()
{
    m_scannerWidget = new BluetoothScannerWidgets(this);
}

void BluetoothWidgets::initLayout()
{
    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addWidget(m_scannerWidget);
    vmainlyout->setContentsMargins(0,0,0,0);

    // Set layout in middle.
    QHBoxLayout *hmainlyout = new QHBoxLayout;
    hmainlyout->addStretch(1);
    hmainlyout->addLayout(vmainlyout,layout_stretch);
    hmainlyout->addStretch(1);
    setLayout(hmainlyout);
}

BluetoothWidgets::~BluetoothWidgets(){
}

