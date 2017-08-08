#include "mainwindow.h"
#include <QApplication>
#include "global_value.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyleSheet("QLabel{color:white;}"
                      "QScrollBar{background:transparent; width: 10px;margin: 0px 2px 0px 0px;}"
                      "QScrollBar::handle{background:rgb(217,217,217);border-radius:4px;}"
                      "QScrollBar::handle:hover{background: rgb(191,191,191);}"
                      "QScrollBar::add-line:vertical{border:1px rgb(230,230,230);height: 1px;}"
                      "QScrollBar::sub-line:vertical{border:1px rgb(230,230,230);height: 1px;}"
                      "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background:transparent;}");
    // Change app font family and size to supprot all device.

    MainWindow w;
#ifdef DEVICE_EVB
    QFont appFont = app.font();
    appFont.setPixelSize(font_size);
    app.setFont(appFont);
    w.showMaximized();
#else
    w.showFullScreen();
#endif

    return app.exec();
}
