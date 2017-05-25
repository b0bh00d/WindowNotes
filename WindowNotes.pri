HEADERS += main.h \
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

win32: SOURCES += events_windows.cpp

FORMS += mainwindow.ui \
         noteedit.ui \
         exportdialog.ui \
