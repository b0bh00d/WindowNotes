#define WIN32_MEAN_AND_LEAN //necessary to avoid compiler errors
#include <windows.h>

#include <QtWidgets/QToolTip>
#include <QtCore/QTimer>
#include <QtGui/QPainter>

#include "notetab.h"

NoteTab::NoteTab(int icon, QWidget* parent, Qt::WindowFlags f)
    : entered(false),
      selected_opacity(1.0),
      unselected_opacity(0.3),
      opacity(0.3),
      opacity_slice(0.0),
      selected(false),
      my_icon(icon),
      event_button(Qt::NoButton),
      QWidget(parent, f)
{
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_TranslucentBackground);

    QPixmap pencil_bitmap = QPixmap(QString(":/images/Note%1.png").arg(my_icon)).scaled(NOTE_DOCK_SIZE, NOTE_DOCK_SIZE);
    setMask(pencil_bitmap.mask());

    QPalette* palette = new QPalette();
    palette->setBrush(QPalette::Background, QBrush(pencil_bitmap));
    setPalette(*palette);
}

NoteTab::~NoteTab()
{
}

void NoteTab::showEvent(QShowEvent* /*event*/)
{
    // make sure the tab is always visible when it is made visible
    SetWindowPos((HWND)this->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    QWidget::show();

    QTimer::singleShot(10, this, SLOT(slot_reset_window_flags()));
}

void NoteTab::flash()
{
    opacity = unselected_opacity;
    QTimer::singleShot(FLASH_DELAY, this, SLOT(slot_flash()));
}

void NoteTab::fade()
{
    int slices = FADE_DURATION / FLASH_DELAY;
    opacity_slice = opacity / slices;
    QTimer::singleShot(FLASH_DELAY, this, SLOT(slot_fade()));
}

void NoteTab::set_selected()
{
    opacity = selected_opacity;
    repaint();
    selected = true;
}

void NoteTab::set_unselected()
{
    selected = false;
    opacity = unselected_opacity;
    repaint();
}

void NoteTab::set_opacities(double selected, double unselected)
{
    selected_opacity = selected;
    unselected_opacity = unselected;
    opacity = unselected;
    update();
}

void NoteTab::paintEvent(QPaintEvent* /*paintEvent*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setOpacity(opacity);

    QPixmap pencil_bitmap = QPixmap(QString(":/images/Note%1.png").arg(my_icon)).scaled(NOTE_DOCK_SIZE, NOTE_DOCK_SIZE);
    painter.drawPixmap(0, 0, pencil_bitmap);
}

void NoteTab::enterEvent(QEvent* /*event*/)
{
    if(selected)
        return;
    enter_point = QCursor::pos();

    if(!my_note.isNull())
    {
        bool use_mask = false;
        if(my_note.hasAttribute("use_mask"))
            use_mask = (my_note.attribute("use_mask").toInt() == 1);

        if(use_mask && my_note.hasAttribute("mask"))
            QToolTip::showText(enter_point, my_note.attribute("mask"), nullptr, QRect(), 600000);
        else
            QToolTip::showText(enter_point, my_note.firstChild().nodeValue(), nullptr, QRect(), 600000);
    }

    entered = true;
    opacity = 1.0;
    emit signal_tab_entered(this);
    update();
}

void NoteTab::leaveEvent(QEvent* /*event*/)
{
    if(selected)
        return;
    if(!my_note.isNull())
        QToolTip::hideText();
    entered = false;
    opacity = unselected_opacity;
    emit signal_tab_exited(this);
    update();
}

void NoteTab::mousePressEvent(QMouseEvent* event)
{
    event_button = event->button();
    if(event_button == Qt::LeftButton)
        emit signal_tab_LMB_down(this, event);
    else if(event_button == Qt::RightButton)
        emit signal_tab_RMB(this, event);
}

void NoteTab::mouseMoveEvent(QMouseEvent* event)
{
    if(event_button == Qt::LeftButton)
        emit signal_tab_LMB_move(this, event);
}

void NoteTab::mouseReleaseEvent(QMouseEvent* event)
{
    if(event_button == Qt::LeftButton)
        emit signal_tab_LMB_up(this, event);
}

void NoteTab::slot_flash()
{
    opacity += 0.1;
    if(opacity >= selected_opacity)
    {
        opacity = selected_opacity;
        emit signal_tab_flashed(this);
    }
    else
        QTimer::singleShot(FLASH_DELAY, this, SLOT(slot_flash()));

    update();
}

void NoteTab::slot_fade()
{
    opacity -= opacity_slice;
    if(opacity <= 0.0)
        emit signal_tab_faded(this);
    else
    {
        QTimer::singleShot(FLASH_DELAY, this, SLOT(slot_fade()));
        update();
    }
}

void NoteTab::slot_reset_window_flags()
{
    SetWindowPos((HWND)this->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
