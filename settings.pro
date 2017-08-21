#-------------------------------------------------
#
# Project created by QtCreator 2017-06-30T10:13:11
#
#-------------------------------------------------

QT += bluetooth network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = settings
TEMPLATE = app

CONFIG+= qt warn_on release

DEFINES += CONFIG_CTRL_IFACE

# 3399Linux、Big DPI
DEFINES += DEVICE_EVB

LIBS += -lasound
QMAKE_CXXFLAGS = -fpermissive

win32 {
  LIBS += -lws2_32 -static
  DEFINES += CONFIG_NATIVE_WINDOWS CONFIG_CTRL_IFACE_NAMED_PIPE
  SOURCES += wifi/wpa_supplicant/src/utils/os_win32.c
} else:win32-g++ {
  # Cross compilation to win32
  LIBS += -lws2_32 -static -mwindows
  DEFINES += CONFIG_NATIVE_WINDOWS CONFIG_CTRL_IFACE_NAMED_PIPE
  SOURCES += wifi/wpa_supplicant/src/utils/os_win32.c
} else:win32-x-g++ {
  # Cross compilation to win32
  LIBS += -lws2_32 -static -mwindows
  DEFINES += CONFIG_NATIVE_WINDOWS CONFIG_CTRL_IFACE_NAMED_PIPE
  DEFINES += _X86_
  SOURCES += wifi/wpa_supplicant/src/utils/os_win32.c
} else {
  DEFINES += CONFIG_CTRL_IFACE_UNIX
  SOURCES += wifi/wpa_supplicant/src/utils/os_unix.c
}

INCLUDEPATH +=$$PWD main
include(main/main.pri)

INCLUDEPATH +=$$PWD base
include(base/base.pri)

INCLUDEPATH +=$$PWD top
include(top/top.pri)

INCLUDEPATH +=$$PWD middle
include(middle/middle.pri)

INCLUDEPATH +=$$PWD wifi
include(wifi/wifi.pri)

INCLUDEPATH +=$$PWD bluetooth
include(bluetooth/bluetooth.pri)

INCLUDEPATH +=$$PWD brightness
include(brightness/brightness.pri)

INCLUDEPATH +=$$PWD calendar
include (calendar/calendar.pri)

INCLUDEPATH +=$$PWD volume
include (volume/volume.pri)

INCLUDEPATH +=$$PWD updater
include (updater/updater.pri)

FORMS = wifi/networkconfig.ui \
        bluetooth/btscanner.ui \
        brightness/brightness.ui \
        calendar/widget.ui \
        volume/volumnwidget.ui \
        updater/updaterwidget.ui

RESOURCES += \
    res_main.qrc \
    res_setting.qrc

