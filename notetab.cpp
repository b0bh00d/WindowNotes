#ifdef QT_WIN
#define WIN32_MEAN_AND_LEAN //necessary to avoid compiler errors
#include <windows.h>
#endif

#include <QtWidgets/QToolTip>
#include <QtCore/QTimer>
#include <QtGui/QPainter>

#include "notetab.h"

#ifdef QT_LINUX
#include "linux/WindowEventsSingleton.h"
#endif

NoteTab::NoteTab(int icon, QWidget* parent, Qt::WindowFlags f)
    : my_icon(icon),
      QWidget(parent, f)
{
    setAttribute(Qt::WA_AlwaysShowToolTips);
    setAttribute(Qt::WA_TranslucentBackground);

    QPixmap tab_bitmap = QPixmap(QString(":/images/Note%1.png").arg(my_icon)).scaled(NOTE_DOCK_SIZE, NOTE_DOCK_SIZE);
    setMask(tab_bitmap.mask());

    QPalette* palette = new QPalette();
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    palette->setBrush(QPalette::Background, QBrush(tab_bitmap));
#pragma GCC diagnostic pop
    setPalette(*palette);
}

NoteTab::~NoteTab()
{
}

void NoteTab::showEvent(QShowEvent* /*event*/)
{
#ifdef QT_WIN
    // make sure the tab is always visible when it is made visible
    SetWindowPos((HWND)this->winId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif

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

    QPixmap tab_bitmap = QPixmap(QString(":/images/Note%1.png").arg(my_icon)).scaled(NOTE_DOCK_SIZE, NOTE_DOCK_SIZE);
    painter.drawPixmap(0, 0, tab_bitmap);
}

void NoteTab::enterEvent(QEvent* /*event*/)
{
#ifdef QT_LINUX
    WindowEventsSingleton::instance()->winEvents()->stop();
#endif

    if(!selected)
    {
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
        else if(!my_tool_tip.isEmpty())
            QToolTip::showText(enter_point, my_tool_tip, nullptr, QRect(), 600000);

        entered = true;
        opacity = 1.0;
        emit signal_tab_entered(this);
        update();
    }
}

void NoteTab::leaveEvent(QEvent* /*event*/)
{
    if(!selected)
    {
        if(!my_note.isNull() || !my_tool_tip.isEmpty())
            QToolTip::hideText();
        entered = false;
        opacity = unselected_opacity;
        emit signal_tab_exited(this);
        update();
    }

#ifdef QT_LINUX
    WindowEventsSingleton::instance()->winEvents()->start();
#endif
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
#ifdef QT_WIN
    SetWindowPos((HWND)this->winId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
#endif
}
