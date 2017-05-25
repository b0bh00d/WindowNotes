#pragma once

#include <QtWidgets/QWidget>
#include <QtGui/QShowEvent>
#include <QtCore/QEvent>

#include "types.h"

namespace Ui {
    class NoteEdit;
}

/// @class NoteEdit
/// @brief Widget that provides for editing an existing Note
///
/// This is a stylized Qt window that allows the user to edit the contents
/// of a Note.  It handles special behaviors, like loss of focus closing
/// the window.

class NoteEdit : public QWidget
{
    Q_OBJECT

public:
    enum EditMode
    {
        MODE_ADD,
        MODE_EDIT
    };

    explicit NoteEdit(EditMode = MODE_ADD, QWidget* parent = 0, Qt::WindowFlags f = Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    ~NoteEdit();

    void            set_note_data(const NoteData& data);
    void            get_note_data(NoteData& data);

    bool            eventFilter(QObject* object, QEvent* event);

signals:
    void            signal_lost_focus();

protected slots:
    void            slot_lost_focus();

protected:
//    virtual void    focusOutEvent(QFocusEvent * event);
    virtual void    showEvent(QShowEvent* event);

private:
    Ui::NoteEdit*   ui;
    QObject*        focus_object;
    int             context_icon;
};
