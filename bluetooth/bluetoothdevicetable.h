#ifndef BLUETOOTHDEVICETABLE_H
#define BLUETOOTHDEVICETABLE_H

#include "base/basetablewidget.h"

class BluetoothStateItem:public QTableWidgetItem
{
public:
    BluetoothStateItem(const QString &text);
protected:
    virtual bool operator<(const QTableWidgetItem &other) const;
};

class BluetoothDeviceTable:public BaseTableWidget
{
public:
    BluetoothDeviceTable(QWidget *parent);

    void insertIntoTable(const QString &name,const QString &address,const QString &state);
    void clearTable();
    void sortTable();
    void updateItemState(int row,const QString& state);
    QString getItemName(int row);
    QString getItemAddress(int row);

private:
    void init();

protected:
    void resizeEvent(QResizeEvent*);
};

#endif // BLUETOOTHDEVICETABLE_H
