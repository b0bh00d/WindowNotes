#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QMenu>
#include <QtWidgets/QFileDialog>
#include <QtGui/QClipboard>
#include <QtGui/QDrag>
#include <QtGui/QIntValidator>
#include <QtCore/QTemporaryFile>
#include <QtCore/QTextStream>
#include <QtCore/QCryptographicHash>
#include <QtCore/QBitArray>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QProcess>
#include <QtCore/QRandomGenerator>
#include <QtCore/QStandardPaths>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "exportdialog.h"

static const QString& settings_version = "1.0";

MainWindow* main_window = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
//    QTime time = QTime::currentTime();
//    qsrand((uint)time.msec());

    // perform operating system-specific initializations
    os_events_init();

    main_window = this;

    ui->setupUi(this);

    if(os_locate_instance())
    {
        QMessageBox::warning(window(), tr("WindowNotes"),
                                 tr("An instance of WindowNotes is already running."));
        exit(0);
    }

    setWindowTitle(tr("WindowNotes by Bob Hood"));
    setWindowIcon(QIcon(":/images/Pencil.png"));

#ifndef DEBUG
    ui->tree_Actions->hide();
#endif

    load_application_settings();
    load_note_database();

    bool connected;

    // Settings

#ifdef QT_WIN
    connected = connect(ui->edit_Add_Key, &QLineEdit::textChanged, this, &MainWindow::slot_validate_add_key);
    ASSERT_UNUSED(connected);
#endif
#ifdef QT_LINUX
    ui->edit_Add_Key->setEnabled(false);
#endif

    connected = connect(ui->check_Start_Automatically, &QCheckBox::stateChanged, this, &MainWindow::slot_toggle_startup);
    ASSERT_UNUSED(connected);

    connected = connect(ui->spin_Selected_Opacity, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                        this, &MainWindow::slot_selected_opacity_changed);
    ASSERT_UNUSED(connected);

    connected = connect(ui->spin_Unselected_Opacity, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                        this, &MainWindow::slot_unselected_opacity_changed);
    ASSERT_UNUSED(connected);

    ui->radio_Position_Left->setChecked(true);

    connected = connect(ui->radio_Position_Left, &QRadioButton::toggled, this, &MainWindow::slot_select_position_left);
    ASSERT_UNUSED(connected);

    connected = connect(ui->radio_Position_Top, &QRadioButton::toggled, this, &MainWindow::slot_select_position_top);
    ASSERT_UNUSED(connected);

    connected = connect(ui->radio_Position_Right, &QRadioButton::toggled, this, &MainWindow::slot_select_position_right);
    ASSERT_UNUSED(connected);

    connected = connect(ui->radio_Position_Bottom, &QRadioButton::toggled, this, &MainWindow::slot_select_position_bottom);
    ASSERT_UNUSED(connected);

    ui->check_Enable_Sound_Effects->setChecked(enable_sound_effects);
    connected = connect(ui->check_Enable_Sound_Effects, &QCheckBox::toggled, this, &MainWindow::slot_enable_sound_effects);
    ASSERT_UNUSED(connected);

    ui->edit_Delete_Note_Sound->set_clear_button(ui->button_Clear_Delete_Sound);
    ui->edit_Delete_Note_Sound->setEnabled(enable_sound_effects);
    ui->edit_Delete_Note_Sound->setText(sound_files[SOUND_DELETE]);

    connected = connect(ui->edit_Delete_Note_Sound, &QLineEdit::editingFinished, this, &MainWindow::slot_delete_sound_file_changed);
    ASSERT_UNUSED(connected);
    connected = connect(ui->button_Clear_Delete_Sound, &QPushButton::clicked, this, &MainWindow::slot_clear_delete_sound);
    ASSERT_UNUSED(connected);
    connected = connect(ui->button_Browse_For_Delete_Sound, &QPushButton::clicked, this, &MainWindow::slot_browse_for_delete_sound);
    ASSERT_UNUSED(connected);

    ui->edit_Copy_Note_Sound->set_clear_button(ui->button_Clear_Copy_Sound);
    ui->edit_Copy_Note_Sound->setEnabled(enable_sound_effects);
    ui->edit_Copy_Note_Sound->setText(sound_files[SOUND_COPY]);

    connected = connect(ui->edit_Copy_Note_Sound, &QLineEdit::editingFinished, this, &MainWindow::slot_copy_sound_file_changed);
    ASSERT_UNUSED(connected);
    connected = connect(ui->button_Clear_Copy_Sound, &QPushButton::clicked, this, &MainWindow::slot_clear_copy_sound);
    ASSERT_UNUSED(connected);
    connected = connect(ui->button_Browse_For_Copy_Sound, &QPushButton::clicked, this, &MainWindow::slot_browse_for_copy_sound);
    ASSERT_UNUSED(connected);

    ui->edit_Open_Edit_Sound->set_clear_button(ui->button_Clear_Open_Sound);
    ui->edit_Open_Edit_Sound->setEnabled(enable_sound_effects);
    ui->edit_Open_Edit_Sound->setText(sound_files[SOUND_NOTEOPEN]);

    connected = connect(ui->edit_Open_Edit_Sound, &QLineEdit::editingFinished, this, &MainWindow::slot_open_sound_file_changed);
    ASSERT_UNUSED(connected);
    connected = connect(ui->button_Clear_Open_Sound, &QPushButton::clicked, this, &MainWindow::slot_clear_open_sound);
    ASSERT_UNUSED(connected);
    connected = connect(ui->button_Browse_For_Open_Sound, &QPushButton::clicked, this, &MainWindow::slot_browse_for_open_sound);
    ASSERT_UNUSED(connected);

    ui->edit_Close_Edit_Sound->set_clear_button(ui->button_Clear_Close_Sound);
    ui->edit_Close_Edit_Sound->setEnabled(enable_sound_effects);
    ui->edit_Close_Edit_Sound->setText(sound_files[SOUND_NOTECLOSE]);

    connected = connect(ui->edit_Close_Edit_Sound, &QLineEdit::editingFinished, this, &MainWindow::slot_close_sound_file_changed);
    ASSERT_UNUSED(connected);
    connected = connect(ui->button_Clear_Close_Sound, &QPushButton::clicked, this, &MainWindow::slot_clear_close_sound);
    ASSERT_UNUSED(connected);
    connected = connect(ui->button_Browse_For_Close_Sound, &QPushButton::clicked, this, &MainWindow::slot_browse_for_close_sound);
    ASSERT_UNUSED(connected);

    set_sound_states();

    ui->check_ClipboardTTL->setChecked(clipboard_ttl);

    connected = connect(ui->check_ClipboardTTL, &QPushButton::clicked, this, &MainWindow::slot_toggle_clipboard_ttl);
    ASSERT_UNUSED(connected);
    connected = connect(ui->edit_ClipboardTTL, &QLineEdit::textEdited, this, &MainWindow::slot_clipboard_ttl_timeout_changed);
    ASSERT_UNUSED(connected);

    if(clipboard_ttl_timeout > 0)
        ui->edit_ClipboardTTL->setText(QString::number(clipboard_ttl_timeout));
    QIntValidator* validator = new QIntValidator(1, 9999, this);
    ui->edit_ClipboardTTL->setValidator(validator);
    ui->edit_ClipboardTTL->setEnabled(clipboard_ttl);

    connected = connect(ui->button_Export, &QPushButton::clicked, this, &MainWindow::slot_export_notes);
    ASSERT_UNUSED(connected);
    connected = connect(ui->button_Import, &QPushButton::clicked, this, &MainWindow::slot_import_notes);
    ASSERT_UNUSED(connected);

    ui->tree_Database->setContextMenuPolicy(Qt::CustomContextMenu);
    connected = connect(ui->tree_Database, &QTreeWidget::customContextMenuRequested, this, &MainWindow::slot_show_database_context_menu);
    ASSERT_UNUSED(connected);
    connected = connect(ui->tree_Database, &QTreeWidget::itemDoubleClicked, this, &MainWindow::slot_edit_assign);
    ASSERT_UNUSED(connected);

    // About

    connected = connect(ui->text_License, &QTextBrowser::anchorClicked, this, &MainWindow::slot_process_link);
    ASSERT_UNUSED(connected);

    connected = connect(ui->label_DenverZoo, &QLabel::linkActivated, this, &MainWindow::slot_process_label_link);
    ASSERT_UNUSED(connected);

    connected = connect(ui->label_CatCareSociety, &QLabel::linkActivated, this, &MainWindow::slot_process_label_link);
    ASSERT_UNUSED(connected);

    connected = connect(ui->label_MaxFund, &QLabel::linkActivated, this, &MainWindow::slot_process_label_link);
    ASSERT_UNUSED(connected);

    connected = connect(ui->label_RMFR, &QLabel::linkActivated, this, &MainWindow::slot_process_label_link);
    ASSERT_UNUSED(connected);

    // Tray

    trayIcon = new QSystemTrayIcon(this);
    connected = connect(trayIcon, &QSystemTrayIcon::messageClicked, this, &MainWindow::slot_message_clicked);
    ASSERT_UNUSED(connected);
    connected = connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::slot_icon_activated);
    ASSERT_UNUSED(connected);

    trayIcon->setIcon(QIcon(":/images/Pencil.png"));
    trayIcon->setToolTip(tr("WindowNotes"));

    build_tray_menu();

    clipboard_timer = new QTimer(this);
    connected = connect(clipboard_timer, &QTimer::timeout, this, &MainWindow::slot_clipboard_ttl);
    clipboard_timer->start(1000);

    trayIcon->show();

    tabs_list = new TabsList;

    cache_sounds();

