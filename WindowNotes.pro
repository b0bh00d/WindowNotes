QT += widgets xml multimedia
TARGET = WindowNotes
TEMPLATE = app

RESOURCES += ./WindowNotes.qrc

mac {
    DEFINES += QT_OSX
}

unix:!mac {
    DEFINES += QT_LINUX
    #QT += x11extras
    QMAKE_CXXFLAGS_WARN_ON += -Wno-reorder
    #LIBS += -lX11
}

win32 {
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
