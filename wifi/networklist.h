#ifndef NETWORKLIST_H
#define NETWORKLIST_H

#include <QObject>
#include <QListWidget>

class NetworkList:public QListWidget
{
    Q_OBJECT
public:
    NetworkList(QWidget *parent);
private:
    void init();

};

#endif // NETWORKLIST_H