#ifdef QT_WIN
    hook_global_keyboard_event();
#endif

    os_set_hooks();
}

MainWindow::~MainWindow()
{
}

void MainWindow::slot_housekeeping()
{
    if(isVisible())
        return;
}

void MainWindow::slot_tab_entered(NoteTab* id)
{
    add_log_entry(QString("Note: 0x%1").arg(reinterpret_cast<size_t>(id),8,16,QChar('0')), "Entered", "");
}

void MainWindow::slot_tab_exited(NoteTab* id)
{
    add_log_entry(QString("Note: 0x%1").arg(reinterpret_cast<size_t>(id),8,16,QChar('0')), "Exited", "");
}

void MainWindow::slot_tab_LMB_down(NoteTab* /*id*/, QMouseEvent* event)
{
    notetab_clicked = true;
    did_drag_and_drop = false;
    drag_start_position = event->pos();
}

void MainWindow::slot_create_data(const QString &mime_type)
{
    if(mime_type != "text/uri-list" && mime_type != "text/plain")
        return;

    QString note_text;

    QDomElement note_node = drag_notetab_id->get_note_node();
    note_text = note_node.firstChild().nodeValue();

    if(mime_type == "text/uri-list")
    {
        QString temp_file_name;
        if(drag_temp_files.contains(drag_notetab_id))
            temp_file_name = drag_temp_files[drag_notetab_id];
        else
        {
            QTemporaryFile temp_file;
            temp_file.setAutoRemove(false);
            if(!temp_file.open())
                return;

            QTextStream stream(&temp_file);
            stream << note_text << "\n";

            temp_file_name = temp_file.fileName();
            drag_temp_files[drag_notetab_id] = temp_file_name;
            temp_file.close();
        }

        QList<QUrl> url_list;
        url_list.append(QUrl::fromLocalFile(temp_file_name));
        drag_mime_data->setUrls(url_list);
        return;
    }

    drag_mime_data->setText(note_text);
}

void MainWindow::slot_tab_LMB_move(NoteTab* id, QMouseEvent* event)
{
    if(!notetab_clicked)
        return;
    if((event->pos() - drag_start_position).manhattanLength() < QApplication::startDragDistance())
         return;

    did_drag_and_drop = true;

    // initiate a drag-and-drop event
    QDrag* drag             = new QDrag(this);
    drag_mime_data          = new MimeData;
    drag_notetab_id         = id;

    // use delayed encoding of the MIME data

    connect(drag_mime_data, &MimeData::dataRequested, this, &MainWindow::slot_create_data, Qt::DirectConnection);

    drag->setMimeData(drag_mime_data);
    drag->setPixmap(QPixmap(QString(":/images/Note%1.png").arg(id->get_note_icon())).scaled(NOTE_DOCK_SIZE, NOTE_DOCK_SIZE));

#ifdef QT_WIN
    unhook_global_keyboard_event();
    unhook_global_win_event();
#endif
#ifdef QT_LINUX
    lnx_ignore_events();
#endif

    hide_notetabs();

    drag_temp_files.clear();

    if(drag->exec(Qt::CopyAction) == Qt::CopyAction)
        os_play_sound(SOUND_DROPPED);  // the drop was accepted somewhere

    foreach(NoteTab* key, drag_temp_files.keys())
        QFile::remove(drag_temp_files[key]);

#ifdef QT_WIN
    hook_global_win_event();
    hook_global_keyboard_event();
#endif
#ifdef QT_LINUX
    lnx_regard_events();
#endif

    // NOTE: these calls to arrange and display will render the 'id' pointer
    // parameter invalid.  DO NOT USE IT AFTERWARD!

    arrange_notetabs();
    display_notetabs();

    // since the UP event will not be passed along due to the drag-and-drop,
    // we clean up here manually...

    notetab_clicked = false;
    did_drag_and_drop = false;
}

void MainWindow::slot_tab_LMB_up(NoteTab* id, QMouseEvent* /*event*/)
{
    notetab_clicked = false;
    if(did_drag_and_drop)
        return;

    os_play_sound(SOUND_COPY);

    add_log_entry(QString("Note: 0x%1").arg(reinterpret_cast<size_t>(id),8,16,QChar('0')), "Left Button", "");

    QClipboard* clipboard = qApp->clipboard();
    QDomElement note_node = id->get_note_node();
    clipboard->setText(note_node.firstChild().nodeValue());

    connect(id, &NoteTab::signal_tab_flashed, this, &MainWindow::slot_tab_flashed);
    tab_animating = true;
    id->flash();

    if(note_node.hasAttribute("clipboard_ttl") && note_node.attribute("clipboard_ttl").toInt())
    {
        if(note_node.hasAttribute("clipboard_seconds"))
        {
            int note_ttl = note_node.attribute("clipboard_seconds").toInt();
            if(note_ttl > 0)
                clipboard_ttl_timeout = note_ttl;
        }
    }

    clipboard_timestamp = QDateTime::currentMSecsSinceEpoch();
}

void MainWindow::slot_tab_RMB(NoteTab* id, QMouseEvent* event)
{
    TabsListIter tabs_iter;

    add_log_entry(QString("Note: 0x%1").arg(reinterpret_cast<size_t>(id),8,16,QChar('0')), "Right Button", "");

    if(!current_context)
        return;

    in_context_menu = true;

    QMenu menu(tr("Context menu"), this);

    QAction* action = menu.addAction(QIcon(":/images/Edit.png"), tr("Edit Note"));
    bool connected = connect(action, &QAction::triggered, this, &MainWindow::slot_pu_edit_note);
    ASSERT_UNUSED(connected);

    menu.addSeparator();

    action = menu.addAction(QIcon(":/images/Restore.png"), tr("Copy Note"));
    connected = connect(action, &QAction::triggered, this, &MainWindow::slot_pu_copy_note);
    ASSERT_UNUSED(connected);

    menu.addSeparator();

    action = menu.addAction(QIcon(":/images/Remove.png"), tr("Delete Note"));
    connected = connect(action, &QAction::triggered, this, &MainWindow::slot_pu_delete_note);
    ASSERT_UNUSED(connected);

    action = menu.addAction(QIcon(":/images/Bomb.png"), tr("Clear All Notes"));
    connected = connect(action, &QAction::triggered, this, &MainWindow::slot_pu_clear_notes);
    ASSERT_UNUSED(connected);

    id->set_selected();

    current_tab = id;
    note_action = NOTEACTION_NONE;

    event_pos = event->globalPos();
    menu.exec(event->globalPos());

    switch(note_action)
    {
        case NOTEACTION_DELETE:
            connected = connect(current_tab, &NoteTab::signal_tab_faded, this, &MainWindow::slot_single_tab_faded);
            tab_animating = true;
            current_tab->fade();

            os_play_sound(SOUND_DELETE);

            break;

        case NOTEACTION_EDIT:
            // leave 'in_context_menu' set to protect 'current_tab' until
            // editing is complete
            QTimer::singleShot(200, this, SLOT(slot_edit_note()));
            break;

        case NOTEACTION_COPY:
            {
                if(!note_clipboard)
                    note_clipboard = new NoteData();

                QDomElement element = id->get_note_node();
                note_clipboard->content = element.firstChild().nodeValue();
            }

            current_tab->set_unselected();
            in_context_menu = false;

            build_tray_menu();

            break;

        case NOTEACTION_DELETE_ALL:
            current_tab->set_unselected();

            for(tabs_iter = tabs_list->begin();tabs_iter != tabs_list->end();tabs_iter++)
            {
                NoteTab* nt = *tabs_iter;
                connected = connect(nt, &NoteTab::signal_tab_faded, this, &MainWindow::slot_multiple_tab_faded);
                ASSERT_UNUSED(connected);
                nt->fade();
            }

            os_play_sound(SOUND_DELETE);
            break;

        case NOTEACTION_NONE:
            current_tab->set_unselected();
            in_context_menu = false;
            break;
    }
}

