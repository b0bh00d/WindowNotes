#pragma once

#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QProcess>
#include <QtCore/QSharedPointer>

#include "Queue.h"

// Threading class to launch and interact with a long-running xprop instance
class ActiveWindow : public QThread
{
    Q_OBJECT

public:
    ActiveWindow(StrQueuePtr q, QObject* parent = nullptr);

    void            stop() { m_do_run = false; }
    const QString&  get_error_output() { return m_error_output; }

signals:
    void    signal_error(QByteArray);
    void    signal_active_window(QString);

protected:
    void    run() override;

private slots:
    void    slot_read_error();
    void    slot_read_output();

private:
    using ProcessPtr = QSharedPointer<QProcess>;

    bool                m_do_run{true};

    StrQueuePtr         m_queue;
    ProcessPtr          m_xprop_spy;
    QString             m_last_window_id;
    QString             m_error_output;
    float               m_poll{0.3};
};

using ActiveWindowPtr = QSharedPointer<ActiveWindow>;
