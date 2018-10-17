#define WIN32_MEAN_AND_LEAN //necessary to avoid compiler errors
#include <windows.h>

#include "mainwindow.h"

extern MainWindow* main_window;

//----------------------------------------------------------------------------
// Windows API-specific code

typedef QVector<HWND>           WinList;
typedef WinList::iterator       WinListIter;
typedef WinList::const_iterator WinListConstIter;

void MainWindow::events_init()
{
    focus_window_handle = GetTopWindow(NULL);

    //msg_command_map[WM_NULL] = "WM_NULL";
    //msg_command_map[WM_CREATE] = "WM_CREATE";
    //msg_command_map[WM_DESTROY] = "WM_DESTROY";
    //msg_command_map[WM_MOVE] = "WM_MOVE";
    //msg_command_map[WM_SIZE] = "WM_SIZE";
    //msg_command_map[WM_ACTIVATE] = "WM_ACTIVATE";
    //msg_command_map[WM_SETFOCUS] = "WM_SETFOCUS";
    //msg_command_map[WM_KILLFOCUS] = "WM_KILLFOCUS";
    //msg_command_map[WM_ENABLE] = "WM_ENABLE";
    //msg_command_map[WM_SETREDRAW] = "WM_SETREDRAW";
    //msg_command_map[WM_SETTEXT] = "WM_SETTEXT";
    //msg_command_map[WM_GETTEXT] = "WM_GETTEXT";
    //msg_command_map[WM_GETTEXTLENGTH] = "WM_GETTEXTLENGTH";
    //msg_command_map[WM_PAINT] = "WM_PAINT";
    //msg_command_map[WM_CLOSE] = "WM_CLOSE";
    //msg_command_map[WM_QUERYENDSESSION] = "WM_QUERYENDSESSION";
    //msg_command_map[WM_QUERYOPEN] = "WM_QUERYOPEN";
    //msg_command_map[WM_ENDSESSION] = "WM_ENDSESSION";
    //msg_command_map[WM_QUIT] = "WM_QUIT";
    //msg_command_map[WM_ERASEBKGND] = "WM_ERASEBKGND";
    //msg_command_map[WM_SYSCOLORCHANGE] = "WM_SYSCOLORCHANGE";
    //msg_command_map[WM_SHOWWINDOW] = "WM_SHOWWINDOW";
    //msg_command_map[WM_WININICHANGE] = "WM_WININICHANGE";
    //msg_command_map[WM_SETTINGCHANGE] = "WM_SETTINGCHANGE";
    //msg_command_map[WM_DEVMODECHANGE] = "WM_DEVMODECHANGE";
    //msg_command_map[WM_ACTIVATEAPP] = "WM_ACTIVATEAPP";
    //msg_command_map[WM_FONTCHANGE] = "WM_FONTCHANGE";
    //msg_command_map[WM_TIMECHANGE] = "WM_TIMECHANGE";
    //msg_command_map[WM_CANCELMODE] = "WM_CANCELMODE";
    //msg_command_map[WM_SETCURSOR] = "WM_SETCURSOR";
    //msg_command_map[WM_MOUSEACTIVATE] = "WM_MOUSEACTIVATE";
    //msg_command_map[WM_CHILDACTIVATE] = "WM_CHILDACTIVATE";
    //msg_command_map[WM_QUEUESYNC] = "WM_QUEUESYNC";
    //msg_command_map[WM_GETMINMAXINFO] = "WM_GETMINMAXINFO";
    //msg_command_map[WM_PAINTICON] = "WM_PAINTICON";
    //msg_command_map[WM_ICONERASEBKGND] = "WM_ICONERASEBKGND";
    //msg_command_map[WM_NEXTDLGCTL] = "WM_NEXTDLGCTL";
    //msg_command_map[WM_SPOOLERSTATUS] = "WM_SPOOLERSTATUS";
    //msg_command_map[WM_DRAWITEM] = "WM_DRAWITEM";
    //msg_command_map[WM_MEASUREITEM] = "WM_MEASUREITEM";
    //msg_command_map[WM_DELETEITEM] = "WM_DELETEITEM";
    //msg_command_map[WM_VKEYTOITEM] = "WM_VKEYTOITEM";
    //msg_command_map[WM_CHARTOITEM] = "WM_CHARTOITEM";
    //msg_command_map[WM_SETFONT] = "WM_SETFONT";
    //msg_command_map[WM_GETFONT] = "WM_GETFONT";
    //msg_command_map[WM_SETHOTKEY] = "WM_SETHOTKEY";
    //msg_command_map[WM_GETHOTKEY] = "WM_GETHOTKEY";
    //msg_command_map[WM_QUERYDRAGICON] = "WM_QUERYDRAGICON";
    //msg_command_map[WM_COMPAREITEM] = "WM_COMPAREITEM";
    //msg_command_map[WM_GETOBJECT] = "WM_GETOBJECT";
    //msg_command_map[WM_COMPACTING] = "WM_COMPACTING";
    //msg_command_map[WM_COMMNOTIFY] = "WM_COMMNOTIFY";
    //msg_command_map[WM_WINDOWPOSCHANGING] = "WM_WINDOWPOSCHANGING";
    //msg_command_map[WM_WINDOWPOSCHANGED] = "WM_WINDOWPOSCHANGED";
    //msg_command_map[WM_POWER] = "WM_POWER";
    //msg_command_map[WM_COPYDATA] = "WM_COPYDATA";
    //msg_command_map[WM_CANCELJOURNAL] = "WM_CANCELJOURNAL";
    //msg_command_map[WM_NOTIFY] = "WM_NOTIFY";
    //msg_command_map[WM_INPUTLANGCHANGEREQUEST] = "WM_INPUTLANGCHANGEREQUEST";
    //msg_command_map[WM_INPUTLANGCHANGE] = "WM_INPUTLANGCHANGE";
    //msg_command_map[WM_TCARD] = "WM_TCARD";
    //msg_command_map[WM_HELP] = "WM_HELP";
    //msg_command_map[WM_USERCHANGED] = "WM_USERCHANGED";
    //msg_command_map[WM_NOTIFYFORMAT] = "WM_NOTIFYFORMAT";
    //msg_command_map[WM_CONTEXTMENU] = "WM_CONTEXTMENU";
    //msg_command_map[WM_STYLECHANGING] = "WM_STYLECHANGING";
    //msg_command_map[WM_STYLECHANGED] = "WM_STYLECHANGED";
    //msg_command_map[WM_DISPLAYCHANGE] = "WM_DISPLAYCHANGE";
    //msg_command_map[WM_GETICON] = "WM_GETICON";
    //msg_command_map[WM_SETICON] = "WM_SETICON";
    //msg_command_map[WM_NCCREATE] = "WM_NCCREATE";
    //msg_command_map[WM_NCDESTROY] = "WM_NCDESTROY";
    //msg_command_map[WM_NCCALCSIZE] = "WM_NCCALCSIZE";
    //msg_command_map[WM_NCHITTEST] = "WM_NCHITTEST";
    //msg_command_map[WM_NCPAINT] = "WM_NCPAINT";
    //msg_command_map[WM_NCACTIVATE] = "WM_NCACTIVATE";
    //msg_command_map[WM_GETDLGCODE] = "WM_GETDLGCODE";
    //msg_command_map[WM_SYNCPAINT] = "WM_SYNCPAINT";
    //msg_command_map[WM_NCMOUSEMOVE] = "WM_NCMOUSEMOVE";
    //msg_command_map[WM_NCLBUTTONDOWN] = "WM_NCLBUTTONDOWN";
    //msg_command_map[WM_NCLBUTTONUP] = "WM_NCLBUTTONUP";
    //msg_command_map[WM_NCLBUTTONDBLCLK] = "WM_NCLBUTTONDBLCLK";
    //msg_command_map[WM_NCRBUTTONDOWN] = "WM_NCRBUTTONDOWN";
    //msg_command_map[WM_NCRBUTTONUP] = "WM_NCRBUTTONUP";
    //msg_command_map[WM_NCRBUTTONDBLCLK] = "WM_NCRBUTTONDBLCLK";
    //msg_command_map[WM_NCMBUTTONDOWN] = "WM_NCMBUTTONDOWN";
    //msg_command_map[WM_NCMBUTTONUP] = "WM_NCMBUTTONUP";
    //msg_command_map[WM_NCMBUTTONDBLCLK] = "WM_NCMBUTTONDBLCLK";
    //msg_command_map[WM_NCXBUTTONDOWN] = "WM_NCXBUTTONDOWN";
    //msg_command_map[WM_NCXBUTTONUP] = "WM_NCXBUTTONUP";
    //msg_command_map[WM_NCXBUTTONDBLCLK] = "WM_NCXBUTTONDBLCLK";
    //msg_command_map[WM_INPUT_DEVICE_CHANGE] = "WM_INPUT_DEVICE_CHANGE";
    //msg_command_map[WM_INPUT] = "WM_INPUT";
    //msg_command_map[WM_KEYFIRST] = "WM_KEYFIRST";
    //msg_command_map[WM_KEYDOWN] = "WM_KEYDOWN";
    //msg_command_map[WM_KEYUP] = "WM_KEYUP";
    //msg_command_map[WM_CHAR] = "WM_CHAR";
    //msg_command_map[WM_DEADCHAR] = "WM_DEADCHAR";
    //msg_command_map[WM_SYSKEYDOWN] = "WM_SYSKEYDOWN";
    //msg_command_map[WM_SYSKEYUP] = "WM_SYSKEYUP";
    //msg_command_map[WM_SYSCHAR] = "WM_SYSCHAR";
    //msg_command_map[WM_SYSDEADCHAR] = "WM_SYSDEADCHAR";
    //msg_command_map[WM_UNICHAR] = "WM_UNICHAR";
    //msg_command_map[WM_KEYLAST] = "WM_KEYLAST";
    //msg_command_map[WM_KEYLAST] = "WM_KEYLAST";
    //msg_command_map[WM_IME_STARTCOMPOSITION] = "WM_IME_STARTCOMPOSITION";
    //msg_command_map[WM_IME_ENDCOMPOSITION] = "WM_IME_ENDCOMPOSITION";
    //msg_command_map[WM_IME_COMPOSITION] = "WM_IME_COMPOSITION";
    //msg_command_map[WM_IME_KEYLAST] = "WM_IME_KEYLAST";
    //msg_command_map[WM_INITDIALOG] = "WM_INITDIALOG";
    //msg_command_map[WM_COMMAND] = "WM_COMMAND";
    //msg_command_map[WM_SYSCOMMAND] = "WM_SYSCOMMAND";
    //msg_command_map[WM_TIMER] = "WM_TIMER";
    //msg_command_map[WM_HSCROLL] = "WM_HSCROLL";
    //msg_command_map[WM_VSCROLL] = "WM_VSCROLL";
    //msg_command_map[WM_INITMENU] = "WM_INITMENU";
    //msg_command_map[WM_INITMENUPOPUP] = "WM_INITMENUPOPUP";
    //msg_command_map[WM_MENUSELECT] = "WM_MENUSELECT";
    //msg_command_map[WM_MENUCHAR] = "WM_MENUCHAR";
    //msg_command_map[WM_ENTERIDLE] = "WM_ENTERIDLE";
    //msg_command_map[WM_MENURBUTTONUP] = "WM_MENURBUTTONUP";
    //msg_command_map[WM_MENUDRAG] = "WM_MENUDRAG";
    //msg_command_map[WM_MENUGETOBJECT] = "WM_MENUGETOBJECT";
    //msg_command_map[WM_UNINITMENUPOPUP] = "WM_UNINITMENUPOPUP";
    //msg_command_map[WM_MENUCOMMAND] = "WM_MENUCOMMAND";
    //msg_command_map[WM_CHANGEUISTATE] = "WM_CHANGEUISTATE";
    //msg_command_map[WM_UPDATEUISTATE] = "WM_UPDATEUISTATE";
    //msg_command_map[WM_QUERYUISTATE] = "WM_QUERYUISTATE";
    //msg_command_map[WM_CTLCOLORMSGBOX] = "WM_CTLCOLORMSGBOX";
    //msg_command_map[WM_CTLCOLOREDIT] = "WM_CTLCOLOREDIT";
    //msg_command_map[WM_CTLCOLORLISTBOX] = "WM_CTLCOLORLISTBOX";
    //msg_command_map[WM_CTLCOLORBTN] = "WM_CTLCOLORBTN";
    //msg_command_map[WM_CTLCOLORDLG] = "WM_CTLCOLORDLG";
    //msg_command_map[WM_CTLCOLORSCROLLBAR] = "WM_CTLCOLORSCROLLBAR";
    //msg_command_map[WM_CTLCOLORSTATIC] = "WM_CTLCOLORSTATIC";
    //msg_command_map[WM_MOUSEFIRST] = "WM_MOUSEFIRST";
    //msg_command_map[WM_MOUSEMOVE] = "WM_MOUSEMOVE";
    //msg_command_map[WM_LBUTTONDOWN] = "WM_LBUTTONDOWN";
    //msg_command_map[WM_LBUTTONUP] = "WM_LBUTTONUP";
    //msg_command_map[WM_LBUTTONDBLCLK] = "WM_LBUTTONDBLCLK";
    //msg_command_map[WM_RBUTTONDOWN] = "WM_RBUTTONDOWN";
    //msg_command_map[WM_RBUTTONUP] = "WM_RBUTTONUP";
    //msg_command_map[WM_RBUTTONDBLCLK] = "WM_RBUTTONDBLCLK";
    //msg_command_map[WM_MBUTTONDOWN] = "WM_MBUTTONDOWN";
    //msg_command_map[WM_MBUTTONUP] = "WM_MBUTTONUP";
    //msg_command_map[WM_MBUTTONDBLCLK] = "WM_MBUTTONDBLCLK";
    //msg_command_map[WM_MOUSEWHEEL] = "WM_MOUSEWHEEL";
    //msg_command_map[WM_XBUTTONDOWN] = "WM_XBUTTONDOWN";
    //msg_command_map[WM_XBUTTONUP] = "WM_XBUTTONUP";
    //msg_command_map[WM_XBUTTONDBLCLK] = "WM_XBUTTONDBLCLK";
    //msg_command_map[WM_MOUSEHWHEEL] = "WM_MOUSEHWHEEL";
    //msg_command_map[WM_MOUSELAST] = "WM_MOUSELAST";
    //msg_command_map[WM_MOUSELAST] = "WM_MOUSELAST";
    //msg_command_map[WM_MOUSELAST] = "WM_MOUSELAST";
    //msg_command_map[WM_MOUSELAST] = "WM_MOUSELAST";
    //msg_command_map[WM_PARENTNOTIFY] = "WM_PARENTNOTIFY";
    //msg_command_map[WM_ENTERMENULOOP] = "WM_ENTERMENULOOP";
    //msg_command_map[WM_EXITMENULOOP] = "WM_EXITMENULOOP";
    //msg_command_map[WM_NEXTMENU] = "WM_NEXTMENU";
    //msg_command_map[WM_SIZING] = "WM_SIZING";
    //msg_command_map[WM_CAPTURECHANGED] = "WM_CAPTURECHANGED";
    //msg_command_map[WM_MOVING] = "WM_MOVING";
    //msg_command_map[WM_POWERBROADCAST] = "WM_POWERBROADCAST";
    //msg_command_map[WM_DEVICECHANGE] = "WM_DEVICECHANGE";
    //msg_command_map[WM_MDICREATE] = "WM_MDICREATE";
    //msg_command_map[WM_MDIDESTROY] = "WM_MDIDESTROY";
    //msg_command_map[WM_MDIACTIVATE] = "WM_MDIACTIVATE";
    //msg_command_map[WM_MDIRESTORE] = "WM_MDIRESTORE";
    //msg_command_map[WM_MDINEXT] = "WM_MDINEXT";
    //msg_command_map[WM_MDIMAXIMIZE] = "WM_MDIMAXIMIZE";
    //msg_command_map[WM_MDITILE] = "WM_MDITILE";
    //msg_command_map[WM_MDICASCADE] = "WM_MDICASCADE";
    //msg_command_map[WM_MDIICONARRANGE] = "WM_MDIICONARRANGE";
    //msg_command_map[WM_MDIGETACTIVE] = "WM_MDIGETACTIVE";
    //msg_command_map[WM_MDISETMENU] = "WM_MDISETMENU";
    //msg_command_map[WM_ENTERSIZEMOVE] = "WM_ENTERSIZEMOVE";
    //msg_command_map[WM_EXITSIZEMOVE] = "WM_EXITSIZEMOVE";
    //msg_command_map[WM_DROPFILES] = "WM_DROPFILES";
    //msg_command_map[WM_MDIREFRESHMENU] = "WM_MDIREFRESHMENU";
    //msg_command_map[WM_IME_SETCONTEXT] = "WM_IME_SETCONTEXT";
    //msg_command_map[WM_IME_NOTIFY] = "WM_IME_NOTIFY";
    //msg_command_map[WM_IME_CONTROL] = "WM_IME_CONTROL";
    //msg_command_map[WM_IME_COMPOSITIONFULL] = "WM_IME_COMPOSITIONFULL";
    //msg_command_map[WM_IME_SELECT] = "WM_IME_SELECT";
    //msg_command_map[WM_IME_CHAR] = "WM_IME_CHAR";
    //msg_command_map[WM_IME_REQUEST] = "WM_IME_REQUEST";
    //msg_command_map[WM_IME_KEYDOWN] = "WM_IME_KEYDOWN";
    //msg_command_map[WM_IME_KEYUP] = "WM_IME_KEYUP";
    //msg_command_map[WM_MOUSEHOVER] = "WM_MOUSEHOVER";
    //msg_command_map[WM_MOUSELEAVE] = "WM_MOUSELEAVE";
    //msg_command_map[WM_NCMOUSEHOVER] = "WM_NCMOUSEHOVER";
    //msg_command_map[WM_NCMOUSELEAVE] = "WM_NCMOUSELEAVE";
    //msg_command_map[WM_WTSSESSION_CHANGE] = "WM_WTSSESSION_CHANGE";
    //msg_command_map[WM_TABLET_FIRST] = "WM_TABLET_FIRST";
    //msg_command_map[WM_TABLET_LAST] = "WM_TABLET_LAST";
    //msg_command_map[WM_CUT] = "WM_CUT";
    //msg_command_map[WM_COPY] = "WM_COPY";
    //msg_command_map[WM_PASTE] = "WM_PASTE";
    //msg_command_map[WM_CLEAR] = "WM_CLEAR";
    //msg_command_map[WM_UNDO] = "WM_UNDO";
    //msg_command_map[WM_RENDERFORMAT] = "WM_RENDERFORMAT";
    //msg_command_map[WM_RENDERALLFORMATS] = "WM_RENDERALLFORMATS";
    //msg_command_map[WM_DESTROYCLIPBOARD] = "WM_DESTROYCLIPBOARD";
    //msg_command_map[WM_DRAWCLIPBOARD] = "WM_DRAWCLIPBOARD";
    //msg_command_map[WM_PAINTCLIPBOARD] = "WM_PAINTCLIPBOARD";
    //msg_command_map[WM_VSCROLLCLIPBOARD] = "WM_VSCROLLCLIPBOARD";
    //msg_command_map[WM_SIZECLIPBOARD] = "WM_SIZECLIPBOARD";
    //msg_command_map[WM_ASKCBFORMATNAME] = "WM_ASKCBFORMATNAME";
    //msg_command_map[WM_CHANGECBCHAIN] = "WM_CHANGECBCHAIN";
    //msg_command_map[WM_HSCROLLCLIPBOARD] = "WM_HSCROLLCLIPBOARD";
    //msg_command_map[WM_QUERYNEWPALETTE] = "WM_QUERYNEWPALETTE";
    //msg_command_map[WM_PALETTEISCHANGING] = "WM_PALETTEISCHANGING";
    //msg_command_map[WM_PALETTECHANGED] = "WM_PALETTECHANGED";
    //msg_command_map[WM_HOTKEY] = "WM_HOTKEY";
    //msg_command_map[WM_PRINT] = "WM_PRINT";
    //msg_command_map[WM_PRINTCLIENT] = "WM_PRINTCLIENT";
    //msg_command_map[WM_APPCOMMAND] = "WM_APPCOMMAND";
    //msg_command_map[WM_THEMECHANGED] = "WM_THEMECHANGED";
    //msg_command_map[WM_CLIPBOARDUPDATE] = "WM_CLIPBOARDUPDATE";
    //msg_command_map[WM_DWMCOMPOSITIONCHANGED] = "WM_DWMCOMPOSITIONCHANGED";
    //msg_command_map[WM_DWMNCRENDERINGCHANGED] = "WM_DWMNCRENDERINGCHANGED";
    //msg_command_map[WM_DWMCOLORIZATIONCOLORCHANGED] = "WM_DWMCOLORIZATIONCOLORCHANGED";
    //msg_command_map[WM_DWMWINDOWMAXIMIZEDCHANGE] = "WM_DWMWINDOWMAXIMIZEDCHANGE";
    //msg_command_map[WM_GETTITLEBARINFOEX] = "WM_GETTITLEBARINFOEX";
    //msg_command_map[WM_HANDHELDFIRST] = "WM_HANDHELDFIRST";
    //msg_command_map[WM_HANDHELDLAST] = "WM_HANDHELDLAST";
    //msg_command_map[WM_AFXFIRST] = "WM_AFXFIRST";
    //msg_command_map[WM_AFXLAST] = "WM_AFXLAST";
    //msg_command_map[WM_PENWINFIRST] = "WM_PENWINFIRST";
    //msg_command_map[WM_PENWINLAST] = "WM_PENWINLAST";
    //msg_command_map[WM_APP] = "WM_APP";
    //msg_command_map[WM_USER] = "WM_USER";

    shell_command_map[HSHELL_WINDOWCREATED] = "Created";
    shell_command_map[HSHELL_WINDOWDESTROYED] = "Destroyed";
    shell_command_map[HSHELL_ACTIVATESHELLWINDOW] = "ActivateShellWindow";
#if defined(HSHELL_WINDOWACTIVATED)
    shell_command_map[HSHELL_WINDOWACTIVATED] = "Activated";
    shell_command_map[HSHELL_GETMINRECT] = "GetMinRect";
    shell_command_map[HSHELL_REDRAW] = "Redraw";
    shell_command_map[HSHELL_TASKMAN] = "TaskMan";
    shell_command_map[HSHELL_LANGUAGE] = "Language";
    shell_command_map[HSHELL_SYSMENU] = "SysMenu";
    shell_command_map[HSHELL_ENDTASK] = "EndTask";
#endif
#if defined(HSHELL_ACCESSIBILITYSTATE)
    shell_command_map[HSHELL_ACCESSIBILITYSTATE] = "AccessibilityState";
    shell_command_map[HSHELL_APPCOMMAND] = "AppCommand";
#endif
#if defined(HSHELL_WINDOWREPLACED)
    shell_command_map[HSHELL_WINDOWREPLACED] = "Replaced";
    shell_command_map[HSHELL_WINDOWREPLACING] = "Replacing";
#endif
    shell_command_map[HSHELL_FLASH] = "Flash";
    shell_command_map[HSHELL_RUDEAPPACTIVATED] = "RudeAppActivated";
}

