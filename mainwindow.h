#pragma once

#ifdef QT_WIN
#define WIN32_MEAN_AND_LEAN // necessary to avoid compiler errors
#include <windows.h>
#endif

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QAction>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QComboBox>
#include <QtGui/QCloseEvent>
#include <QtGui/QMouseEvent>
#include <QtCore/QPoint>
#include <QtCore/QString>
#include <QtCore/QMap>
#include <QtCore/QTime>
#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include <QtCore/QByteArray>
#include <QtCore/QTimer>

#ifdef QT_LINUX
// #include <X11/Xlib.h>
// #include <X11/Xutil.h>
#include "linux/WindowEvents.h"
#endif

#include "types.h"
#include "context.h"
#include "notetab.h"
#include "noteedit.h"
#include "mimedata.h"

#define AS_LPCWSTR(str) (const wchar_t*)str.utf16()
#define AS_LPBYTE(str) (const LPBYTE)str.utf16()

const int HousekeepingTimeout = 5000;

class SoundLineEdit;
class QTimer;

namespace Ui
{
    class MainWindow;
}

/// @class MainWindow
/// @brief The main window for WindowNotes

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:     // methods
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void            set_visible(bool visible);

    void            add_log_entry(const QString& col1, const QString& col2 = QString(), const QString& col3 = QString());
#ifdef QT_WIN
    bool            process_win_msg(MSG* msg, long* result);
    void            process_win_event(DWORD event, HWND win_id);
#endif
    void            process_add();

    QChar           get_add_key() const;

protected:  // methods
    void            closeEvent(QCloseEvent *event);

private slots:
    void            slot_tab_entered(NoteTab* id);
    void            slot_tab_exited(NoteTab* id);
    void            slot_tab_LMB_down(NoteTab* id, QMouseEvent* event);
    void            slot_create_data(const QString &mime_type);
    void            slot_tab_LMB_move(NoteTab* id, QMouseEvent* event);
    void            slot_tab_LMB_up(NoteTab* id, QMouseEvent* event);
    void            slot_tab_RMB(NoteTab* id, QMouseEvent* event);
    void            slot_single_tab_faded(NoteTab* id);
    void            slot_multiple_tab_faded(NoteTab* id);
    void            slot_tab_flashed(NoteTab* id);

    void            slot_pu_edit_note();
    void            slot_pu_copy_note();
    void            slot_pu_delete_note();
    void            slot_pu_clear_notes();

    void            slot_edit_note();
    void            slot_close_edit_window();
    void            slot_close_add_window();

    void            slot_quit();
    void            slot_process_add();
    void            slot_edit_options();
    void            slot_restore();
    void            slot_icon_activated(QSystemTrayIcon::ActivationReason reason);
    void            slot_message_clicked();
    void            slot_menu_action(QAction* action);

    void            slot_clear_database();

    void            slot_about();
#ifdef QT_WIN
    void            slot_validate_add_key();
#endif
    void            slot_toggle_startup();
    void            slot_selected_opacity_changed();
    void            slot_unselected_opacity_changed();

    void            slot_select_position_left();
    void            slot_select_position_top();
    void            slot_select_position_right();
    void            slot_select_position_bottom();
    void            slot_enable_sound_effects();

    void            slot_delete_sound_file_changed();
    void            slot_clear_delete_sound();
    void            slot_browse_for_delete_sound();
    void            slot_copy_sound_file_changed();
    void            slot_clear_copy_sound();
    void            slot_browse_for_copy_sound();
    void            slot_open_sound_file_changed();
    void            slot_clear_open_sound();
    void            slot_browse_for_open_sound();
    void            slot_close_sound_file_changed();
    void            slot_clear_close_sound();
    void            slot_browse_for_close_sound();
    void            slot_process_link(const QUrl&);
    void            slot_process_label_link(const QString&);

    void            slot_toggle_clipboard_ttl();
    void            slot_clipboard_ttl_timeout_changed(const QString&);
    void            slot_clipboard_ttl();

    void            slot_export_notes();
    void            slot_import_notes();

    void            slot_housekeeping();

    void            slot_show_database_context_menu(const QPoint &point);

    void            slot_database_delete_context();
    void            slot_database_delete_note();
    void            slot_database_copy_note();

    void            slot_edit_assign(QTreeWidgetItem*, int);
    void            slot_edit_finished(QTreeWidgetItem*, int);
    void            slot_combo_changed(int);

#ifdef QT_LINUX
    void    slot_thread_error(QByteArray);
    void    slot_window_event(WindowEvents::Action, WindowEvents::WindowData);
#endif

