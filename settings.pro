#-------------------------------------------------
#
# Project created by QtCreator 2017-06-30T10:13:11
#
#-------------------------------------------------

QT       += core gui bluetooth network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = settings

TEMPLATE = app

CONFIG+= qt warn_on release



DEFINES += CONFIG_CTRL_IFACE

# 3399Linux、Big DPI
DEFINES += DEVICE_EVB

win32 {
  LIBS += -lws2_32 -static
  DEFINES += CONFIG_NATIVE_WINDOWS CONFIG_CTRL_IFACE_NAMED_PIPE
  SOURCES += setting/wpa_supplicant/src/utils/os_win32.c
} else:win32-g++ {
  # cross compilation to win32
  LIBS += -lws2_32 -static -mwindows
  DEFINES += CONFIG_NATIVE_WINDOWS CONFIG_CTRL_IFACE_NAMED_PIPE
  SOURCES += setting/wpa_supplicant/src/utils/os_win32.c
} else:win32-x-g++ {
  # cross compilation to win32
  LIBS += -lws2_32 -static -mwindows
  DEFINES += CONFIG_NATIVE_WINDOWS CONFIG_CTRL_IFACE_NAMED_PIPE
  DEFINES += _X86_
  SOURCES += setting/wpa_supplicant/src/utils/os_win32.c
} else {
  DEFINES += CONFIG_CTRL_IFACE_UNIX
  SOURCES += setting/wpa_supplicant/src/utils/os_unix.c
}



INCLUDEPATH +=$$PWD base
include(base/base.pri)

INCLUDEPATH +=$$PWD setting
include(setting/setting.pri)



SOURCES += main.cpp\
        mainwindow.cpp \
    global_value.cpp

HEADERS  += mainwindow.h \
    global_value.h


FORMS = networkconfig.ui \
        btscanner.ui \
    brightness/brightness.ui \
    calendar/widget.ui \
    volumn/volumnwidget.ui

RESOURCES += \
    res_main.qrc \
    res_setting.qrc

