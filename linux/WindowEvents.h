#pragma once

#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtCore/QSharedPointer>

#include "Queue.h"
#include "ActiveWindow.h"

class WindowEvents : public QThread
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

    void    stop() { m_do_run = false; }

signals:
    void    signal_window_event(Action, WindowData);
    void    signal_thread_error(QByteArray);

protected:
    void    run() override;

private slots:
    void    slot_active_window(QString);
    void    slot_thread_error(QByteArray);
    void    slot_process();

private:
    bool    m_do_run{true};

    ActiveWindowPtr m_active_window;

    StrQueuePtr m_queue;

    QTimer      m_timer;

    WindowData  m_last_win_data;
};

extern QMap<WindowEvents::Action, QString> state_name;

using WindowEventsPtr = QSharedPointer<WindowEvents>;

Q_DECLARE_METATYPE(WindowEvents::Action);
Q_DECLARE_METATYPE(WindowEvents::WindowData);
