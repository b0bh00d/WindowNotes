#pragma once

#include <QtWidgets/QWidget>
#include <QtGui/QMouseEvent>
#include <QtGui/QShowEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QBitmap>
#include <QtGui/QPixmap>
#include <QtCore/QPoint>
#include <QtCore/QList>

#include "types.h"

const int FLASH_DELAY = 30;
const int FADE_DURATION = 300;

/// @class NoteTab
/// @brief Visual representation of a Note
///
/// This widget visually and logically represents a live Note.  It handles special
/// behaviors, like highlighting on mouse over, context menu, deletion animations, etc.
///
/// Instances of this class are managed by the Context class.

class NoteTab : public QWidget
{
    Q_OBJECT

public:
    NoteTab(int icon, QWidget* parent = 0, Qt::WindowFlags f = Qt::Tool | Qt::FramelessWindowHint);
    ~NoteTab();

    void    flash();
    void    fade();
    void    set_selected();
    void    set_unselected();
    void    set_opacities(double selected, double unselected);

    void            set_note_node(QDomElement note)           { my_note = note; }
    QDomElement     get_note_node()                     const { return my_note; }
    int             get_note_icon()                     const { return my_icon; }

signals:
    void    signal_tab_entered(NoteTab* id);
    void    signal_tab_exited(NoteTab* id);
    void    signal_tab_LMB_down(NoteTab* id, QMouseEvent* event);
    void    signal_tab_LMB_move(NoteTab* id, QMouseEvent* event);
    void    signal_tab_LMB_up(NoteTab* id, QMouseEvent* event);
    void    signal_tab_RMB(NoteTab* id, QMouseEvent* event);
    void    signal_tab_faded(NoteTab* id);
    void    signal_tab_flashed(NoteTab* id);

protected:
    virtual void showEvent(QShowEvent* event);
    virtual void paintEvent(QPaintEvent* event);
    virtual void enterEvent(QEvent* event);
    virtual void leaveEvent(QEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

private slots:
    void    slot_flash();
    void    slot_fade();
    void    slot_reset_window_flags();

private:    // data members
    bool    entered;
    double  selected_opacity;
    double  unselected_opacity;
    double  opacity;
    double  opacity_slice;
    bool    selected;

    QPoint  enter_point;
    int     my_icon;
    QDomElement my_note;

    Qt::MouseButton event_button;
};

typedef QList<NoteTab*>     TabsList;
typedef TabsList::iterator  TabsListIter;
