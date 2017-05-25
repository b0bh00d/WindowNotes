#pragma once

#include <QtWidgets/QApplication>
#include <QtCore/QAbstractNativeEventFilter>

#include "mainwindow.h"

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

class MyApplication : public QApplication
{
public:
    MyApplication(int argc, char*argv[]) : QApplication(argc, argv)
    {
        installNativeEventFilter((my_filter = new MyWinEventFilter()));
    }

    ~MyApplication()
    {
        removeNativeEventFilter(my_filter);
        delete my_filter;
    }

private:
    MyWinEventFilter* my_filter;
};
