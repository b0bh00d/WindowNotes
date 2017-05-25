#include "soundlineedit.h"

#include "types.h"

SoundLineEdit::SoundLineEdit(QWidget* parent)
    : clear_button(0),
      QLineEdit(parent)
{
    bool connected = connect(this, &SoundLineEdit::textChanged, this, &SoundLineEdit::slot_process_textEdited);
    ASSERT_UNUSED(connected);
}

SoundLineEdit::~SoundLineEdit()
{
}

QString SoundLineEdit::adjusted_text()
{
    QString value = text();
    QString lower_value = value.toLower();
    if(QFile::exists(lower_value) && lower_value.endsWith(".wav"))
        return value;

    return QString();
}

void SoundLineEdit::adjust_text_state()
{
    QString lower_value = text().toLower();

    if(clear_button)
        clear_button->setEnabled(isEnabled() && !lower_value.isEmpty() && lower_value != tr("(internal)"));

    if(lower_value == tr("(internal)") || !isEnabled())
        setStyleSheet(QString::fromUtf8("color: rgb(170, 170, 170);"));
    else if(QFile::exists(lower_value) && lower_value.endsWith(".wav"))
        setStyleSheet(QString::fromUtf8("color: rgb(0, 120, 0);"));
    else if(!lower_value.isEmpty())
        setStyleSheet(QString::fromUtf8("color: rgb(120, 0, 0);"));
}

void SoundLineEdit::clear()
{
    QLineEdit::clear();
    setText(tr("(internal)"));
}

void SoundLineEdit::setDisabled(bool value)
{
    QLineEdit::setDisabled(value);

    if(value && text() == tr("(internal)"))
        setText("");
    else if(!value && text().isEmpty())
        setText(tr("(internal)"));
    else
        adjust_text_state();
}

void SoundLineEdit::setEnabled(bool value)
{
    QLineEdit::setEnabled(value);

    if(!value && text() == tr("(internal)"))
        setText("");
    else if(value && text().isEmpty())
        setText(tr("(internal)"));
    else
        adjust_text_state();
}

void SoundLineEdit::focusInEvent(QFocusEvent* event)
{
    if(text() == tr("(internal)"))
        setText("");
    else
        adjust_text_state();

    QLineEdit::focusInEvent(event);
}

void SoundLineEdit::focusOutEvent(QFocusEvent* event)
{
    QLineEdit::focusOutEvent(event);

    QString lower_value = text().toLower();
    if(lower_value.isEmpty() || !QFile::exists(lower_value) || !lower_value.endsWith(".wav"))
        setText(tr("(internal)"));
    else
        adjust_text_state();
}

void SoundLineEdit::showEvent(QShowEvent* /*event*/)
{
    if(text().isEmpty() || !QFile::exists(text()))
    {
        if(isEnabled())
            setText(tr("(internal)"));
        else
            setText("");
    }

    QLineEdit::show();
}

void SoundLineEdit::slot_process_textEdited()
{
    adjust_text_state();
}
