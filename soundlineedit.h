#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtGui/QShowEvent>
#include <QtGui/QFocusEvent>

/// @class SoundLineEdit
/// @brief Specialization of the QLineEdit class for audio
///
/// This subclass of QLineEdit provides for special behaviors when selecting audio
/// clips to be used in WindowNotes.

class SoundLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    SoundLineEdit(QWidget* parent = 0);
    ~SoundLineEdit();

    void        set_clear_button(QPushButton* button) { clear_button = button; }
    QString     adjusted_text();

public slots:
    void    clear();
    void    setDisabled(bool);
    void    setEnabled(bool);

private slots:
    void    slot_process_textEdited();

protected:
    virtual void showEvent(QShowEvent*);
    virtual void focusInEvent(QFocusEvent*);
    virtual void focusOutEvent(QFocusEvent*);

private:
    void    adjust_text_state();

    QPushButton* clear_button;
};
