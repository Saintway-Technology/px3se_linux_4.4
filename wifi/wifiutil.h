#ifndef WIFIUTIL_H
#define WIFIUTIL_H


class WifiUtil
{
public:
    static bool console_run(const char *cmdline);

    static int is_supplicant_running();
    static int is_hostapd_running();
    static int creat_supplicant_file();
    static int creat_hostapd_file(const char* name, const char* password);
    static int wifi_start_supplicant();
    static int wifi_stop_supplicant();
    static int wifi_start_hostapd();
    static int wifi_stop_hostapd();
    static void lanStateChanhe(bool state);
    static void getIPAddress();
};

#endif // WIFIUTIL_H
