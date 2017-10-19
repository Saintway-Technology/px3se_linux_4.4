#ifndef TABCURRENTSTATUS_H
#define TABCURRENTSTATUS_H
#include <QLabel>
#include <QPushButton>
#include "basewidget.h"

class TabCurrentStatus:public BaseWidget
{
public:
    TabCurrentStatus(QWidget *parent);

public:
    QLabel *textLastMsg;
    QLabel *textAuthentication;
    QLabel *textEncryption;
    QLabel *textSSID;
    QLabel *textBSSID;
    QLabel *textIPAddress;
    QPushButton *connectButton;
    QPushButton *disconnectButton;

    QLabel *lastMessageLabel;
    QLabel *authenticationLabel ;
    QLabel *encryptionLabel;
    QLabel *ssidLabel;
    QLabel *bssidLabel;
    QLabel *ipAddressLabel;
};

#endif // TABCURRENTSTATUS_H