void MainWindow::slot_tab_flashed(NoteTab* /*id*/)
{
    tab_animating = false;
}

void MainWindow::slot_single_tab_faded(NoteTab* id)
{
    tab_animating = false;

    hide_notetabs();

    if(current_context->remove_note(id->get_note_node()) == 0)
    {
        delete_notetabs();

        contexts_list.removeAll(current_context);

        if(contexts_list.size() == 0)
        {
            QDomElement root = note_database->documentElement();
            root.removeChild(current_context->get_node());
        }

        delete current_context;
        current_context = (Context*)0;
    }
    else
    {
        arrange_notetabs();
        display_notetabs();
    }

    save_note_database();

    in_context_menu = false;
}

void MainWindow::slot_multiple_tab_faded(NoteTab* id)
{
    if(current_context->remove_note(id->get_note_node()) != 0)
        return;

    tab_animating = false;

    delete_notetabs();

    QDomElement root = note_database->documentElement();
    root.removeChild(current_context->get_node());
    contexts_list.removeAll(current_context);

    delete current_context;
    current_context = (Context*)0;

    save_note_database();

    in_context_menu = false;
}

void MainWindow::slot_pu_edit_note()
{
    note_action = NOTEACTION_EDIT;
}

void MainWindow::slot_pu_copy_note()
{
    note_action = NOTEACTION_COPY;
}

void MainWindow::slot_pu_delete_note()
{
    note_action = NOTEACTION_DELETE;
}

void MainWindow::slot_pu_clear_notes()
{
    note_action = NOTEACTION_DELETE_ALL;
}

void MainWindow::slot_edit_note()
{
    if(note_edit_window)
        delete note_edit_window;

    note_edit_window = new NoteEdit(NoteEdit::MODE_EDIT);
    const QRect& r = note_edit_window->geometry();
    note_edit_window->setGeometry(event_pos.x(), event_pos.y(), r.width(), r.height());

    QDomElement note_node = current_tab->get_note_node();

    NoteData note_data;
    note_data.mode = NoteData::Mode_Edit;
    note_data.context_icon = current_context->get_icon();

    ContextsListIter iter;
    for(iter = contexts_list.begin();iter != contexts_list.end();iter++)
        note_data.current_contexts.append((*iter)->context_id);

    if(current_context->id_is_expression)
    {
        note_data.context = current_context->context_expr.pattern();
        note_data.regexp = true;
    }
    else
        note_data.context = current_context->context_id;

    note_data.content = note_node.firstChild().nodeValue();

    note_edit_window->set_note_data(note_data);

    bool connected = connect(note_edit_window, &NoteEdit::signal_lost_focus, this, &MainWindow::slot_close_edit_window);
    ASSERT_UNUSED(connected);

    os_play_sound(SOUND_NOTEOPEN);
    note_edit_window->show();
#ifdef QT_WIN
    make_active_window(note_edit_window->winId());
#endif
#ifdef QT_LINUX
    note_edit_window->raise();
    note_edit_window->setFocus();
#endif
}

void MainWindow::slot_close_edit_window()
{
    NoteData note_data;
    note_edit_window->get_note_data(note_data);

    if(note_data.context.size() && note_data.content.size())
    {
        QDomElement note_node = current_tab->get_note_node();
        note_node.firstChild().setNodeValue(note_data.content);

        save_note_database();
    }

    note_edit_window->close();

    os_play_sound(SOUND_NOTECLOSE);

    note_edit_window->deleteLater();
    note_edit_window = (NoteEdit*)0;

    current_tab->set_unselected();
    in_context_menu = false;
}

void MainWindow::add_log_entry(const QString& col1, const QString& col2, const QString& col3)
{
    if(!winId())
        return;

    QTreeWidgetItem* item;
    while(ui->tree_Actions->topLevelItemCount() > 99)
    {
        item = ui->tree_Actions->takeTopLevelItem(0);
        delete item;
    }

    item = new QTreeWidgetItem(ui->tree_Actions, QStringList() << col1 << col2 << col3);

    for(int x = 0;x < ui->tree_Actions->columnCount();x++)
        ui->tree_Actions->resizeColumnToContents(x);

    ui->tree_Actions->setCurrentItem(item);
}

void MainWindow::hide_notetabs()
{
    if(tab_animating || !tabs_list->size() || isVisible())
        return;

    TabsListIter tabs_iter;
    for(tabs_iter = tabs_list->begin();tabs_iter != tabs_list->end();tabs_iter++)
        (*tabs_iter)->hide();
}

void MainWindow::delete_notetabs()
{
    if(tab_animating || !current_context || isVisible())
        return;

    if(tabs_list->size())
    {
        TabsListIter tabs_iter;
        for(tabs_iter = tabs_list->begin();tabs_iter != tabs_list->end();tabs_iter++)
        {
            NoteTab* nt = *tabs_iter;
            nt->hide();
            delete nt;
        }
        tabs_list->clear();
    }
}

