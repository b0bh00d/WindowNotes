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
    : QThread(parent)
{
    qRegisterMetaType<WindowEvents::Action>("Action");
    qRegisterMetaType<WindowEvents::WindowData>("WindowData");

    m_queue = StrQueuePtr(new Queue<QString>);

    m_active_window = ActiveWindowPtr(new ActiveWindow(m_queue));

    connect(m_active_window.data(), &ActiveWindow::signal_active_window, this, &WindowEvents::slot_active_window);
    connect(m_active_window.data(), &ActiveWindow::signal_error, this, &WindowEvents::slot_thread_error);
}

void WindowEvents::slot_active_window(QString win_id)
{
    m_queue->enqueue(win_id);
}

void WindowEvents::slot_thread_error(QByteArray output)
{
    emit signal_thread_error(output);
}

void WindowEvents::slot_process()
{
    QString new_id;

    if(!m_queue->empty())
        new_id = m_queue->dequeue();

    auto win_id = m_last_win_data.window_id;
    if(!new_id.isEmpty() && new_id.compare(m_last_win_data.window_id))
        win_id = new_id;

    if(!win_id.isEmpty())
    {
        QProcess xwininfo;
        xwininfo.setProgram("xwininfo");
        xwininfo.setArguments(QStringList() << "-id" << win_id);
        xwininfo.closeWriteChannel();

        xwininfo.start();
        xwininfo.waitForStarted();
        xwininfo.waitForFinished();
        auto output = xwininfo.readAllStandardOutput();

        QList<QByteArray> process_lines = output.split('\n');

        WindowData win_data;
        win_data.window_id = win_id;

        foreach(QByteArray line, process_lines)
        {
            QString line_ = line.trimmed();
            if(!line_.isEmpty())
            {
                if(line_.contains("xwininfo: Window id:"))
                {
                    // grab the window title
                    static auto r = QRegularExpression("\"(.+)\"$");
                    auto match = r.match(line_);
                    if(match.hasMatch())
                        win_data.title = match.captured(1);
                }
                else if(line_.contains("Absolute upper-left X"))
                {
                    static auto r = QRegularExpression(":\\s+(\\w+)$");
                    auto match = r.match(line_);
                    if(match.hasMatch())
                        win_data.abs_left = match.captured(1).toInt();
                }
                else if(line_.contains("Absolute upper-left Y"))
                {
                    static auto r = QRegularExpression(":\\s+(\\w+)$");
                    auto match = r.match(line_);
                    if(match.hasMatch())
                        win_data.abs_top = match.captured(1).toInt();
                }
                else if(line_.contains("Relative upper-left X"))
                {
                    static auto r = QRegularExpression(":\\s+(\\w+)$");
                    auto match = r.match(line_);
                    if(match.hasMatch())
                        win_data.rel_left = match.captured(1).toInt();
                }
                else if(line_.contains("Relative upper-left Y"))
                {
                    static auto r = QRegularExpression(":\\s+(\\w+)$");
                    auto match = r.match(line_);
                    if(match.hasMatch())
                        win_data.rel_top = match.captured(1).toInt();
                }
                else if(line_.contains("Width: "))
                {
                    static auto r = QRegularExpression(":\\s+(\\w+)$");
                    auto match = r.match(line_);
                    if(match.hasMatch())
                        win_data.width = match.captured(1).toInt();
                }
                else if(line_.contains("Height: "))
                {
                    static auto r = QRegularExpression(":\\s+(\\w+)$");
                    auto match = r.match(line_);
                    if(match.hasMatch())
                        win_data.height = match.captured(1).toInt();

                    // height is always the last line we need; don't
                    // waste cycles processing the rest
                    break;
                }
            }
        }

        // figure out the delta (if any) from the last read

        if(m_last_win_data.window_id.isEmpty() || win_data.window_id.compare(m_last_win_data.window_id))
            win_data.action = Action::Active;
        else if(win_data.title.compare(m_last_win_data.title))
            win_data.action = Action::Title;
        else if((win_data.width != m_last_win_data.width) || (win_data.height != m_last_win_data.height))
            win_data.action = Action::Resizing;
        else if((win_data.abs_top != m_last_win_data.abs_top) ||
            (win_data.abs_left != m_last_win_data.abs_left) ||
            (win_data.rel_top != m_last_win_data.rel_top) ||
            (win_data.rel_left != m_last_win_data.rel_left))
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
}

void WindowEvents::run()
{
    m_active_window->start();

    // m_timer.moveToThread(this);
    // m_timer.setInterval(300);
    // connect(&m_timer, &QTimer::timeout, this, &WindowEvents::slot_process);
    // m_timer.start();

    while(m_do_run)
    {
        QThread::msleep(300);
        slot_process();
    }

    m_active_window->stop();
}
