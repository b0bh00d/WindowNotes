#include "notewindow.h"

NoteWindow::NoteWindow(QWidget* parent, Qt::WindowFlags f)
    : entered(false),
      QWidget(parent, f)
{
    //QTextDocument* doc = new QTextDocument(this);
    //QVBoxLayout* layout = new QVBoxLayout();

    //layout->addWidget(doc);
    //setLayout(layout);
    //setMouseTracking(true);
}

NoteWindow::~NoteWindow()
{
}

void NoteWindow::enterEvent(QEvent* /*event*/)
{
    entered = true;
    QString msg = QString("Note: 0x%1").arg(reinterpret_cast<unsigned long>(this),8,16,QChar('0'));
    emit signal_note_entered(msg);
}

void NoteWindow::leaveEvent(QEvent* /*event*/)
{
    entered = false;
    QString msg = QString("Note: 0x%1").arg(reinterpret_cast<unsigned long>(this),8,16,QChar('0'));
    emit signal_note_exited(msg);
}

//void NoteWindow::mouseMoveEvent(QMouseEvent* event)
//{
//    if(event->type() == QEvent::Enter && !entered)
//    {
//        emit signal_right_mouse_button("Enter event.");
//        entered = true;
//    }
//    else if(event->type() == QEvent::Leave)
//    {
//        emit signal_right_mouse_button("Exit event.");
//        entered = false;
//    }

//    //if(event->button() == Qt::RightButton)
//    //{
//    //    emit signal_right_mouse_button("Right button pressed in note.");
//    //}
//}
