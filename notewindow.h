#ifndef NOTEWINDOW_H
#define NOTEWINDOW_H

#include <QtGui>

class NoteWindow : public QWidget
{
    Q_OBJECT

    bool    entered;

public:
    NoteWindow(QWidget* parent = 0, Qt::WindowFlags f = Qt::Tool | Qt::FramelessWindowHint);
    ~NoteWindow();

signals:
    void    signal_note_entered(const QString& msg);
    void    signal_note_exited(const QString& msg);

protected:
    virtual void enterEvent(QEvent* /*event*/);
    virtual void leaveEvent(QEvent* /*event*/);

    //virtual void mouseMoveEvent(QMouseEvent* event);
};

#endif  // NOTEWINDOW_H
