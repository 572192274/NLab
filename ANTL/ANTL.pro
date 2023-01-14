#-------------------------------------------------
#
# Project created by QtCreator 2022-02-14T01:04:39
#
#-------------------------------------------------

QT       += core gui
QT+=sql
QT+=network
RC_ICONS=logo.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ANTL
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        annotationwindow.cpp \
    login.cpp \
    DB/conn.cpp \
    antlwidget.cpp \
    selectitem.cpp

HEADERS += \
        annotationwindow.h \
    login.h \
    DB/conn.h \
    antlwidget.h \
    selectitem.h

FORMS += \
        annotationwindow.ui \
    login.ui \
    antlwidget.ui \
    selectitem.ui

RESOURCES += \
    rs.qrc
