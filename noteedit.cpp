#include <QTimer>
#include <QtWidgets/QLineEdit>

#include "mainwindow.h"
#include "noteedit.h"
#include "ui_noteedit.h"

extern MainWindow* main_window;

NoteEdit::NoteEdit(NoteEdit::EditMode mode, QWidget *parent, Qt::WindowFlags f)
    : focus_object(0),
      QWidget(parent, f),
      ui(new Ui::NoteEdit)
{
    ui->setupUi(this);

    if(mode == MODE_ADD)
        ui->label_Edit->hide();
    else
        ui->label_Add->hide();
    ui->edit_Note_Content->installEventFilter(this);
    ui->combo_Context->installEventFilter(this);
    QAbstractItemView* view = ui->combo_Context->view();
    view->installEventFilter(this);

    ui->check_Context_RegExpr->installEventFilter(this);
}

NoteEdit::~NoteEdit()
{
    delete ui;
}

bool NoteEdit::eventFilter(QObject* object, QEvent* event)
{
    if(event->type() == QEvent::FocusIn)
        focus_object = object;
    else if(event->type() == QEvent::FocusOut)
    {
        focus_object = (QObject*)0;
        QTimer::singleShot(100, this, &NoteEdit::slot_lost_focus);
    }

    return false;
}

void NoteEdit::showEvent(QShowEvent* /*event*/)
{
    QTextCursor cursor = ui->edit_Note_Content->textCursor();
    cursor.setPosition(ui->edit_Note_Content->toPlainText().length());
    ui->edit_Note_Content->setTextCursor(cursor);

    ui->edit_Note_Content->setFocus();
}

void NoteEdit::slot_lost_focus()
{
    if(!focus_object)
        emit signal_lost_focus();
}

void NoteEdit::set_note_data(const NoteData& data)
{
    ui->combo_Context->addItems(data.current_contexts);
    ui->combo_Context->lineEdit()->setText(data.context);
    ui->check_Context_RegExpr->setChecked(data.regexp);

    context_icon = data.context_icon;

    ui->edit_Note_Content->setPlainText(data.content);

    if(data.mode == NoteData::Mode_Edit)
    {
        ui->combo_Context->setEnabled(false);
        ui->check_Context_RegExpr->setEnabled(false);
    }
}

void NoteEdit::get_note_data(NoteData& data)
{
    data.context = ui->combo_Context->lineEdit()->text();
    data.regexp = ui->check_Context_RegExpr->isChecked();
    data.content = ui->edit_Note_Content->toPlainText();
}
