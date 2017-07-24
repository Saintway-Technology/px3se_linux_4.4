#ifndef UPDATERWIDGET_H
#define UPDATERWIDGET_H

#include <QWidget>
#include <QFileInfo>

namespace Ui {
class UpdaterWidget;
}

class UpdaterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UpdaterWidget(QWidget *parent = 0);
    ~UpdaterWidget();

private slots:
    void on_m_selectFilePushButton_clicked();

    void on_m_updatePushButton_clicked();

private:
    Ui::UpdaterWidget *ui;
    QFileInfo *file;
};

#endif // UPDATERWIDGET_H
