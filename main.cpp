#include "camera.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(recursos);

    QApplication app(argc, argv);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("Systray"),
                              QObject::tr("I couldn't detect any system tray "
                                          "on this system."));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    Camera camera;
    camera.show();
    return app.exec();
}
