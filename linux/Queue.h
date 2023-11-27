#pragma once

#include <QtCore/QQueue>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QSharedPointer>

template<typename T>
class Queue
{
public:
    explicit Queue() {}

    bool empty()
    {
        QMutexLocker lock(&m_mutex);
        return m_queue.empty();
    }

    int count()
    {
        QMutexLocker lock(&m_mutex);
        return m_queue.count();
    }

    void enqueue(const T& item)
    {
        QMutexLocker lock(&m_mutex);
        m_queue.enqueue(item);
    }

    T dequeue()
    {
        QMutexLocker lock(&m_mutex);
        return m_queue.dequeue();
    }

private:
    QMutex      m_mutex;
    QQueue<T>   m_queue;
};

template<typename T>
using QueuePtr = QSharedPointer<Queue<T>>;
using StrQueuePtr = QSharedPointer<Queue<QString>>;
