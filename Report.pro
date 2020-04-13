#-------------------------------------------------
#
# Project created by QtCreator 2019-09-16T17:19:43
#
#-------------------------------------------------

QT       += core gui sql charts network xlsx

win32: RC_ICONS = $$PWD/images/main.ico
win32:{
    VERSION = 1.0.0
    QMAKE_TARGET_COMPANY = "Smena LTD."
    QMAKE_TARGET_PRODUCT = "CDR/EDR Report"
    QMAKE_TARGET_DESCRIPTION = "Working with CDR/EDR Database"
    QMAKE_TARGET_COPYRIGHT = "S.Semenkin"
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Report
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        analysedata.cpp \
        callstat.cpp \
        chartbyseparatethread.cpp \
        customslice.cpp \
        loadstatecheck.cpp \
        main.cpp \
        mainwindow.cpp \
        mylineedit.cpp \
        settings.cpp \
        textedit.cpp

HEADERS += \
        analysedata.h \
        callstat.h \
        chartbyseparatethread.h \
        customslice.h \
        loadstatecheck.h \
        mainwindow.h \
        mylineedit.h \
        settings.h \
        textedit.h

FORMS += \
        callstat.ui \
        mainwindow.ui \
        settings.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    rs.qrc


DISTFILES +=
