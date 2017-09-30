#ifndef RIGHTSTACKEDWIDGETS1_H
#define RIGHTSTACKEDWIDGETS1_H

#include <basewidget.h>
#include "bluetoothscannerwidgets.h"

class BluetoothWidgets:public BaseWidget
{
    Q_OBJECT
public:
    BluetoothWidgets(QWidget *parent);
    ~BluetoothWidgets();
private:
    BluetoothScannerWidgets *m_scannerWidget;

    void initData();
    void initLayout();
};

#endif // RIGHTSTACKEDWIDGETS1_H
