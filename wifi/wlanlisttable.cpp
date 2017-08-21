#include "wlanlisttable.h"

#include <QHeaderView>
#include <QScrollBar>
#include "global_value.h"

#ifdef DEVICE_EVB
int wlan_item_height = 50;
int next_step_move_distance = 100;
#else
int wlan_item_height = 30;
int next_step_move_distance = 30;
#endif

WlanListTable::WlanListTable(QWidget *parent):BaseTableWidget(parent,next_step_move_distance)
{
    init();
    initConnection();
}

void WlanListTable::init()
{
    m_previousFousedRow = -1;
    m_playingItemRow = -1;

    insertColumn(0);
    insertColumn(1);
    insertColumn(2);
    insertColumn(3);

    // Set item height.
    verticalHeader()->setDefaultSectionSize(wlan_item_height);

#ifdef DEVICE_EVB
    QFont font = this->font();
    font.setPixelSize(font_size-5);
    setFont(font);
#endif
}

void WlanListTable::initConnection()
{
    connect(this,SIGNAL(cellEntered(int,int)),this,SLOT(slot_cellEnter(int,int)));
}

void WlanListTable::slot_cellEnter(int ,int )
{
    //    QTableWidgetItem *it = item(m_previousFousedRow,0);
    //    if(it != NULL)
    //    {
    //        if(m_playingItemRow!=m_previousFousedRow){
    //            setRowTextColor(m_previousFousedRow,QColor(255,255,255));
    //        }
    //    }
    //    it = item(row, column);
    //    if(it != NULL)
    //    {
    //        setRowTextColor(row,QColor(26,158,255));
    //    }
    //    m_previousFousedRow = row;
}

void WlanListTable::setRowTextColor(int, const QColor&)const
{
    //    for(int col=0; col<columnCount(); col++)
    //    {
    //        QTableWidgetItem *it = item(row, col);
    //        it->setTextColor(color);
    //    }
}

void WlanListTable::leaveEvent(QEvent *event)
{
    QTableWidget::leaveEvent(event);
    slot_cellEnter(-1, -1);
}

void WlanListTable::resizeEvent(QResizeEvent *event)
{
#ifdef DEVICE_EVB
    QTableWidget::resizeEvent(event);
    horizontalHeader()->resizeSection(0,width()-470);
    horizontalHeader()->resizeSection(1,210);
    horizontalHeader()->resizeSection(2,60);
    horizontalHeader()->resizeSection(3,200);
#else
    QTableWidget::resizeEvent(event);
    horizontalHeader()->resizeSection(0,width()-330);
    horizontalHeader()->resizeSection(1,160);
    horizontalHeader()->resizeSection(2,20);
    horizontalHeader()->resizeSection(3,150);
#endif
}

