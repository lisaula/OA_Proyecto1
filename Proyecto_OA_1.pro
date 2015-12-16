#-------------------------------------------------
#
# Project created by QtCreator 2015-11-03T11:35:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Proyecto_OA_1
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    cp.cpp \
    disco.cpp \
    dialog.cpp \
    table.cpp \
    tree.cpp \
    node.cpp

HEADERS  += widget.h \
    cp.h \
    disco.h \
    dialog.h \
    table.h \
    tree.h \
    B_node.h \
    Btree.h \
    node.h

FORMS    += widget.ui \
    dialog.ui \
    table.ui \
    tree.ui