void MainWindow::slot_export_notes()
{
    ExportDialog export_dialog;
    export_dialog.set_file_format(ExportDialog::FORMAT_TEXT);

    switch(export_dialog.exec())
    {
        case QDialog::Rejected:
        case QMessageBox::No:
        case QMessageBox::Cancel:
            return;
    }

    QString folder                  = export_dialog.get_folder();
    ExportDialog::FileFormat format = export_dialog.get_file_format();

    QString file_name = QString("%1\\WindowNotes.%2")
                .arg(folder)
                .arg((format == ExportDialog::FORMAT_XML) ? "xml" : ((format == ExportDialog::FORMAT_CSV) ? "csv" : "txt"));

    QFile output_stream(file_name);
    if(!output_stream.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return;
    QTextStream out(&output_stream);

    // convert all existing masked notes
    int context_count = 0;
    ContextsListIter iter;
    for(iter = contexts_list.begin();iter != contexts_list.end();iter++)
    {
        Context& context = *(*iter);

        if(format == ExportDialog::FORMAT_CSV)
        {
            out << QUrl::toPercentEncoding(context.context_id) << ",";
            out << context.id_is_expression << ",";
            out << context.note_count() << ",";
        }
        else if(format == ExportDialog::FORMAT_TEXT)
        {
            if(context_count)
                out << "-----------------\n";
            out << "context_id: " << context.context_id << "\n";
            out << "id_is_expression: " <<context.id_is_expression << "\n";
            out << "note_count: " << context.note_count() << "\n";
        }

        for(int i = 0;i < (*iter)->note_count();i++)
        {
            if(format == ExportDialog::FORMAT_CSV && i)
                out << ",";

            QDomElement note = (*iter)->get_note(i);

            QString note_content;
            int icon = (*iter)->get_icon();

            note_content = note.firstChild().nodeValue();

            if(format == ExportDialog::FORMAT_CSV)
            {
                out << ",";
                out << icon << ",";
                out << QUrl::toPercentEncoding(note_content);
            }
            else if(format == ExportDialog::FORMAT_TEXT)
            {
                out << (i + 1) << ": note_icon: " << icon << "\n";
                out << (i + 1) << ": note_content: " << note_content;
                if(!note_content.endsWith('\n'))
                    out << "\n";
            }
        }

        if(format == ExportDialog::FORMAT_CSV)
            out << "\n";

        ++context_count;
    }

    output_stream.close();
}

void MainWindow::slot_import_notes()
{
}

void MainWindow::display_notetabs()
{
    if(tab_animating || !current_context || !tabs_list->size() || isVisible())
        return;

    TabsListIter iter;
    for(iter = tabs_list->begin();iter != tabs_list->end();iter++)
    {
        (*iter)->show();
        (*iter)->raise();
    }
}

NoteTab* MainWindow::add_notetab(int icon)
{
    NoteTab* nt = new NoteTab(icon, this);
    nt->set_opacities(selected_opacity, unselected_opacity);
    bool connected = connect(nt, &NoteTab::signal_tab_entered, this, &MainWindow::slot_tab_entered);
    connected = connect(nt, &NoteTab::signal_tab_exited, this, &MainWindow::slot_tab_exited);
    connected = connect(nt, &NoteTab::signal_tab_LMB_down, this, &MainWindow::slot_tab_LMB_down);
    connected = connect(nt, &NoteTab::signal_tab_LMB_move, this, &MainWindow::slot_tab_LMB_move);
    connected = connect(nt, &NoteTab::signal_tab_LMB_up, this, &MainWindow::slot_tab_LMB_up);
    connected = connect(nt, &NoteTab::signal_tab_RMB, this, &MainWindow::slot_tab_RMB);
    return nt;
}

NoteTab* MainWindow::append_addtab()
{
    NoteTab* nt = new NoteTab(9999, this);
    nt->set_opacities(selected_opacity, unselected_opacity);
    bool connected = connect(nt, &NoteTab::signal_tab_LMB_up, this, &MainWindow::slot_process_add);
    return nt;
}

int MainWindow::arrange_notetabs(bool hide_and_show)
{
    if(tab_animating
#ifdef QT_WIN
        || !focus_window_handle
#endif
        || !current_context || isVisible())
        return 0;

    QBitArray sides(4);

    QDesktopWidget desk;
    QRect r = desk.screenGeometry();
#ifdef QT_WIN
    GetWindowRect(focus_window_handle, (LPRECT)&focus_window_rect);

    auto win_left = focus_window_rect.left;
    auto win_bottom = focus_window_rect.bottom;
    auto win_right = focus_window_rect.right;
    auto win_top = focus_window_rect.top;
#endif
#ifdef QT_LINUX
    auto win_left = focus_window_rect.left();
    auto win_bottom = focus_window_rect.bottom();
    auto win_right = focus_window_rect.right();
    auto win_top = focus_window_rect.top();
#endif

    if((win_left - NOTE_OFFSET - NOTE_DOCK_SIZE) > 0)
        sides.setBit(SIDE_LEFT, true);
    if((win_bottom + NOTE_OFFSET + NOTE_DOCK_SIZE) < r.height())
        sides.setBit(SIDE_BOTTOM, true);
    if((win_right + NOTE_OFFSET + NOTE_DOCK_SIZE) < r.width())
        sides.setBit(SIDE_RIGHT, true);
    if((win_top - NOTE_OFFSET - NOTE_DOCK_SIZE) > 0)
        sides.setBit(SIDE_TOP, true);

    if(!sides.count(true))
        return 0;

#ifdef QT_WIN
    int left_count = (focus_window_rect.bottom - focus_window_rect.top) / (NOTE_DOCK_SIZE + 5);
    int bottom_count = (focus_window_rect.right - focus_window_rect.left) / (NOTE_DOCK_SIZE + 5);
#endif
#ifdef QT_LINUX
    int left_count = (focus_window_rect.bottom() - focus_window_rect.top()) / (NOTE_DOCK_SIZE + 5);
    int bottom_count = (focus_window_rect.right() - focus_window_rect.left()) / (NOTE_DOCK_SIZE + 5);
#endif

    int rotation[] = { SIDE_LEFT, SIDE_BOTTOM, SIDE_RIGHT, SIDE_TOP };
    int direction[] = { 1, 1, -1, -1 };
    int maxtabs = (sides[SIDE_LEFT] ? left_count : 0) + (sides[SIDE_RIGHT] ? left_count : 0) + (sides[SIDE_TOP] ? bottom_count : 0) + (sides[SIDE_BOTTOM] ? bottom_count : 0);

    if(favored_side == FAVOR_TOP)
    {
        rotation[0] = SIDE_TOP; rotation[1] = SIDE_RIGHT; rotation[2] = SIDE_BOTTOM; rotation[3] = SIDE_LEFT;
        direction[0] = 1; direction[1] = 1; direction[2] = -1; direction[3] = -1;
    }
    else if(favored_side == FAVOR_BOTTOM)
    {
        rotation[0] = SIDE_BOTTOM; rotation[1] = SIDE_RIGHT; rotation[2] = SIDE_TOP; rotation[3] = SIDE_LEFT;
        direction[0] = 1; direction[1] = -1; direction[2] = -1; direction[3] = -1;
    }
    else if(favored_side == FAVOR_RIGHT)
    {
        rotation[0] = SIDE_RIGHT; rotation[1] = SIDE_BOTTOM; rotation[2] = SIDE_LEFT; rotation[3] = SIDE_TOP;
        direction[0] = 1; direction[1] = -1; direction[2] = -1; direction[3] = 1;
    }

    delete_notetabs();

    int max_notes = current_context->note_count() /* include the add tab */ + 1;
    int add_tab = max_notes;
    int note_index = 0;

    for(int i = 0; i < 4;i++)
    {
        if(sides[rotation[i]])
        {
            int left, top;
            int left_increment, top_increment;
            int side_max;

            switch(rotation[i])
            {
                case SIDE_LEFT:
                    left = win_left - NOTE_OFFSET - NOTE_DOCK_SIZE;
                    left_increment = 0;
                    top = (direction[i] == 1) ? (win_top + NOTE_MARGIN) : (win_bottom - NOTE_DOCK_SIZE);
                    top_increment = NOTE_DOCK_SIZE + NOTE_MARGIN;
                    top_increment *= direction[i];
                    side_max = left_count;
                    break;

                case SIDE_TOP:
                    left = (direction[i] == 1) ? (win_left + NOTE_MARGIN) : (win_right - NOTE_DOCK_SIZE);
                    left_increment = NOTE_DOCK_SIZE + NOTE_MARGIN;
                    left_increment *= direction[i];
                    top = win_top - NOTE_OFFSET - NOTE_DOCK_SIZE;
                    top_increment = 0;
                    side_max = bottom_count;
                    break;

                case SIDE_RIGHT:
                    left = win_right + NOTE_OFFSET;
                    left_increment = 0;
                    top = (direction[i] == 1) ? (win_top + NOTE_MARGIN) : (win_bottom - NOTE_DOCK_SIZE);
                    top_increment = NOTE_DOCK_SIZE + NOTE_MARGIN;
                    top_increment *= direction[i];
                    side_max = left_count;
                    break;

                case SIDE_BOTTOM:
                    left = (direction[i] == 1) ? (win_left + NOTE_MARGIN) : (win_right - NOTE_DOCK_SIZE);
                    left_increment = NOTE_DOCK_SIZE + NOTE_MARGIN;
                    left_increment *= direction[i];
                    top = win_bottom + NOTE_OFFSET;
                    top_increment = 0;
                    side_max = bottom_count;
                    break;
            }

            while(note_index < max_notes && note_index < maxtabs && side_max)
            {
                NoteTab* nt{nullptr};
                if(note_index == add_tab)
                    nt = append_addtab();
                else
                    nt = add_notetab(current_context->get_icon());
                nt->setGeometry(left, top, NOTE_DOCK_SIZE, NOTE_DOCK_SIZE);
                if(note_index != add_tab)
                    nt->set_note_node(current_context->get_note(note_index).toElement());
                hide_and_show ? nt->show() : nt->hide();

                tabs_list->append(nt);

                top += top_increment;
                left += left_increment;

                --side_max;

                ++note_index;
            }
        }
    }

    return sides.count(true);
}

void MainWindow::slot_quit()
{
#ifdef QT_WIN
    unhook_global_keyboard_event();
    unhook_global_win_event();
    DeregisterShellHookWindow((HWND)winId());
#endif
#ifdef QT_LINUX
    lnx_ignore_events();
#endif

    if(note_clipboard)
        delete note_clipboard;

    if(note_edit_window)
        delete note_edit_window;

    if(settings_modified)
        save_application_settings();
    purge_empty_contexts();
    if(notes_modified)
        save_note_database();

    ContextsListIter context_iter;
    for(context_iter = contexts_list.begin();context_iter != contexts_list.end();context_iter++)
        delete (*context_iter);

    if(tabs_list)
    {
        TabsListIter tabs_iter;
        for(tabs_iter = tabs_list->begin();tabs_iter != tabs_list->end();tabs_iter++)
            delete (*tabs_iter);
        delete tabs_list;
    }

    if(note_database)
        delete note_database;

    if(clipboard_timer)
    {
        clipboard_timer->stop();
        delete clipboard_timer;
    }

    qApp->quit();
}

void MainWindow::set_visible(bool visible)
{
    if(visible)
        restore_window_data(this);

    QWidget::setVisible(visible);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if(trayIcon->isVisible())
    {
        save_window_data(this);
        hide();

        disconnect(ui->tree_Database, &QTreeWidget::itemChanged, this, &MainWindow::slot_edit_finished);

#ifdef QT_WIN
        hook_global_keyboard_event();
#endif
#ifdef QT_LINUX
        lnx_regard_events();
#endif
        event->ignore();
    }
}

void MainWindow::save_window_data(QWidget* window)
{
    if(!window_geometry_save_enabled)
        return;

    QString key = window->windowTitle();
    window_data[key] = window->saveGeometry();

    settings_modified = true;
}

void MainWindow::restore_window_data(QWidget* window)
{
    QString key = window->windowTitle();
    if(!window_data.contains(key))
        return;

    window->restoreGeometry(window_data[key]);
}

int MainWindow::ask_question(const QString& question, bool with_cancel, const QString& title, QMessageBox::Icon icon, QStringList buttons)
{
    QMessageBox msg_box(this);
    msg_box.setText(question);
    msg_box.setWindowTitle(title);

    if(buttons.size() == 0)
    {
        QMessageBox::StandardButtons buttons = (QMessageBox::Yes | QMessageBox::No);

        if(with_cancel)
            buttons |= QMessageBox::Cancel;
        msg_box.setStandardButtons(buttons);
    }
    else
    {
        msg_box.addButton(buttons[0], QMessageBox::YesRole);

        if(buttons.size() > 1)
            msg_box.addButton(buttons[1], QMessageBox::NoRole);

        if(buttons.size() > 2)
            msg_box.addButton(buttons[2], QMessageBox::RejectRole);
    }
    msg_box.setDefaultButton(QMessageBox::Yes);
    msg_box.setIcon(icon);

    int result = msg_box.exec();

    return result;
}

#ifdef QT_WIN
QChar MainWindow::get_add_key() const
{
    return add_hook_key;
}
#endif

void MainWindow::process_add()
{
    if(
#ifdef QT_WIN
    focus_window_handle == (HWND)winId() ||
#endif
    focus_window_title.isEmpty())
        return;

    QTimer::singleShot(100, this, &MainWindow::slot_process_add);
}
#include <QThread>
void MainWindow::slot_process_add()
{
#ifdef QT_WIN
    unhook_global_keyboard_event();
#endif
#ifdef QT_LINUX
    lnx_ignore_events();
#endif
    hide_notetabs();

    if(note_edit_window)
        delete note_edit_window;

#ifdef QT_WIN
    GetWindowRect(focus_window_handle, (LPRECT)&focus_window_rect);
#endif

    note_edit_window = new NoteEdit();
    NoteData note_data;

    ContextsListIter iter;
    for(iter = contexts_list.begin();iter != contexts_list.end();iter++)
        note_data.current_contexts.append((*iter)->context_id);

    if(!current_context)
        current_context = locate_context();
    if(current_context)
    {
        note_data.context = current_context->context_id;
        note_data.regexp = current_context->id_is_expression;
    }
    else
        note_data.context = focus_window_title;
    note_data.context_icon = next_tab_icon;
    note_edit_window->set_note_data(note_data);

    const QRect& r = note_edit_window->geometry();

#ifdef QT_WIN
    auto win_left = focus_window_rect.left;
    auto win_top = focus_window_rect.top;
#endif
#ifdef QT_LINUX
    auto win_left = focus_window_rect.left();
    auto win_top = focus_window_rect.top();
#endif

    note_edit_window->setGeometry(win_left + 10, win_top + 20, r.width(), r.height());
    note_edit_window->setFixedSize(r.width(), r.height());
    bool connected = connect(note_edit_window, &NoteEdit::signal_lost_focus, this, &MainWindow::slot_close_add_window);
    ASSERT_UNUSED(connected);

    os_play_sound(SOUND_NOTEOPEN);
    note_edit_window->show();
#ifdef QT_WIN
    make_active_window(note_edit_window->winId());
#endif
#ifdef QT_LINUX
    note_edit_window->raise();
    note_edit_window->setFocus();
#endif
}

void MainWindow::slot_close_add_window()
{
    NoteData note_data;
    note_edit_window->get_note_data(note_data);

    note_edit_window->close();

    os_play_sound(SOUND_NOTECLOSE);

    note_edit_window->deleteLater();
    note_edit_window = (NoteEdit*)0;

    if(note_data.content.size() && note_data.context.size())
    {
        if(!current_context)
            current_context = locate_context();
        if(!current_context)
        {
            QDomElement root = note_database->documentElement();
            QDomElement context = note_database->createElement("Context");
            context.setAttribute("id", note_data.context);
            context.setAttribute("tab_icon", next_tab_icon);
            context.setAttribute("is_regexp", note_data.regexp ? 1 : 0);
            root.appendChild(context);

            current_context = new Context(note_database, context);
            contexts_list.append(current_context);

            if(++next_tab_icon > MAX_TAB_ICONS)
                next_tab_icon = 1;
        }

        current_context->add_note(note_data);

        save_note_database();
        save_application_settings();
    }

    arrange_notetabs();
    display_notetabs();

#ifdef QT_WIN
    hook_global_keyboard_event();
#endif
#ifdef QT_LINUX
    lnx_regard_events();
#endif
}

void MainWindow::slot_edit_options()
{
    ui->tab_Main->setCurrentIndex(MAINTAB_SETTINGS);
    slot_restore();
}

void MainWindow::slot_restore()
{
#ifdef QT_WIN
    unhook_global_keyboard_event();
#endif
#ifdef QT_LINUX
    lnx_ignore_events();
#endif

    populate_note_tree();
    bool connected = connect(ui->tree_Database, &QTreeWidget::itemChanged, this, &MainWindow::slot_edit_finished);
    ASSERT_UNUSED(connected);

    restore_window_data(this);
    showNormal();
    activateWindow();
    raise();
}

void MainWindow::slot_about()
{
    ui->tab_Main->setCurrentIndex(MAINTAB_ABOUT);
    slot_restore();
}

void MainWindow::slot_icon_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::DoubleClick:
            ui->tab_Main->setCurrentIndex(MAINTAB_SETTINGS);
            slot_restore();
            break;

        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::MiddleClick:
        default:
            break;
    }
}

