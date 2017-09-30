#include "bluetoothdevicetable.h"
#include <QHeaderView>

#ifdef DEVICE_EVB
int move_distance_next_step = 300;
int bluetooth_item_height = 65;
#else
int move_distance_next_step = 100;
int bluetooth_item_height = 35;
#endif

BluetoothStateItem::BluetoothStateItem(const QString &text):QTableWidgetItem(text)
{
}

bool BluetoothStateItem::operator <(const QTableWidgetItem &other) const
{
    if(this->text() == "Connected"){
        return false;
    }else if(this->text() == "Paired"){
        if(other.text() == "Connected"){
            return true;
        }else{
            return false;
        }
    }
    return QTableWidgetItem::operator <(other);
}

BluetoothDeviceTable::BluetoothDeviceTable(QWidget *parent):BaseTableWidget(parent,move_distance_next_step)
{
    init();
}

void BluetoothDeviceTable::init()
{
    insertColumn(0);
    insertColumn(1);
    insertColumn(2);
    insertColumn(3);
    insertColumn(4);

    verticalHeader()->setDefaultSectionSize(bluetooth_item_height);
}

void BluetoothDeviceTable::insertIntoTable(const QString &name,const QString &address,const QString &state)
{
    int rowcount= rowCount();
    insertRow(rowcount);

    setItem(rowcount,1, new QTableWidgetItem(QIcon(QPixmap(":/image/setting/bluetooth_item_icon.png")),""));
    setItem(rowcount,2, new QTableWidgetItem(name));
    setItem(rowcount,3, new QTableWidgetItem(address));
    setItem(rowcount,4, new BluetoothStateItem(state));

    item(rowcount,2)->setTextAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    item(rowcount,3)->setTextAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    item(rowcount,4)->setTextAlignment(Qt::AlignVCenter|Qt::AlignLeft);

    sortTable();
}

void BluetoothDeviceTable::sortTable()
{
    sortItems(4,Qt::DescendingOrder);
}

void BluetoothDeviceTable::clearTable()
{
    for(int i=rowCount();i>0;i--){
        removeRow(0);
    }
}

void BluetoothDeviceTable::updateItemState(int row, const QString &state)
{
    item(row,4)->setText(state);
}

QString BluetoothDeviceTable::getItemName(int row)
{
    return item(row,2)->text();
}

QString BluetoothDeviceTable::getItemAddress(int row)
{
    return item(row,3)->text();
}

void BluetoothDeviceTable::resizeEvent(QResizeEvent *event)
{
#ifdef DEVICE_EVB
    horizontalHeader()->resizeSection(0,20);
    horizontalHeader()->resizeSection(1,50);
    horizontalHeader()->resizeSection(2,this->width()-520);
    horizontalHeader()->resizeSection(3,300);
    horizontalHeader()->resizeSection(4,150);
#else
    horizontalHeader()->resizeSection(0,10);
    horizontalHeader()->resizeSection(1,20);
    horizontalHeader()->resizeSection(2,this->width()-370);
    horizontalHeader()->resizeSection(3,220);
    horizontalHeader()->resizeSection(4,120);
#endif
    QTableWidget::resizeEvent(event);
}
