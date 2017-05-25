QT += widgets xml
TARGET = WindowNotes
TEMPLATE = app

RESOURCES += ./WindowNotes.qrc

mac {
    DEFINES += QT_OSX
}

unix:!mac {
    DEFINES += QT_LINUX
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

win32:RC_FILE = WindowNotes.rc

include(WindowNotes.pri)
