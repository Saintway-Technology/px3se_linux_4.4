#ifndef WpaManager_H
#define WpaManager_H

#include <QSystemTrayIcon>
#include <QObject>
#include <QTimer>
#include <QSocketNotifier>
#include <wpa_supplicant/wpamsg.h>

class WpaManager : public QObject
{
    Q_OBJECT
public:
    static WpaManager* getInstance(QObject *parent);
    // Send request to wap supplicant servcice.
    virtual int ctrlRequest(const char *cmd, char *buf, size_t *buflen);
    virtual void triggerUpdate();
    virtual void editNetwork(const QString &sel);
    virtual void removeNetwork(const QString &sel);
    virtual void enableNetwork(const QString &sel);
    virtual void disableNetwork(const QString &sel);
    virtual int getNetworkDisabled(const QString &sel);
    void setBssFromScan(const QString &bssid);
    void addInterface();
    int openCtrlConnection(const char *ifname);
#ifndef QT_NO_SESSIONMANAGER
    void saveState();
    void updateScanResult();
#endif

public slots:
    virtual void updateStatus();
    virtual void updateNetworks();
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();
    virtual void scan();
    // virtual void eventHistory();
    virtual void ping();
    virtual void signalMeterUpdate();
    virtual void processCtrlReq(const char *req);
    // It used cooperate with 'receiveMsgs' for translateMessage DispatchMessage.
    virtual void processMsg(char *msg);
    virtual void receiveMsgs();

    virtual void connectB();
    virtual void disconnectB();
    // Selete network card.
    virtual void selectAdapter(const QString &sel);
    virtual void selectNetwork(const QString &sel);
    virtual void editSelectedNetwork();
    virtual void editListedNetwork();
    virtual void removeSelectedNetwork();
    virtual void removeListedNetwork();
    virtual void addNetwork();
    virtual void enableAllNetworks();
    virtual void disableAllNetworks();
    virtual void removeAllNetworks();
    virtual void saveConfig();
    virtual void updateNetworkDisabledStatus();
    virtual void enableListedNetwork(bool);
    virtual void disableListedNetwork(bool);
    virtual void showTrayStatus();
    virtual void peersDialog();
    virtual void tabChanged(int index);
    virtual void wpsPbc();
    virtual void wpsGeneratePin();
    virtual void wpsApPinChanged(const QString &text);
    virtual void wpsApPin();
#ifdef CONFIG_NATIVE_WINDOWS
    virtual void startService();
    virtual void stopService();
#endif /* CONFIG_NATIVE_WINDOWS */

private:
    static WpaManager *_instance;
    WpaManager(QObject *parent=0);

    bool networkMayHaveChanged;
    char *ctrl_iface;
    struct wpa_ctrl *ctrl_conn;
    QSocketNotifier *msgNotifier;
    QTimer *timer;
    int pingsToStatusUpdate;
    WpaMsgList msgs;
    char *ctrl_iface_dir;
    struct wpa_ctrl *monitor_conn;
    //    UserDataRequest *udr;

    QString wpaStateTranslate(char *state);
    bool ackTrayIcon;
    bool startInTray;
    bool quietMode;

    bool wpsRunning;

    QString bssFromScan;

    void stopWpsRun(bool success);

    QTimer *signalMeterTimer;
    int signalMeterInterval;

#ifdef CONFIG_NATIVE_WINDOWS
    bool serviceRunning();
#endif /* CONFIG_NATIVE_WINDOWS */
    // Identify whether has connected to wpa supplicant service.
    bool connectedToService;
};

#endif /* WpaManager_H */
