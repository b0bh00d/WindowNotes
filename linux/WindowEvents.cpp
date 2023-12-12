#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include <QtCore/QMap>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

#include "WindowEvents.h"

QMap<WindowEvents::Action, QString> state_name
{
        {WindowEvents::Action::None, "None"},
        {WindowEvents::Action::Active, "Active"},
        {WindowEvents::Action::Title, "Title"},
        {WindowEvents::Action::MoveStarted, "MoveStarted"},
        {WindowEvents::Action::Moving, "Moving"},
        {WindowEvents::Action::MoveEnded, "MoveEnded"},
        {WindowEvents::Action::ResizeStarted, "ResizeStarted"},
        {WindowEvents::Action::Resizing, "Resizing"},
        {WindowEvents::Action::ResizeEnded, "ResizeEnded"},
};

WindowEvents::WindowEvents(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<WindowEvents::Action>("Action");
    qRegisterMetaType<WindowEvents::WindowData>("WindowData");

    m_qdbus_available = QDBusConnection::sessionBus().isConnected();
    if(m_qdbus_available)
    {
// QDBusConnection::sessionBus().connect("org.gnome.SessionManager", "/org/gnome/SessionManager/Presence", "org.gnome.SessionManager.Presence" ,"StatusChanged", this, SLOT(MySlot(uint)));

        // https://doc.qt.io/qt-5/qdbusinterface.html
        m_dbus_i = InterfacePtr(new QDBusInterface("org.gnome.Shell",
                                                   "/org/gnome/Shell/Extensions/Windows",
                                                   "org.gnome.Shell.Extensions.Windows"));
        // m_service_watcher = new QDBusServiceWatcher(SERVICE_NAME, QDBusConnection::sessionBus(),
        //                                QDBusServiceWatcher::WatchForRegistration);
    }
}

void WindowEvents::slot_active_window(QString win_id)
{
    m_queue.enqueue(win_id);
}

void WindowEvents::slot_thread_error(QByteArray output)
{
    emit signal_thread_error(output);
}

void WindowEvents::slot_process()
{
    QByteArray output;

    if(m_qdbus_available)
    {
        QDBusReply<QString> reply = m_dbus_i->call(QLatin1String("FocusInfo"));
        output = reply.value().toUtf8();
    }
    else
    {
        QProcess gdbus;
        gdbus.setProgram("gdbus");
        gdbus.setArguments(QStringList()
            << "call"
            << "--session"
            << "--dest"<< "org.gnome.Shell"
            << "--object-path" << "/org/gnome/Shell/Extensions/Windows"
            << "--method" << "org.gnome.Shell.Extensions.Windows.FocusInfo"
            );
        gdbus.closeWriteChannel();

        gdbus.start();
        gdbus.waitForStarted();
        gdbus.waitForFinished();

        output = gdbus.readAllStandardOutput();

        // clean it up
        output.remove(0, 2);
        output.chop(4);
    }

    QJsonDocument info(QJsonDocument::fromJson(output));
    auto new_id = static_cast<quint32>(info["id"].toDouble());

    // quint32 new_id{static_cast<quint32>(info["id"])};

    auto win_id = m_last_win_data.window_id;
    if(new_id)
    {
        if(new_id != m_last_win_data.window_id)
            win_id = new_id;
    }
    else
        win_id = new_id;

    WindowData win_data;

    if(win_id)
    {
        win_data.window_id = win_id;
        win_data.title = info["title"].toString();
        win_data.top = static_cast<quint32>(info["y"].toDouble());
        win_data.left = static_cast<quint32>(info["x"].toDouble());
        win_data.width = static_cast<quint32>(info["width"].toDouble());
        win_data.height = static_cast<quint32>(info["height"].toDouble());

        // figure out the delta (if any) from the last read

        if(!m_last_win_data.window_id || win_data.window_id != m_last_win_data.window_id)
            win_data.action = Action::Active;
        else if(win_data.title.compare(m_last_win_data.title))
            win_data.action = Action::Title;
        else if((win_data.width != m_last_win_data.width) || (win_data.height != m_last_win_data.height))
            win_data.action = Action::Resizing;
        else if((win_data.top != m_last_win_data.top) || (win_data.left != m_last_win_data.left))
            win_data.action = Action::Moving;

        if(win_data.action == Action::Moving)
        {
            if(m_last_win_data.action != Action::Moving)
                emit signal_window_event(Action::MoveStarted, win_data);
        }
        else if(win_data.action == Action::Resizing)
        {
            if(m_last_win_data.action != Action::Resizing)
                emit signal_window_event(Action::ResizeStarted, win_data);
        }
        else if(win_data.action == Action::None)
        {
            if(m_last_win_data.action == Action::Moving)
                emit signal_window_event(Action::MoveEnded, win_data);
            else if(m_last_win_data.action == Action::Resizing)
                emit signal_window_event(Action::ResizeEnded, win_data);
        }
        else
            emit signal_window_event(win_data.action, win_data);

        m_last_win_data = win_data;
    }
    else if(m_last_win_data.window_id)
    {
        // make sure we clean up if there are no more windows on the desktop
        emit signal_window_event(Action::None, win_data);
        m_last_win_data = win_data;
    }
}

void WindowEvents::start()
{
    if(!m_timer.isActive())
    {
        m_timer.setInterval(100);
        connect(&m_timer, &QTimer::timeout, this, &WindowEvents::slot_process);
        m_timer.start();
    }
}

void WindowEvents::stop()
{
    if(m_timer.isActive())
    {
        disconnect(&m_timer, &QTimer::timeout, this, &WindowEvents::slot_process);
        m_timer.stop();
    }
}
