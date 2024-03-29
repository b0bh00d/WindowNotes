#include <QtCore/QDir>

#include <QtWidgets/QApplication>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMessageBox>

#ifdef QT_WIN
#include "main.h"
#endif

#include "mainwindow.h"

int main(int argc, char *argv[])
{
#ifdef QT_WIN
    MyApplication app(argc, argv);
    MyApplication::setEffectEnabled(Qt::UI_FadeTooltip, false);
#endif
#ifdef QT_LINUX
    QApplication app(argc, argv);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip, false);
#endif

    if(!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(0,
                              QObject::tr("WindowsNotes"),
                              QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }

    MainWindow window;
    window.hide();

#ifdef QT_LINUX
    // make sure the audio system can find the sound files
    auto items = QString(argv[0]).split('/');
    items.pop_back();
    auto path = items.join('/');
    QDir::setCurrent(path);
#endif

    return app.exec();
}
