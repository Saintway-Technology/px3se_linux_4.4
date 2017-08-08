#include "networklist.h"

#include <QHeaderView>
#include <QScrollBar>
#include "global_value.h"

#ifdef DEVICE_EVB
int network_item_height = 50;
#else
int network_item_height = 30;
#endif

NetworkList::NetworkList(QWidget *parent):QListWidget(parent)
{
    setStyleSheet("QListWidget{background:rgb(27,29,36);color:white}");

    init();
}

void NetworkList::init()
{
    setMouseTracking(true);
    setFrameShadow(QFrame::Plain);
    setFrameShape(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //   setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Expanding);

    setEditTriggers(QTableWidget::NoEditTriggers);
    setSelectionBehavior (QAbstractItemView::SelectRows);
    setSelectionMode (QAbstractItemView::SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setAcceptDrops(true);

    verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;margin: 0px 2px 0px 0px;}"
                                       "QScrollBar::handle{background:rgb(217,217,217);border-radius:4px;}"
                                       "QScrollBar::handle:hover{background: rgb(191,191,191);}"
                                       "QScrollBar::add-line:vertical{border:1px rgb(230,230,230);height: 1px;}"
                                       "QScrollBar::sub-line:vertical{border:1px rgb(230,230,230);height: 1px;}"
                                       "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background:transparent;}");

#ifdef DEVICE_EVB
    QFont font = this->font();
    font.setPixelSize(font_size-5);
    setFont(font);
#endif
}