void MainWindow::slot_message_clicked()
{
    QMessageBox::information(0,
                             tr("WindowNotes"),
                             tr("Sorry, I already gave what help I could.\n"
                                "Maybe you should try asking a human?"));
}

void MainWindow::slot_clear_database()
{
    QString question(tr("Are you sure you wish to completely remove all your"
                        "<br>current database entries?  This cannot be undone!"));

    switch(ask_question(question, false, tr("WindowNotes: Clear Database"), QMessageBox::Warning))
    {
        case QDialog::Rejected:
        case QMessageBox::No:
        case QMessageBox::Cancel:
            return;
    }
}

void MainWindow::slot_menu_action(QAction* action)
{
    if(action == options_action)
    {
        slot_edit_options();
    }
    else if(action == paste_action)
    {
        if(!current_context)
            current_context = locate_context();
        if(!current_context)
        {
            QDomElement root = note_database->documentElement();
            QDomElement context = note_database->createElement("Context");
            context.setAttribute("id", focus_window_title);
            context.setAttribute("tab_icon", next_tab_icon);
            context.setAttribute("is_regexp", 0);
            root.appendChild(context);

            current_context = new Context(note_database, context);
            contexts_list.append(current_context);

            if(++next_tab_icon > MAX_TAB_ICONS)
                next_tab_icon = 1;
        }

        current_context->add_note(*note_clipboard);

        save_note_database();
        save_application_settings();

        arrange_notetabs();
        display_notetabs();
    }
    else if(action == about_action)
    {
        slot_about();
    }
    else if(action == quit_action)
    {
        slot_quit();
    }
}

void MainWindow::build_tray_menu()
{
    if(trayIconMenu)
    {
        disconnect(trayIconMenu, &QMenu::triggered, this, &MainWindow::slot_menu_action);
        delete trayIconMenu;
    }

    options_action  = new QAction(QIcon(":/images/Options.png"), tr("Edit &Settings..."), this);
    if(note_clipboard)
        paste_action = new QAction(QIcon(":/images/Restore.png"), tr("&Paste Note"), this);
    about_action    = new QAction(QIcon(":/images/About.png"), tr("&About"), this);
    quit_action     = new QAction(QIcon(":/images/Quit.png"), tr("&Quit"), this);

    trayIconMenu = new QMenu(this);

    trayIconMenu->addAction(options_action);
    trayIconMenu->addSeparator();
    if(note_clipboard)
    {
        trayIconMenu->addAction(paste_action);
        trayIconMenu->addSeparator();
    }
    trayIconMenu->addAction(about_action);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quit_action);

    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIconMenu, &QMenu::triggered, this, &MainWindow::slot_menu_action);
}

