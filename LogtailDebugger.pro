#-------------------------------------------------
#
# Project created by QtCreator 2017-12-16T23:38:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LogtailDebugger
TEMPLATE        = app
MOC_DIR         = temp/moc
RCC_DIR         = temp/rcc
UI_DIR          = temp/ui
OBJECTS_DIR     = temp/obj
win32:RC_FILE   = other/main.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH     += $$PWD
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    regexdebugger.cpp

HEADERS += \
        mainwindow.h \
    regexdebugger.h

FORMS += \
        mainwindow.ui \
    regexdebugger.ui
ICON = logtail.icns
RESOURCES += \
    other/qss.qrc \
    other/main.qrc