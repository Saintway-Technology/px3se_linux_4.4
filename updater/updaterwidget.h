#ifndef UPDATERWIDGET_H
#define UPDATERWIDGET_H

#include <QWidget>
#include <QFileInfo>

#include "updater_recovery_start.h"

namespace Ui {
class UpdaterWidget;
}

class UpdaterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UpdaterWidget(QWidget *parent = 0);
    ~UpdaterWidget();

public slots:
    void on_m_updatePushButton_clicked();
    void retranslateUi();

private:
    Ui::UpdaterWidget *ui;
    QFileInfo *file;
    UpdaterInfo fwinfo;
};

#endif // UPDATERWIDGET_H
