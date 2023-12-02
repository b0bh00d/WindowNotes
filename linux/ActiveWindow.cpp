#include <QtCore/QTimer>
#include <QtCore/QStringList>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

#include "ActiveWindow.h"

//----------------------------------------------------------------------------
// xprop monitor thread

ActiveWindow::ActiveWindow(StrQueuePtr q, QObject* parent)
    : QThread(parent),
      m_queue(q)
{}

void ActiveWindow::slot_read_error()
{
    auto lines = m_xprop_spy->readAllStandardError();
    emit signal_error(lines);
    m_do_run = false; // stop the thread
}

void ActiveWindow::slot_read_output()
{
    auto output = m_xprop_spy->readAllStandardOutput();

    QList<QByteArray> process_lines = output.split('\n');

    foreach(QByteArray line, process_lines)
    {
        QString line_ = line.trimmed();
        if(line_.contains("_NET_ACTIVE_WINDOW"))
        {
            static auto r = QRegularExpression("window\\s+id\\s+\\#\\s+([\\wa-fx]+)$");
            auto match = r.match(line_);
            if(match.hasMatch())
            {
                emit signal_active_window(match.captured(1));
            }
        }
    }
}

void ActiveWindow::run()
{
    m_xprop_spy = ProcessPtr(new QProcess());
    m_xprop_spy->setProgram("xprop");
    m_xprop_spy->setArguments(QStringList() << "-root" << "-spy");
    m_xprop_spy->closeWriteChannel();
    connect(m_xprop_spy.data(), &QProcess::readyReadStandardError, this, &ActiveWindow::slot_read_error);
    connect(m_xprop_spy.data(), &QProcess::readyReadStandardOutput, this, &ActiveWindow::slot_read_output);

    m_xprop_spy->start();
    m_xprop_spy->waitForStarted();

    while(m_xprop_spy->state() == QProcess::Running)
        m_xprop_spy->waitForFinished(100);

    if(!m_do_run)
        m_xprop_spy->terminate();

    m_xprop_spy->waitForFinished();
}
