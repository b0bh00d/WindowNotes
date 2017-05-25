#include "main.h"

int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);

    if(!QSystemTrayIcon::isSystemTrayAvailable())
    {
        QMessageBox::critical(0,
                              QObject::tr("WindowsNotes"),
                              QObject::tr("I couldn't detect any system tray on this system."));
        return 1;
    }

    MyApplication::setEffectEnabled(Qt::UI_FadeTooltip, false);

    MainWindow window;
    window.hide();

    return app.exec();
}
