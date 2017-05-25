#include "types.h"

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QDialogButtonBox>

#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    bool connected = connect(ui->button_BrowseForFolder, &QPushButton::clicked, this, &ExportDialog::slot_browse_for_folder);
    ASSERT_UNUSED(connected);

    QPushButton* ok = ui->buttonBox->button(QDialogButtonBox::Ok);
    ok->setEnabled(false);
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::set_folder(const QString& path)
{
    if(!QFile::exists(path))
        return;

    ui->label_Path->setText(path);

    QPushButton* ok = ui->buttonBox->button(QDialogButtonBox::Ok);
    ok->setEnabled(true);
}

void ExportDialog::set_file_format(ExportDialog::FileFormat format)
{
    switch(format)
    {
        case FORMAT_XML:
            ui->radio_XML->setChecked(true);
            break;

        case FORMAT_CSV:
            ui->radio_CSV->setChecked(true);
            break;

        case FORMAT_TEXT:
            ui->radio_Text->setChecked(true);
            break;
    }
}

QString ExportDialog::get_folder()
{
    return ui->label_Path->text();
}

ExportDialog::FileFormat ExportDialog::get_file_format()
{
    if(ui->radio_XML->isChecked())
        return FORMAT_XML;
    if(ui->radio_CSV->isChecked())
        return FORMAT_CSV;
    return FORMAT_TEXT;
}

void ExportDialog::slot_browse_for_folder()
{
    QString path = ui->label_Path->text();
    path = QFileDialog::getExistingDirectory(this, tr("Select Export Folder"), path);
    ui->label_Path->setText(path);

    QPushButton* ok = ui->buttonBox->button(QDialogButtonBox::Ok);
    ok->setEnabled(true);
}