void DBprintf(const char *fmt, ...)
{
    static int winMode = 0;
    static char  buf[2048];
    static WCHAR utf16_buffer[1024];

    va_list marker;
    va_start(marker, fmt);
    vsprintf_s(buf, sizeof(buf), fmt, marker);
    va_end(marker);

    printf("%s", buf);
    fflush(stdout);

    if(!winMode)
        winMode = (((GetVersion() & 0x80000000 ) != 0) ? 2 : 1);

    if(winMode != 2)
    {
#ifdef UNICODE
        // convert UTF-8 to UTF-16
        MultiByteToWideChar(CP_UTF8, 0, buf, -1, utf16_buffer, sizeof(utf16_buffer)/2);
        OutputDebugString(utf16_buffer);
#else
        OutputDebugString(buf);
#endif
    }

    SetLastError(0);
}

LPTSTR GetLastErrorText( LPTSTR lpszBuf, DWORD dwSize )
{
    DWORD dwRet;
    LPTSTR lpszTemp = NULL;

    dwRet = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |FORMAT_MESSAGE_ARGUMENT_ARRAY,
                           NULL,
                           GetLastError(),
                           LANG_NEUTRAL,
                           (LPTSTR)&lpszTemp,
                           0,
                           NULL );

    // supplied buffer is not long enough
    if ( !dwRet || ( (long)dwSize < (long)dwRet+14 ) )
        lpszBuf[0] = TEXT('\0');
    else
    {
        lpszTemp[lstrlen(lpszTemp)-2] = TEXT('\0');  // remove cr and newline character
        swprintf_s( lpszBuf, dwSize, TEXT("%s (0x%x)"), lpszTemp, GetLastError() );
    }

    if ( lpszTemp )
        LocalFree((HLOCAL) lpszTemp );

    return lpszBuf;
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    WinList* win_list = (WinList*)lParam;
    win_list->append(hwnd);
    return TRUE;
}

