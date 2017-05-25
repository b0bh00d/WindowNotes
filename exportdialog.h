#pragma once

#include <QtCore/QString>
#include <QtWidgets/QDialog>

namespace Ui {
class ExportDialog;
}

/// @class ExportDialog
/// @brief Dialog for exporting Notes
///
/// This dialog is presented to the user to allow them to export their
/// current Note database (including all Contexts) to a text file in
/// various formats.

class ExportDialog : public QDialog
{
    Q_OBJECT
public:     // typedefs and enums
    typedef enum
    {
        FORMAT_XML,
        FORMAT_CSV,
        FORMAT_TEXT,
    } FileFormat;

public:
    explicit ExportDialog(QWidget *parent = 0);
    ~ExportDialog();

    void        set_folder(const QString& path);
    void        set_file_format(FileFormat format);
    QString     get_folder();
    FileFormat  get_file_format();

private slots:
    void        slot_browse_for_folder();

private:
    Ui::ExportDialog *ui;
};