void MainWindow::load_application_settings()
{
    window_data.clear();

    QString settings_file_name;
    auto config_location = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
    settings_file_name = QDir::toNativeSeparators(QString("%1/WindowNotes.ini").arg(config_location[0]));
    QSettings settings(settings_file_name, QSettings::IniFormat);

    next_tab_icon       = settings.value("next_tab_icon", next_tab_icon).toInt();
    backup_database     = settings.value("backup_database", backup_database).toBool();
    max_backups         = settings.value("max_backups", max_backups).toUInt();
#ifdef QT_WIN
    add_hook_key        = settings.value("add_hook_key", add_hook_key).toChar().toLatin1();
#endif
    start_automatically = settings.value("start_automatically", start_automatically).toBool();
    selected_opacity    = settings.value("selected_opacity", selected_opacity).toDouble();
    unselected_opacity  = settings.value("unselected_opacity", unselected_opacity).toDouble();
    favored_side        = settings.value("favored_side", favored_side).toInt();

    clipboard_ttl = settings.value("clipboard_ttl", clipboard_ttl).toBool();
    clipboard_ttl_timeout = settings.value("clipboard_ttl_timeout", clipboard_ttl_timeout).toInt();

    enable_sound_effects= settings.value("enable_sound_effects", enable_sound_effects).toBool();

    sound_files.resize(SOUND_MAX);

    int soundfiles_size = settings.beginReadArray("sound_files");
    if(soundfiles_size)
    {
        for(int i = 0; i < soundfiles_size; ++i)
        {
            settings.setArrayIndex(i);
            sound_files[i] = settings.value(QString("sound_file_%1").arg(i + 1)).toString();
        }
    }

    settings.endArray();

    int windata_size = settings.beginReadArray("window_data");
    if(windata_size)
    {
        for(int i = 0; i < windata_size; ++i)
        {
            settings.setArrayIndex(i);

            QString key = settings.value("key").toString();
            window_data[key] = settings.value("geometry").toByteArray();
        }
    }
    settings.endArray();

    ui->check_Start_Automatically->setChecked(start_automatically);
#ifdef QT_WIN
    ui->edit_Add_Key->setText(QString(add_hook_key));
#endif
    ui->spin_Selected_Opacity->setValue((int)(selected_opacity * 100.0));
    ui->spin_Unselected_Opacity->setValue((int)(unselected_opacity * 100.0));

    switch(favored_side)
    {
        case FAVOR_LEFT:
            ui->radio_Position_Left->setChecked(true);
            break;
        case FAVOR_TOP:
            ui->radio_Position_Top->setChecked(true);
            break;
        case FAVOR_RIGHT:
            ui->radio_Position_Right->setChecked(true);
            break;
        case FAVOR_BOTTOM:
            ui->radio_Position_Bottom->setChecked(true);
            break;
    }

        os_set_startup();

    settings_modified = !QFile::exists(settings_file_name);
}

void MainWindow::save_application_settings()
{
    QString settings_file_name;
    auto config_location = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
    settings_file_name = QDir::toNativeSeparators(QString("%1/WindowNotes.ini").arg(config_location[0]));
    QSettings settings(settings_file_name, QSettings::IniFormat);

    settings.clear();

    settings.setValue("next_tab_icon", next_tab_icon);
    settings.setValue("backup_database", backup_database);
    settings.setValue("max_backups", max_backups);
#ifdef QT_WIN
    settings.setValue("add_hook_key", QChar(add_hook_key));
#endif
    settings.setValue("start_automatically", start_automatically);
    settings.setValue("selected_opacity", selected_opacity);
    settings.setValue("unselected_opacity", unselected_opacity);
    settings.setValue("favored_side", favored_side);

    settings.setValue("clipboard_ttl", clipboard_ttl);
    settings.setValue("clipboard_ttl_timeout", clipboard_ttl_timeout);

    settings.setValue("enable_sound_effects", enable_sound_effects);
    settings.beginWriteArray("sound_files");
      quint32 item_index = 0;
      foreach(QString key, sound_files)
      {
          settings.setArrayIndex(item_index++);
          settings.setValue(QString("sound_file_%1").arg(item_index), sound_files[item_index-1]);
      }
    settings.endArray();

    if(window_data.size())
    {
        settings.beginWriteArray("window_data");
          quint32 item_index = 0;
          QList<QString> keys = window_data.keys();
          foreach(QString key, keys)
          {
              settings.setArrayIndex(item_index++);

              settings.setValue("key", key);
              settings.setValue("geometry", window_data[key]);
          }
        settings.endArray();
    }

    settings_modified = false;
}

void MainWindow::load_note_database()
{
    if(note_database)
        delete note_database;

    note_database = new QDomDocument("WindowNotes");
    QDomElement root = note_database->createElement("WindowNotes");
    root.setAttribute("version", settings_version);

    note_database->appendChild(root);

    QDomNode node(note_database->createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF8\""));
    note_database->insertBefore(node, note_database->firstChild());

    QString database_file_name;
    auto config_location = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
    database_file_name = QDir::toNativeSeparators(QString("%1/WindowNotes.xml").arg(config_location[0]));

    QFile file(database_file_name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QString errorStr, versionStr = settings_version;
    int errorLine;
    int errorColumn;

    if(!note_database->setContent(&file, true, &errorStr, &errorLine, &errorColumn))
    {
        QMessageBox::information(window(), tr("WindowNotes"),
                                 tr("Notes database parse error at line %1, column %2:\n%3")
                                 .arg(errorLine)
                                 .arg(errorColumn)
                                 .arg(errorStr));
        return;
    }

    root = note_database->documentElement();
    if(root.tagName() != "WindowNotes")
    {
        QMessageBox::information(window(), tr("WindowNotes"),
                                 tr("The file 'WindowNotes.xml' is not an WindowNotes database file."));
        return;
    }
    else if(root.hasAttribute("version"))
        versionStr = root.attribute("version");

    if(versionStr == settings_version)
    {
        QDomNodeList children = root.childNodes();
        for(int i = 0;i < children.length();i++)
        {
            QDomNode node = children.at(i);
            if(node.nodeName() == "Context")
            {
                contexts_list.append(new Context(note_database, node.toElement()));

                QDomNodeList children = node.childNodes();
                for(int i = 0;i < children.length();i++)
                {
                    QDomNode node = children.at(i);
                    if(node.nodeName() == "Note")
                    {
                        QDomElement element = node.toElement();
                        //if(element.hasAttribute("use_mask") && element.attribute("use_mask").toInt() == 1)
                        //    ++mask_count;
                    }
                }
            }
        }
    }
}

void MainWindow::roll_database_backups(const QString& database_file_name)
{
    QString file_name_source = QString("%1.%2").arg(database_file_name).arg(max_backups);

    if(QFile::exists(file_name_source))
        QFile::remove(file_name_source);

    for(int i = max_backups;i > 1;i--)
    {
        file_name_source = QString("%1.%2").arg(database_file_name).arg(i - 1);
        if(QFile::exists(file_name_source))
        {
            QString file_name_target = QString("%1.%2").arg(database_file_name).arg(i);
            QFile::rename(file_name_source, file_name_target);
        }
    }
}

void MainWindow::purge_empty_contexts()
{
    QList<QDomNode> node_list;

    QDomElement root = note_database->documentElement();
    QDomNodeList children = root.childNodes();
    for(int i = 0;i < children.length();i++)
    {
        QDomNode node = children.at(i);
        if(node.nodeName() == "Context")
        {
            QDomNodeList notes = node.childNodes();
            if(notes.length() == 0)
                node_list.push_back(node);
        }
    }

    if(node_list.length())
    {
        foreach(QDomNode node, node_list)
            root.removeChild(node);

        notes_modified = true;
    }
}

void MainWindow::save_note_database()
{
    const int IndentSize = 2;

    auto config_location = QStandardPaths::standardLocations(QStandardPaths::AppConfigLocation);
    QString database_file_name = QDir::toNativeSeparators(QString("%1/WindowNotes.xml").arg(config_location[0]));

    // if directed, make a backup before we overwrite it, and
    // then remove the backup if the write is successful

    if(backup_database && QFile::exists(database_file_name))
    {
        roll_database_backups(database_file_name);
        QString backup_file_name = QString("%1.1").arg(database_file_name);
        QFile::copy(database_file_name, backup_file_name);
    }

    QFile file(database_file_name);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
        return;

    QTextStream out;
    out.setDevice(&file);
    out.setCodec("UTF-8");

    note_database->save(out, IndentSize);

    if(file.error() != QFileDevice::NoError)
    {
        // TODO: handle this error
    }

    notes_modified = false;
}

