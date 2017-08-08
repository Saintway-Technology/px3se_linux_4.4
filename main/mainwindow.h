#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "base/basewindow.h"
#include "settingwidgets.h"

class SettingWidgets;
/**
 * The main window of application.
 * Used for global control.such as keypress response、initial interface etc.
 */
class MainWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent *event);
private:
    SettingWidgets *m_setttingsWid;

    void initLayout();
    void initConnection();

private slots:
    void slot_appQuit();
    void slot_standby();
};



#endif // MAINWINDOW_H
