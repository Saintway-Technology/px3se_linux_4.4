#include "bluetoothwidgets.h"

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
    m_scannerWidget = new BluetoothScannerWidget(this);
}

void BluetoothWidgets::initLayout()
{
    QVBoxLayout *vmainlyout = new QVBoxLayout;
    vmainlyout->addSpacing(30);
    vmainlyout->addWidget(m_scannerWidget);
    vmainlyout->addSpacing(20);
    vmainlyout->addStretch(0);
    vmainlyout->setContentsMargins(0,0,0,0);

    // Set layout in middle.
    QHBoxLayout *hmainlyout = new QHBoxLayout;
    hmainlyout->addStretch(1);
    hmainlyout->addLayout(vmainlyout,4);
    hmainlyout->addStretch(1);
    setLayout(hmainlyout);
}

BluetoothWidgets::~BluetoothWidgets(){
   delete m_scannerWidget;
}