private:    // typedefs and enums
    using WindowData = QMap<QString, QByteArray>;
    using AuthenticateContext = QMap<QString, bool>;
    using CommandMap = QMap<int, QString>;
    using TreeItem = QTreeWidgetItem*;
    using ContextMap = QMap<QComboBox*, QTreeWidgetItem*>;

    enum
    {
        NOTEACTION_NONE,
        NOTEACTION_EDIT,
        NOTEACTION_COPY,
        NOTEACTION_DELETE,
        NOTEACTION_DELETE_ALL,
    };

    enum
    {
        FAVOR_LEFT,
        FAVOR_TOP,
        FAVOR_RIGHT,
        FAVOR_BOTTOM,
    };

    enum
    {
        SIDE_LEFT,
        SIDE_BOTTOM,
        SIDE_RIGHT,
        SIDE_TOP,
    };

    enum
    {
        MAINTAB_SETTINGS,
        MAINTAB_ABOUT,
    };

    enum
    {
        SETTAB_ACTIVATION,
        SETTAB_TABS,
        SETTAB_SOUNDS,
        SETTAB_SECURITY,
    };

    enum
    {
        SOUND_DELETE,
        SOUND_COPY,
        SOUND_NOTEOPEN,
        SOUND_NOTECLOSE,
        SOUND_DROPPED,
        SOUND_MAX
    };

    enum
    {
        COMBO_YES,
        COMBO_NO
    };

private:    // methods
    bool            os_events_init();
    void            os_events_cleanup();
    bool            os_locate_instance();
    void            os_play_sound(int sound);
    void            os_set_startup();
    void            os_set_hooks();

    NoteTab*        add_notetab(int icon);
    NoteTab*        append_addtab();
    int             arrange_notetabs(bool hide_and_show=false);
    void            hide_notetabs();
    void            delete_notetabs();
    void            display_notetabs();

    void            save_window_data(QWidget* window);
    void            restore_window_data(QWidget* window);
    int             ask_question(const QString& question, bool with_cancel = false, const QString& title = "WindowNotes: Question", QMessageBox::Icon icon = QMessageBox::Question, QStringList buttons = QStringList());

    void            build_tray_menu();

    void            load_application_settings();
    void            save_application_settings();
    void            load_note_database();
    void            roll_database_backups(const QString& database_file_name);
    void            purge_empty_contexts();
    void            save_note_database();

    void            recache_sound(int sound, SoundLineEdit* control);
    void            cache_sounds();
    void            set_sound_states();

    Context*        locate_context(bool literal = false);
    Context*        locate_context(const QString& context_id);

    void            populate_note_tree();

#ifdef QT_WIN
    void            hook_global_keyboard_event();
    void            unhook_global_keyboard_event();

    void            hook_global_win_event();
    void            unhook_global_win_event();

    void            make_active_window(WId win_id);
#endif
#ifdef QT_LINUX
    void            lnx_ignore_events();
    void            lnx_regard_events();
#endif

private:    // data members
    Ui::MainWindow*     ui;

    QSystemTrayIcon*    trayIcon{nullptr};
    QMenu*              trayIconMenu{nullptr};

    QAction*            about_action{nullptr};
    QAction*            options_action{nullptr};
    QAction*            quit_action{nullptr};
    QAction*            copy_action{nullptr};
    QAction*            paste_action{nullptr};

    QAction*            toolbar_add_action{nullptr};
    QAction*            toolbar_edit_action{nullptr};
    QAction*            toolbar_remove_action{nullptr};

    QPoint              event_pos;

#ifdef QT_WIN
    HHOOK               keyboard_hook_handle{0};
    HHOOK               mouse_hook_handle{0};
    HHOOK               shell_hook_handle{0};
    HHOOK               cbt_hook_handle{0};
    HWINEVENTHOOK       win_movesize_hook_handle{0};
    HWINEVENTHOOK       win_minimize_hook_handle{0};
    RECT                focus_window_rect{0};
    unsigned int        WM_SHELLHOOKMESSAGE;
    CommandMap          shell_command_map;

    QChar               add_hook_key{DEFAULT_ADD_KEY};
    void                win_get_active_rect();
#endif
#ifdef QT_LINUX
    WindowEventsPtr     m_window_events;

    bool                report_events{true};
    uint                focus_window_handle{0};
#endif

    QRect               focus_window_rect;
    QString             focus_window_title;

    WindowData          window_data;

    bool                window_geometry_save_enabled{true};

    TabsList*           tabs_list{nullptr};
    NoteTab*            current_tab{nullptr};
    int                 note_action;

    Context*            current_context{nullptr};
    ContextsList        contexts_list;

    bool                in_context_menu{false};
    bool                notetab_clicked{false};
    bool                enable_sound_effects{true};

    double              selected_opacity{1.0}, unselected_opacity{0.6};

    NoteEdit*           note_edit_window{nullptr};

    QDomDocument*       note_database{nullptr};
    NoteData*           note_clipboard{nullptr};

    int                 next_tab_icon{1};

    bool                backup_database{true};
    int                 max_backups{5};

    bool                start_automatically{false};

    bool                in_validate_key{false};

    bool                settings_modified{false};
    bool                notes_modified{false};

    bool                tab_animating{false};

    int                 favored_side{FAVOR_LEFT};

    bool                clipboard_ttl{false};
    int                 clipboard_ttl_timeout{0};
    QTimer*             clipboard_timer{nullptr};
    qint64              clipboard_timestamp{0};

    QPoint              drag_start_position;
    NoteTab*            drag_notetab_id{nullptr};
    MimeData*           drag_mime_data{nullptr};
    QMap<NoteTab*, QString>    drag_temp_files;
    bool                did_drag_and_drop{false};

    QVector<QString>    sound_files;
    QVector<QByteArray> sound_cache;

    TreeItem            context_item;

    QString             edit_old_value;

    ContextMap          context_map;
};

Q_DECLARE_METATYPE(QDomNode);
