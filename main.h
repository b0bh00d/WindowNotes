#pragma once

#include <QtWidgets/QApplication>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMessageBox>
#ifdef QT_WIN
#include <QtCore/QAbstractNativeEventFilter>
#endif

#include "mainwindow.h"

#ifdef QT_WIN
class MyWinEventFilter : public QAbstractNativeEventFilter
{
public:
    MyWinEventFilter() : QAbstractNativeEventFilter() {}

    virtual bool nativeEventFilter(const QByteArray & /*eventType*/, void *message, long *result) Q_DECL_OVERRIDE
    {
        extern MainWindow* main_window;
        if(main_window)
            return main_window->process_win_msg((MSG*)message, result);
        return false;
    }
};
#endif

class MyApplication : public QApplication
{
public:
    MyApplication(int argc, char*argv[]) : QApplication(argc, argv)
    {
#ifdef QT_WIN
        installNativeEventFilter((my_filter = new MyWinEventFilter()));
#endif
    }

    ~MyApplication()
    {
#ifdef QT_WIN
        removeNativeEventFilter(my_filter);
        delete my_filter;
#endif
    }

private:
#ifdef QT_WIN
    MyWinEventFilter* my_filter{nullptr};
#endif
};
