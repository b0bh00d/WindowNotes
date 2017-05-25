#pragma once

/// Various types and definitions used by the WindowNotes application

#include <QtCore/QtGlobal>
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

#define HTML_HEADER "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"><html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">p, li { white-space: pre-wrap; }</style></head><body style=\"font-family:'Courier'; font-size:11pt; font-weight:400; font-style:normal;\">"

#define ASSERT_UNUSED(cond) Q_ASSERT(cond); Q_UNUSED(cond)

#define DEFAULT_ADD_KEY   0x0041

//#ifdef UNICODE
//
//#define QStringToTCHAR(x) (wchar_t*) x.utf16()
//#define PQStringToTCHAR(x) (wchar_t*) x->utf16()
//#define TCHARToQString(x) QString::fromUtf16((x))
//#define TCHARToQStringN(x,y) QString::fromUtf16((x),(y))
//
//#else
//
//#define QStringToTCHAR(x) x.local8Bit().constData()
//#define PQStringToTCHAR(x) x->local8Bit().constData()
//#define TCHARToQString(x) QString::fromLocal8Bit((x))
//#define TCHARToQStringN(x,y) QString::fromLocal8Bit((x),(y))
//
//#endif

const int NOTE_MARGIN = 5;
const int NOTE_OFFSET = 5;
const int NOTE_DOCK_SIZE = 25;
const int NOTE_SHOW_SIZE = 50;

const int MAX_TAB_ICONS = 6;

void DBprintf(const char *fmt, ...);

struct NoteData
{
    enum
    {
        Mode_None,
        Mode_Add,
        Mode_Edit
    };

    int         mode;
    QString     context;
    QString     content;
    bool        regexp;
    int         context_icon;
    QStringList current_contexts;

    NoteData()
        : mode(Mode_None),
          regexp(false) {}
};

#ifndef NDEBUG
#define DEBUG
#endif
