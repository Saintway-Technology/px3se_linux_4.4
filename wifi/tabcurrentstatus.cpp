#include "tabcurrentstatus.h"
#include <QBoxLayout>

TabCurrentStatus::TabCurrentStatus(QWidget *parent):BaseWidget(parent)
{
    QVBoxLayout *vmainlyout = new QVBoxLayout;

    // LastMessage
    QHBoxLayout *lyout2 = new QHBoxLayout;
    lastMessageLabel = new QLabel(this);


    textLastMsg = new QLabel(this);
    textLastMsg->setText(QString());
    lyout2->addWidget(lastMessageLabel,1);
    lyout2->addWidget(textLastMsg,2);

    // Authenticant
    QHBoxLayout *lyout3 = new QHBoxLayout;
    authenticationLabel= new QLabel(this);


    textAuthentication = new QLabel(this);
    textAuthentication->setText(QString("None"));
    lyout3->addWidget(authenticationLabel,1);
    lyout3->addWidget(textAuthentication,2);

    // Encryption
    QHBoxLayout *lyout4 = new QHBoxLayout;
    encryptionLabel = new QLabel(this);


    textEncryption = new QLabel(this);
    textEncryption->setText(QString("None"));
    lyout4->addWidget(encryptionLabel,1);
    lyout4->addWidget(textEncryption,2);

    // SSID
    QHBoxLayout *lyout5 = new QHBoxLayout;
    ssidLabel = new QLabel(this);


    textSSID = new QLabel(this);
    textSSID->setText(QString("None"));
    lyout5->addWidget(ssidLabel,1);
    lyout5->addWidget(textSSID,2);

    // BSSID
    QHBoxLayout *lyout6 = new QHBoxLayout;
    bssidLabel = new QLabel(this);


    textBSSID = new QLabel(this);
    textBSSID->setText(QString("None"));
    lyout6->addWidget(bssidLabel,1);
    lyout6->addWidget(textBSSID,2);

    // IP address
    QHBoxLayout *lyout7 = new QHBoxLayout;
    ipAddressLabel = new QLabel(this);


    textIPAddress = new QLabel(this);
    textIPAddress->setText(QString("None"));
    lyout7->addWidget(ipAddressLabel,1);
    lyout7->addWidget(textIPAddress,2);

    // Connect and Disconnect button
    QHBoxLayout *lyout8 = new QHBoxLayout;
    connectButton = new QPushButton(tr("Connect"),this);

#ifdef DEVICE_EVB
int wifi_button_width = 145;
int wifi_button_height = 45;
#else
int wifi_button_width = 90;
int wifi_button_height = 30;
#endif
    connectButton->setFixedSize(wifi_button_width,wifi_button_height);

    disconnectButton = new QPushButton(tr("Disconnect"),this);
    disconnectButton->setFixedSize(wifi_button_width,wifi_button_height);
    lyout8->addStretch(0);
    lyout8->addWidget(connectButton);
    lyout8->addSpacing(30);
    lyout8->addWidget(disconnectButton);
    lyout8->addStretch(0);

    vmainlyout->addLayout(lyout2);
    vmainlyout->addLayout(lyout3);
    vmainlyout->addLayout(lyout4);
    vmainlyout->addLayout(lyout5);
    vmainlyout->addLayout(lyout6);
    vmainlyout->addLayout(lyout7);
    vmainlyout->addSpacing(40);
    vmainlyout->addLayout(lyout8);
    vmainlyout->addStretch(0);
    vmainlyout->setContentsMargins(10,10,10,10);

    setLayout(vmainlyout);
}
