#ifndef WLANLISTTABLE_H
#define WLANLISTTABLE_H

#include <QObject>
#include <QTableWidget>

class WlanListTable:public QTableWidget
{
    Q_OBJECT
public:
    WlanListTable(QWidget *parent);
private:
    int m_previousFousedRow;
    int m_playingItemRow;

    void init();
    void initConnection();
    void setPlayingItemIndex(int index){m_playingItemRow = index;}
    void setRowTextColor(int row,const QColor &color)const;
protected:
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent*);

private slots:
    void slot_cellEnter(int,int);
};

#endif // WLANLISTTABLE_H