#ifdef QT_WIN
void MainWindow::slot_validate_add_key()
{
    if(in_validate_key)
        return;

    in_validate_key = true;

    auto t = ui->edit_Add_Key->text();
    add_hook_key = t[0].toUpper();
    ui->edit_Add_Key->clear();

    ui->edit_Add_Key->setText(add_hook_key);
    ui->edit_Add_Key->setCursorPosition(0);

    save_application_settings();

    in_validate_key = false;
}
#endif

void MainWindow::slot_toggle_startup()
{
    bool new_start = ui->check_Start_Automatically->isChecked();
    if(new_start == start_automatically)
        return;

    start_automatically = new_start;
    os_set_startup();

    save_application_settings();
}

void MainWindow::slot_selected_opacity_changed()
{
    selected_opacity = ui->spin_Selected_Opacity->value() / 100.0;
    settings_modified = true;
}

void MainWindow::slot_unselected_opacity_changed()
{
    unselected_opacity = ui->spin_Unselected_Opacity->value() / 100.0;
    settings_modified = true;
}

void MainWindow::slot_select_position_left()
{
    favored_side = FAVOR_LEFT;
    settings_modified = true;
}

void MainWindow::slot_select_position_top()
{
    favored_side = FAVOR_TOP;
    settings_modified = true;
}

void MainWindow::slot_select_position_right()
{
    favored_side = FAVOR_RIGHT;
    settings_modified = true;
}

void MainWindow::slot_select_position_bottom()
{
    favored_side = FAVOR_BOTTOM;
    settings_modified = true;
}

void MainWindow::set_sound_states()
{
    ui->edit_Delete_Note_Sound->setEnabled(enable_sound_effects);
    ui->button_Browse_For_Delete_Sound->setEnabled(enable_sound_effects);

    ui->edit_Copy_Note_Sound->setEnabled(enable_sound_effects);
    ui->button_Browse_For_Copy_Sound->setEnabled(enable_sound_effects);

    ui->edit_Open_Edit_Sound->setEnabled(enable_sound_effects);
    ui->button_Browse_For_Open_Sound->setEnabled(enable_sound_effects);

    ui->edit_Close_Edit_Sound->setEnabled(enable_sound_effects);
    ui->button_Browse_For_Close_Sound->setEnabled(enable_sound_effects);
}

void MainWindow::slot_enable_sound_effects()
{
    enable_sound_effects = !enable_sound_effects;
    set_sound_states();
    settings_modified = true;
}

void MainWindow::recache_sound(int sound, SoundLineEdit* control)
{
    QString text = control->adjusted_text();
    if(text != sound_files[sound])
    {
        sound_files[sound] = text;
        QString sound_file_name;
        if(text.length())
            sound_file_name = text;
        else
            sound_file_name = QString(":/sounds/%1.wav").arg(sound + 1);

        QFile sound_file(sound_file_name);
        if(sound_file.open(QIODevice::ReadOnly))
        {
            sound_cache[sound] = sound_file.readAll();
            sound_file.close();
        }

        settings_modified = true;
    }
}

void MainWindow::slot_delete_sound_file_changed()
{
    recache_sound(SOUND_DELETE, ui->edit_Delete_Note_Sound);
}

void MainWindow::slot_copy_sound_file_changed()
{
    recache_sound(SOUND_COPY, ui->edit_Copy_Note_Sound);
}

void MainWindow::slot_open_sound_file_changed()
{
    recache_sound(SOUND_NOTEOPEN, ui->edit_Open_Edit_Sound);
}

void MainWindow::slot_close_sound_file_changed()
{
    recache_sound(SOUND_NOTECLOSE, ui->edit_Close_Edit_Sound);
}

void MainWindow::cache_sounds()
{
    sound_cache.clear();

    for(int i = 0;i < SOUND_MAX;i++)
    {
        QString sound_file_name;
        if(sound_files[i].length())
            sound_file_name = sound_files[i];
        else
            sound_file_name = QString(":/sounds/%1.wav").arg(i + 1);

        QFile sound_file(sound_file_name);
        if(sound_file.open(QIODevice::ReadOnly))
        {
            sound_cache.append(sound_file.readAll());
            sound_file.close();
        }
    }
}

void MainWindow::slot_clear_delete_sound()
{
    ui->edit_Delete_Note_Sound->clear();
    recache_sound(SOUND_DELETE, ui->edit_Delete_Note_Sound);
    set_sound_states();
    settings_modified = true;
}

void MainWindow::slot_clear_copy_sound()
{
    ui->edit_Copy_Note_Sound->clear();
    recache_sound(SOUND_COPY, ui->edit_Copy_Note_Sound);
    set_sound_states();
    settings_modified = true;
}

void MainWindow::slot_clear_open_sound()
{
    ui->edit_Open_Edit_Sound->clear();
    recache_sound(SOUND_NOTEOPEN, ui->edit_Open_Edit_Sound);
    set_sound_states();
    settings_modified = true;
}

void MainWindow::slot_clear_close_sound()
{
    ui->edit_Close_Edit_Sound->clear();
    recache_sound(SOUND_NOTECLOSE, ui->edit_Close_Edit_Sound);
    set_sound_states();
    settings_modified = true;
}

void MainWindow::slot_browse_for_delete_sound()
{
    QString current_file = ui->edit_Delete_Note_Sound->adjusted_text();
    QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Select WAV File..."), current_file, tr("Sound File (*.wav)")));
    if(file.isEmpty() || file == sound_files[SOUND_DELETE])
        return;
    ui->edit_Delete_Note_Sound->setText(file);
    recache_sound(SOUND_DELETE, ui->edit_Delete_Note_Sound);
    set_sound_states();
    settings_modified = true;
}

void MainWindow::slot_browse_for_copy_sound()
{
    QString current_file = ui->edit_Copy_Note_Sound->adjusted_text();
    QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Select WAV File..."), current_file, tr("Sound File (*.wav)")));
    if(file.isEmpty() || file == sound_files[SOUND_COPY])
        return;
    ui->edit_Copy_Note_Sound->setText(file);
    recache_sound(SOUND_COPY, ui->edit_Copy_Note_Sound);
    set_sound_states();
    settings_modified = true;
}

void MainWindow::slot_browse_for_open_sound()
{
    QString current_file = ui->edit_Open_Edit_Sound->adjusted_text();
    QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Select WAV File..."), current_file, tr("Sound File (*.wav)")));
    if(file.isEmpty() || file == sound_files[SOUND_NOTEOPEN])
        return;
    ui->edit_Open_Edit_Sound->setText(QDir::toNativeSeparators(file));
    recache_sound(SOUND_NOTEOPEN, ui->edit_Open_Edit_Sound);
    set_sound_states();
    settings_modified = true;
}

void MainWindow::slot_browse_for_close_sound()
{
    QString current_file = ui->edit_Close_Edit_Sound->adjusted_text();
    QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Select WAV File..."), current_file, tr("Sound File (*.wav)")));
    if(file.isEmpty() || file == sound_files[SOUND_NOTECLOSE])
        return;
    ui->edit_Close_Edit_Sound->setText(file);
    recache_sound(SOUND_NOTECLOSE, ui->edit_Close_Edit_Sound);
    set_sound_states();
    settings_modified = true;
}

void MainWindow::slot_process_link(const QUrl& url)
{
    slot_process_label_link(url.toString());
}

void MainWindow::slot_process_label_link(const QString& url)
{
    QProcess::startDetached("cmd", QStringList() << "/c" << "start" << url);
}

void MainWindow::slot_toggle_clipboard_ttl()
{
    clipboard_ttl = ui->check_ClipboardTTL->isChecked();
    ui->edit_ClipboardTTL->setEnabled(clipboard_ttl);

    save_application_settings();
}

void MainWindow::slot_clipboard_ttl_timeout_changed(const QString& value)
{
    clipboard_ttl_timeout = value.toInt();
    save_application_settings();
}

void MainWindow::slot_clipboard_ttl()
{
    if(!clipboard_ttl || !clipboard_ttl_timeout || !clipboard_timestamp)
        return;

    if((QDateTime::currentMSecsSinceEpoch() - clipboard_timestamp) > (clipboard_ttl_timeout * 1000))
    {
        QClipboard* clipboard = qApp->clipboard();
        clipboard->setText(QString(""));
        clipboard_timestamp = 0L;
        clipboard_ttl_timeout = ui->edit_ClipboardTTL->text().toInt();
    }
}

Context* MainWindow::locate_context(bool literal)
{
    if(!focus_window_title.isEmpty())
    {
        ContextsListIter iter;
        for(iter = contexts_list.begin();iter != contexts_list.end();iter++)
        {
            Context& context = *(*iter);
            if(literal)
            {
                if(context == focus_window_title)
                    return &context;
            }
            else
            {
                if(context.match(focus_window_title))
                    return &context;
            }
        }
    }

    return (Context*)0;
}

