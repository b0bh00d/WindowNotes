#pragma once

#include <QtCore/QMap>
#include <QtCore/QQueue>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtCore/QSharedPointer>

#include "ActiveWindow.h"

class WindowEvents : public QObject
{
    Q_OBJECT

public:
    enum class Action : quint32 {
        None,
        Active,
        Title,
        MoveStarted,
        Moving,
        MoveEnded,
        ResizeStarted,
        Resizing,
        ResizeEnded,
    };

    // POD struct to hold info about the currently active window
    struct WindowData
    {
        Action action{Action::None};
        QString window_id;
        QString title;
        qint32 abs_top{0};
        qint32 abs_left{0};
        qint32 rel_top{0};
        qint32 rel_left{0};
        qint32 width{0};
        qint32 height{0};
    };

    explicit WindowEvents(QObject *parent = nullptr);

    void    start();
    void    stop();

signals:
    void    signal_window_event(Action, WindowData);
    void    signal_thread_error(QByteArray);

private slots:
    void    slot_active_window(QString);
    void    slot_thread_error(QByteArray);
    void    slot_process();

private:
    ActiveWindowPtr m_active_window;

    QQueue<QString> m_queue;

    QTimer      m_timer;

    WindowData  m_last_win_data;
};

extern QMap<WindowEvents::Action, QString> state_name;

using WindowEventsPtr = QSharedPointer<WindowEvents>;

Q_DECLARE_METATYPE(WindowEvents::Action);
Q_DECLARE_METATYPE(WindowEvents::WindowData);
