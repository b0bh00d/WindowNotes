QT += widgets xml dbus
TARGET = WindowNotes
TEMPLATE = app


mac {
    DEFINES += QT_OSX
}

unix:!mac {
    RESOURCES += ./linux.qrc
    DEFINES += QT_LINUX
    # QT += multimedia
    #QT += x11extras
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
    LIBS += -lpthread -lm -ldl
    INCLUDEPATH += ../miniaudio
}

win32 {
    RESOURCES += ./WindowNotes.qrc
    DEFINES += QT_WIN
    LIBS += -lwinmm -ladvapi32
}

INTERMEDIATE_NAME = intermediate
MOC_DIR = $$INTERMEDIATE_NAME/moc
OBJECTS_DIR = $$INTERMEDIATE_NAME/obj
RCC_DIR = $$INTERMEDIATE_NAME/rcc
UI_DIR = $$INTERMEDIATE_NAME/ui

win32: RC_FILE = WindowNotes.rc

include(WindowNotes.pri)
