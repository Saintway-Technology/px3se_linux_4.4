HEADERS += \
    $$PWD/wifiwidgets.h \
    $$PWD/wlanlisttable.h \
    $$PWD/netconfigdialog.h \
    $$PWD/switchwidget.h \
    $$PWD/wifiutil.h \
    $$PWD/networklist.h \
    $$PWD/tabcurrentstatus.h

SOURCES += \
    $$PWD/wifiwidgets.cpp \
    $$PWD/wlanlisttable.cpp \
    $$PWD/netconfigdialog.cpp \
    $$PWD/switchwidget.cpp \
    $$PWD/wifiutil.cpp \
    $$PWD/networklist.cpp \
    $$PWD/tabcurrentstatus.cpp

INCLUDEPATH +=$$PWD wpa_supplicant
include (wpa_supplicant/wpa_supplicant.pri)