void MainWindow::make_active_window(WId win_id)
{
    // This is some strange stuff, but it works...
    // https://stackoverflow.com/questions/916259/win32-bring-a-window-to-top
    HWND _win_id = (HWND)win_id;
    HWND hCurWnd = ::GetForegroundWindow();
    DWORD dwMyID = ::GetCurrentThreadId();
    DWORD dwCurID = ::GetWindowThreadProcessId(hCurWnd, NULL);
    ::AttachThreadInput(dwCurID, dwMyID, TRUE);
    ::SetWindowPos(_win_id, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    ::SetWindowPos(_win_id, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    ::SetForegroundWindow(_win_id);
    ::AttachThreadInput(dwCurID, dwMyID, FALSE);
    ::SetFocus(_win_id);
    ::SetActiveWindow(_win_id);
}

bool MainWindow::locate_instance()
{
    TCHAR   buffer[MAX_PATH];
    WinList win_list;

    QRegExp title_regex("^WindowNotes .+Bob Hood$");

    EnumWindows(EnumWindowsProc, (LPARAM)&win_list);

    WinListConstIter iter;
    for(iter = win_list.begin();iter != win_list.end();iter++)
    {
        ::GetWindowText(*iter, buffer, MAX_PATH);
        QString title = QString::fromWCharArray(buffer);
        if((title_regex.indexIn(title) != -1) && (*iter != (HWND)winId()))
            return true;
    }

    return false;
}

void MainWindow::play_sound(int sound)
{
    if(enable_sound_effects &&
       sound >= SOUND_DELETE &&
       sound < SOUND_MAX &&
       sound_cache[sound].size())
        ::PlaySound((LPCWSTR)sound_cache[sound].constData(), NULL, SND_MEMORY | SND_ASYNC);
}

void MainWindow::set_startup()
{
    HKEY hkey;

    QString key_str = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    QString label_str = "WindowNotes";

    if(start_automatically)
    {
        QString buffer_str;

        wchar_t path[MAX_PATH];
        GetModuleFileName(NULL,path,MAX_PATH);
        QString path_str = QString::fromWCharArray(path);

        if(!path_str.startsWith('"'))
            path_str = QString("\"%1\"").arg(path_str);

        if(RegOpenKeyEx(HKEY_CURRENT_USER, AS_LPCWSTR(key_str), 0, KEY_SET_VALUE, &hkey) == ERROR_SUCCESS)
        {
            int len = path_str.size();
#ifdef UNICODE
            len *= 2;
#endif
            RegSetValueEx(hkey,AS_LPCWSTR(label_str), 0, REG_SZ, AS_LPBYTE(path_str), len);
            RegCloseKey(hkey);
        }
    }
    else
    {
        if(RegOpenKeyEx(HKEY_CURRENT_USER, AS_LPCWSTR(key_str), 0, KEY_SET_VALUE, &hkey) == ERROR_SUCCESS)
        {
            RegDeleteValue(hkey, AS_LPCWSTR(label_str));
            RegCloseKey(hkey);
        }
    }
}

LPTSTR get_window_text(HWND win_id, int& text_length)
{
    LPTSTR  lptstr = (LPTSTR)0;

    text_length = GetWindowTextLength(win_id);
    if(text_length)
    {
        text_length += sizeof(TCHAR) * 3;
        lptstr = new TCHAR[text_length];
        GetWindowText(win_id, lptstr, text_length);
    }

    return lptstr;
}

void MainWindow::set_hooks()
{
    QString msg("SHELLHOOK");
    WM_SHELLHOOKMESSAGE = RegisterWindowMessage(AS_LPCWSTR(msg));

    HWND my_id = (HWND)winId();
    RegisterShellHookWindow(my_id);
    hook_global_win_event();

    HWND active_id = GetForegroundWindow();
    if(active_id && active_id != my_id)
    {
        int length;
        LPTSTR lptstr = get_window_text(active_id, length);
        if(lptstr)
        {
            focus_window_handle = active_id;
            focus_window_title = QString::fromWCharArray(lptstr);
            current_context = locate_context();
            if(current_context)
            {
                arrange_notetabs();
                display_notetabs();
            }
        }
    }
}

bool MainWindow::process_win_msg(MSG* msg, long* result)
{
    HWND    win_id;
    int     length;
    LPTSTR  lptstr = (LPTSTR)0;
    bool    action = false;

    if(in_context_menu || notetab_clicked)
        return action;

    if(result)
        *result = 0L;

    if(msg->message == WM_SHELLHOOKMESSAGE)
    {
        win_id = (HWND)msg->lParam;
        lptstr = get_window_text(win_id, length);
        QString win_text;
        if(lptstr)
            win_text = QString::fromWCharArray(lptstr);

#ifndef NDEBUG
        QRegExp re(".*Camtasia.*");
        if(re.indexIn(win_text) != -1)
        {
            DBprintf("Ignoring window \"%s\"\n", win_text.toLatin1().constData());
            delete [] lptstr;
            return action;
        }
#endif

        if(shell_command_map.contains(msg->wParam))
        {
            if(lptstr)
                add_log_entry(shell_command_map[msg->wParam], win_text);
        }

        switch(msg->wParam)
        {
            case HSHELL_REDRAW:
                if(focus_window_handle != win_id)
                    break;

            case HSHELL_WINDOWACTIVATED:
            case HSHELL_RUDEAPPACTIVATED:
                focus_window_handle == win_id ? hide_notetabs() : delete_notetabs();

                focus_window_handle = win_id;

                if(win_id != (HWND)winId() && lptstr)
                {
                    focus_window_title = win_text;
                    current_context = locate_context();
                    if(current_context && IsWindowVisible(win_id))
                    {
                        arrange_notetabs();
                        display_notetabs();
                    }
                }

                action = true;
                break;

            case HSHELL_WINDOWCREATED:
                action = true;
                break;

            case HSHELL_WINDOWDESTROYED:
                if(focus_window_handle == win_id)
                {
                    delete_notetabs();
                    current_context = (Context*)0;
                    focus_window_handle = (HWND)0;
                    focus_window_title.clear();
                }
                action = true;
                break;

            default:
                break;
        }
    }

    if(lptstr)
        delete [] lptstr;

    return action;
}

void MainWindow::process_win_event(DWORD event, HWND win_id)
{
    if(in_context_menu || notetab_clicked)
        return;

    int length;
    LPTSTR lptstr = get_window_text(win_id, length);

    if(!lptstr)
        return;

    QString win_text = QString::fromWCharArray(lptstr);

#ifndef NDEBUG
    QRegExp re(".*Camtasia.*");
    if(re.indexIn(win_text) != -1)
    {
        DBprintf("Ignoring window \"%s\"\n", win_text.toLatin1().constData());
        delete [] lptstr;
        return;
    }
#endif

    if(event == EVENT_SYSTEM_MOVESIZESTART)
    {
        focus_window_handle == win_id ? hide_notetabs() : delete_notetabs();

        if(win_id != focus_window_handle)
        {
            focus_window_handle = win_id;
            focus_window_title.clear();
        }
        add_log_entry("MoveSizeStart", win_text);
    }
    else if(event == EVENT_SYSTEM_MOVESIZEEND)
    {
        if(win_id != focus_window_handle)
        {
            focus_window_handle = win_id;
            focus_window_title = win_text;
        }

        // get the focus window's position and size
        GetWindowRect(focus_window_handle, (LPRECT)&focus_window_rect);
        QString win_rect = QString("<%1, %2, %3, %4>")
                            .arg(focus_window_rect.left)
                            .arg(focus_window_rect.top)
                            .arg(focus_window_rect.right)
                            .arg(focus_window_rect.bottom);
        add_log_entry("MoveSizeEnd", win_text, win_rect);

        current_context = locate_context();
        if(current_context)
        {
            arrange_notetabs();
            display_notetabs();
        }
    }
    else if(event == EVENT_SYSTEM_MINIMIZESTART)
    {
        delete_notetabs();
        current_context = (Context*)0;
        focus_window_handle = (HWND)0;
        focus_window_title.clear();
        add_log_entry("MinimizeStart", win_text);
    }
    else if(event == EVENT_SYSTEM_MINIMIZEEND)
        add_log_entry("MinimizeEnd", win_text);
    else
        add_log_entry(QString("0x%1").arg(event, 4, 16, QChar('0')), win_text);

    delete [] lptstr;
}

void CALLBACK WinEventProc(HWINEVENTHOOK /*hook*/, DWORD event, HWND hwnd,
                           LONG /*idObject*/, LONG /*idChild*/,
                           DWORD /*dwEventThread*/, DWORD /*dwmsEventTime*/)
{
    main_window->process_win_event(event, hwnd);
}

void MainWindow::hook_global_win_event()
{
    win_movesize_hook_handle= SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                                     NULL,
                                     WinEventProc, 0, 0,
                                     WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    win_minimize_hook_handle= SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND,
                                     NULL,
                                     WinEventProc, 0, 0,
                                     WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
}

void MainWindow::unhook_global_win_event()
{
    if(!win_movesize_hook_handle)
        return;
    UnhookWinEvent(win_movesize_hook_handle);
    win_movesize_hook_handle = (HWINEVENTHOOK)0;
}

static bool lwkey_down = false;

LRESULT CALLBACK LowLevelKeyboardProc( int nCode, WPARAM wParam, LPARAM lParam )
{
    if(nCode < 0)
        return CallNextHookEx( NULL, nCode, wParam, lParam );

    KBDLLHOOKSTRUCT *pKeyboard = (KBDLLHOOKSTRUCT *)lParam;

    switch(wParam)
    {
        case WM_KEYDOWN:
            if(pKeyboard->vkCode == VK_LWIN || pKeyboard->vkCode == VK_RWIN)
                lwkey_down = true;
            else if(main_window->get_add_key().unicode() == pKeyboard->vkCode && lwkey_down)
            {
                lwkey_down = false;
                main_window->process_add();
                return 1;
            }

            break;

        case WM_KEYUP: // When the key has been pressed and released
            if(pKeyboard->vkCode == VK_LWIN || pKeyboard->vkCode == VK_RWIN)
                lwkey_down = false;

            break;
    }

    return CallNextHookEx( NULL, nCode, wParam, lParam );
}

void MainWindow::hook_global_keyboard_event()
{
    if(keyboard_hook_handle)
        return;

    HINSTANCE appInstance = GetModuleHandle(NULL); // Set a global Windows Hook to capture keystrokes.
    keyboard_hook_handle = SetWindowsHookExW( WH_KEYBOARD_LL, LowLevelKeyboardProc, appInstance, 0 );
}

void MainWindow::unhook_global_keyboard_event()
{
    if(!keyboard_hook_handle)
        return;

    UnhookWindowsHookEx(keyboard_hook_handle);
    keyboard_hook_handle = (HHOOK)0;
}