Context* MainWindow::locate_context(const QString& context_id)
{
    QString old_focus_window_title = focus_window_title;
    focus_window_title = context_id;
    Context* context = locate_context(true);
    focus_window_title = old_focus_window_title;

    return context;
}

void MainWindow::populate_note_tree()
{
    QTreeWidgetItem* item;
    while(ui->tree_Database->topLevelItemCount())
    {
        item = ui->tree_Database->takeTopLevelItem(0);
        delete item;
    }
    context_map.clear();

    QDomElement root = note_database->documentElement();
    QDomNodeList children = root.childNodes();
    for(int i = 0;i < children.length();++i)
    {
        QDomNode node = children.at(i);
        if(!node.nodeName().compare("Context"))
        {
            QDomElement element = node.toElement();
            if(!element.isNull() && element.hasAttribute("id"))
            {
                QString context_id = element.attribute("id");
                bool id_is_expression = false;
                if(element.hasAttribute("is_regexp") && element.attribute("is_regexp").toInt())
                    id_is_expression = true;

                item = new QTreeWidgetItem(ui->tree_Database, QStringList() << context_id << (id_is_expression ? tr("Yes") : tr("No")));
                item->setFlags(item->flags() | Qt::ItemIsEditable);

                QVariant variant;
                variant.setValue(node);
                item->setData(0, Qt::UserRole, variant);

                QComboBox* combo = new QComboBox(this);
                combo->addItems(QStringList() << tr("Yes") << tr("No"));
                combo->setCurrentIndex(id_is_expression ? COMBO_YES : COMBO_NO);
                bool connected = connect(combo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                                         this, &MainWindow::slot_combo_changed);
                ASSERT_UNUSED(connected);

                ui->tree_Database->setItemWidget(item, 1, combo);
                context_map[combo] = item;

                QDomNodeList children = element.childNodes();
                for(int j = 0;j < children.length();++j)
                {
                    QDomNode _node = children.at(j);
                    if(!_node.nodeName().compare("Note"))
                    {
                        QDomElement _element = _node.toElement();
                        QString value = _element.firstChild().nodeValue();
                        // make carriage return and newlines visible and editable
                        value.replace("\r", "\\r");
                        value.replace("\n", "\\n");
                        QTreeWidgetItem* _item = new QTreeWidgetItem(item, QStringList() << value);
                        _item->setFlags(item->flags() | Qt::ItemIsEditable);

                        QVariant variant;
                        variant.setValue(_node);
                        _item->setData(0, Qt::UserRole, variant);
                    }
                }
            }
        }
    }

    const QRect& r = ui->tree_Database->geometry();
    ui->tree_Database->setColumnWidth(0, r.width() * .8);

    //for(int i = 0;i < ui->tree_Database->columnCount();++i)
    //    ui->tree_Database->resizeColumnToContents(i);
}

void MainWindow::slot_show_database_context_menu(const QPoint &pos)
{
    context_item = ui->tree_Database->itemAt(pos);
    if(!context_item)
        return;

    QDomNode context_element = context_item->data(0, Qt::UserRole).value<QDomNode>();
    bool is_context = (context_element.nodeName().compare("Context") == 0);

    QMenu menu(tr("Context menu"), this);
    QAction* action;

    bool connected = false;
    if(is_context)
    {
        action = menu.addAction(QIcon(":/images/Remove.png"), tr("Remove Context"));
        connected = connect(action, &QAction::triggered, this, &MainWindow::slot_database_delete_context);
    }
    else
    {
        action = menu.addAction(QIcon(":/images/Remove.png"), tr("Remove Note"));
        connected = connect(action, &QAction::triggered, this, &MainWindow::slot_database_delete_note);

        action = menu.addAction(QIcon(":/images/Edit.png"), tr("Copy to Clipboard"));
        connected = connect(action, &QAction::triggered, this, &MainWindow::slot_database_copy_note);
    }
    ASSERT_UNUSED(connected);

    menu.exec(ui->tree_Database->viewport()->mapToGlobal(pos));
}

void MainWindow::slot_database_delete_context()
{
    QDomNode dom_node = context_item->data(0, Qt::UserRole).value<QDomNode>();
    QDomElement dom_element = dom_node.toElement();
    Context* context = locate_context(dom_element.attribute("id"));

    context->clear_notes();
    contexts_list.removeAll(context);

    QDomElement root = note_database->documentElement();
    root.removeChild(context->get_node());

    delete context;

    ui->tree_Database->takeTopLevelItem(ui->tree_Database->indexOfTopLevelItem(context_item));
    delete context_item;

    save_note_database();
}

void MainWindow::slot_database_delete_note()
{
    Context* context = nullptr;
    TreeItem context_tree_item = context_item->parent();

    {
        QDomNode dom_node = context_tree_item->data(0, Qt::UserRole).value<QDomNode>();
        QDomElement dom_element = dom_node.toElement();
        context = locate_context(dom_element.attribute("id"));
    }

    QDomNode dom_node = context_item->data(0, Qt::UserRole).value<QDomNode>();
    QDomElement dom_element = dom_node.toElement();

    if(context->remove_note(dom_element) == 0)
    {
        contexts_list.removeAll(context);

        QDomElement root = note_database->documentElement();
        root.removeChild(context->get_node());

        delete context;

        ui->tree_Database->takeTopLevelItem(ui->tree_Database->indexOfTopLevelItem(context_tree_item));
        delete context_tree_item;
    }
    else
    {
        context_tree_item->takeChild(context_tree_item->indexOfChild(context_item));
        delete context_item;
    }

    save_note_database();
}

void MainWindow::slot_database_copy_note()
{
    os_play_sound(SOUND_COPY);

    QDomNode dom_node = context_item->data(0, Qt::UserRole).value<QDomNode>();
    QDomElement note_node = dom_node.toElement();

    QClipboard* clipboard = qApp->clipboard();
    clipboard->setText(note_node.firstChild().nodeValue());
}

void MainWindow::slot_edit_assign(QTreeWidgetItem* item, int col)
{
    edit_old_value = item->text(col);
    ui->tree_Database->editItem(item, col);
}

void MainWindow::slot_edit_finished(QTreeWidgetItem* item, int col)
{
    QDomNode dom_node = item->data(0, Qt::UserRole).value<QDomNode>();
    QString edit_new_value = item->text(col);
    if(edit_new_value.isEmpty())
    {
        item->setText(col, edit_old_value);
        QTimer::singleShot(100, [this]() {
                QMessageBox::warning(this, tr("WindowNotes"),
                                           tr("Please use the context menu to remove items\nfrom the database."));
            });
    }
    else
    {
        if(!dom_node.nodeName().compare("Context"))
        {
            QDomElement dom_element = dom_node.toElement();
            Context* context = locate_context(edit_old_value);
            if(col == 0)
            {
                if(context->id_is_expression)
                {
                    QRegExp context_expr(edit_new_value);
                    if(!context_expr.isValid())
                        edit_new_value = edit_old_value;
                    else
                    {
                        context->context_expr.setPattern(edit_new_value);
                        context->context_id = edit_new_value;
                        dom_element.setAttribute("id", edit_new_value);
                    }
                }
                else
                    context->context_id = edit_new_value;
            }
        }
        else
        {
            QString new_value = edit_new_value;
            // convert symbolic values back to raw before storing in the database
            new_value.replace("\\r", "\r");
            new_value.replace("\\n", "\n");
            dom_node.firstChild().setNodeValue(new_value);
        }

        item->setText(col, edit_new_value);
    }

    save_note_database();
}

void MainWindow::slot_combo_changed(int index)
{
    QComboBox *combo = qobject_cast<QComboBox *>(sender());
    QTreeWidgetItem* item = context_map[combo];
    QDomNode dom_node = item->data(0, Qt::UserRole).value<QDomNode>();
    QDomElement dom_element = dom_node.toElement();
    Context* context = locate_context(dom_element.attribute("id"));

    if(index == COMBO_YES)
    {
        if(!context->id_is_expression)
        {
            context->id_is_expression = true;
            QRegExp context_expr(item->text(0));
            if(!context_expr.isValid())
                context->id_is_expression = false;
        }
    }
    else
        context->id_is_expression = false;

    dom_element.setAttribute("is_regexp", context->id_is_expression ? "1" : "0");

    combo->setCurrentIndex(context->id_is_expression ? COMBO_YES : COMBO_NO);

    save_note_database();
}
