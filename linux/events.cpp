// https://github.com/ickyicky/window-calls
// https://www.codeproject.com/Articles/5271677/How-to-Create-A-GNOME-Extension
// https://www.reddit.com/r/linuxquestions/comments/8dhxnq/sample_code_for_the_c_gnome_dbus_bindings/
// https://github.com/makercrew/dbus-sample#operator-well-could-you-help-me-place-this-call

// https://docs.gtk.org/gio/
// https://gjs.guide/extensions/
// https://github.com/julio641742/gnome-shell-extension-reference/blob/master/REFERENCE.md

// https://www.qtcentre.org/threads/37066-How-do-i-connect-to-QDbus-signal-in-QTDbus

#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>
#include <QtMultimedia/QSound>

#include "mainwindow.h"
#include "WindowEventsSingleton.h"

extern MainWindow* main_window;

bool MainWindow::os_events_init()
{
    return true;
}

void MainWindow::os_events_cleanup()
{
    m_window_events->stop();
}

bool MainWindow::os_locate_instance()
{
    static auto title_regex = QRegularExpression("^WindowNotes .+Bob Hood$");

    QProcess xwininfo;
    xwininfo.setProgram("xwininfo");
    xwininfo.setArguments(QStringList() << "-tree" << "-root");
    xwininfo.closeWriteChannel();

    xwininfo.start();
    xwininfo.waitForStarted();
    xwininfo.waitForFinished();
    auto output = xwininfo.readAllStandardOutput();

    QList<QByteArray> process_lines = output.split('\n');

    foreach(QByteArray line, process_lines)
    {
        QString line_ = line.trimmed();

        auto match = title_regex.match(line_);
        if(match.hasMatch())
            return true;
    }

    return false;
}

void MainWindow::os_play_sound(int sound)
{
    // https://gist.github.com/ghedo/963382

    QString s(sound_files[sound]);
    if(s.isEmpty())
        s = QString(":/sounds/%1.wav").arg(sound + 1);

    QSound::play(s);
}

void MainWindow::os_set_startup()
{
}

void MainWindow::os_set_hooks()
{
    // start the WindowEvents thread
    m_window_events = WindowEventsSingleton::instance()->winEvents();//WindowEventsPtr(new WindowEvents());

    connect(m_window_events.data(), &WindowEvents::signal_window_event, this, &MainWindow::slot_window_event);
    connect(m_window_events.data(), &WindowEvents::signal_thread_error, this, &MainWindow::slot_thread_error);

    m_window_events->start();
}

void MainWindow::slot_window_event(WindowEvents::Action action, WindowEvents::WindowData win_data)
{
    auto id = win_data.window_id;

    switch(action)
    {
        case WindowEvents::Action::Active:
            focus_window_handle == id ? hide_notetabs() : delete_notetabs();

            // the active window has changed (and so has the title)
            focus_window_handle = id;
            focus_window_rect = QRect(win_data.left, win_data.top, win_data.width, win_data.height);

            // fall-through here
            [[fallthrough]];

        case WindowEvents::Action::Title:
            // the title of the active window has changed
            focus_window_title = win_data.title;

            current_context = locate_context();

            arrange_notetabs();
            display_notetabs();

            break;

        case WindowEvents::Action::MoveStarted:
            // the window has started moving
        case WindowEvents::Action::ResizeStarted:
            // the window has started resizing
            focus_window_handle == id ? hide_notetabs() : delete_notetabs();

            if(id != focus_window_handle)
            {
                focus_window_handle = id;
                focus_window_title.clear();
            }
            break;

        case WindowEvents::Action::Moving:
        case WindowEvents::Action::Resizing:
            break;
        case WindowEvents::Action::MoveEnded:
            // the window has stopped moving
        case WindowEvents::Action::ResizeEnded:
            // the window has finished resizing
            if(id != focus_window_handle)
            {
                focus_window_handle = id;
                focus_window_title = win_data.title;
            }

            // get the focus window's position and size
            focus_window_rect = QRect(win_data.left, win_data.top, win_data.width, win_data.height);

            current_context = locate_context();

            arrange_notetabs();
            display_notetabs();

            break;

        case WindowEvents::Action::None:
            focus_window_title.clear();
            current_context = nullptr;
            delete_notetabs();

            // fall-through here
            [[fallthrough]];

        default:
            break;
    }
}

void MainWindow::slot_thread_error(QByteArray output)
{
    QMessageBox::critical(this, "Thread Error", output);
}
