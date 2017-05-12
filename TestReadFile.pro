#-------------------------------------------------
#
# Project created by QtCreator 2017-04-28T14:12:17
#
#-------------------------------------------------

QT       += core gui sql
l
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestReadFile
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


SOURCES += main.cpp\
        mainwindow.cpp \
    logfilereadertask.cpp \
    logfilereaderwidget.cpp \
    loglineparser.cpp \
    databasemanager.cpp \
    databasecreator.cpp \
    typeconversion.cpp \
    logdataview.cpp \
    painttextdelegate.cpp \
    relationsign.cpp \
    descdbtreeview.cpp \
    descdbtreeitem.cpp \
    treeitemdataview.cpp \
    colordelegate.cpp \
    relationshipdelegate.cpp \
    formattextdelegate.cpp \
    datamodel.cpp \
    filterdialog.cpp \
    descsortdatamodel.cpp \
    deschighlightdatamodel.cpp \
    logfiledialog.cpp

HEADERS  += mainwindow.h \
    logfilereadertask.h \
    logfilereaderwidget.h \
    loglineparser.h \
    databasemanager.h \
    databasecreator.h \
    typeconversion.h \
    filter.h \
    logdataview.h \
    painttextdelegate.h \
    relationsign.h \
    descdbtreeview.h \
    descdbtreeitem.h \
    treeitemdataview.h \
    colordelegate.h \
    relationshipdelegate.h \
    formattextdelegate.h \
    datamodel.h \
    filterdialog.h \
    descsortdatamodel.h \
    deschighlightdatamodel.h \
    logfiledialog.h

FORMS    += mainwindow.ui \
    logfilereaderwidget.ui \
    filterdialog.ui \
    logfiledialog.ui

RESOURCES += \
    resource.qrc
