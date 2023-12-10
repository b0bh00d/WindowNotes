HEADERS += main.h \
    $$PWD/linux/events.h \
           mainwindow.h \
           notetab.h \
           types.h \
           context.h \
           noteedit.h \
           soundlineedit.h \
           exportdialog.h \
           mimedata.h \

SOURCES += main.cpp \
           mainwindow.cpp \
           notetab.cpp \
           context.cpp \
           noteedit.cpp \
           soundlineedit.cpp \
           exportdialog.cpp \
           mimedata.cpp \

win32 {
    SOURCES += windows/events.cpp
}

unix:!mac {
    HEADERS += linux/Queue.h \
               linux/WindowEvents.h \
               linux/ActiveWindow.h \
               linux/singleton.h \
               linux/events.h \
               linux/events_types.h \
               linux/WindowEventsSingleton.h
    SOURCES += linux/events.cpp \
               linux/WindowEvents.cpp \
               linux/ActiveWindow.cpp \
               linux/WindowEventsSingleton.cpp
}

FORMS += mainwindow.ui \
         noteedit.ui \
         exportdialog.ui \
