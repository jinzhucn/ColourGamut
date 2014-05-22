#-------------------------------------------------
#
# Project created by QtCreator 2014-05-22T11:27:49
#
#-------------------------------------------------

QT       += core gui opengl

LIBS += -lGLU

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ColourGamut
TEMPLATE = app


SOURCES += \
           main.cpp\
           mainwindow.cpp \
           glcanvas.cpp \
    color.cpp

HEADERS  += color.h \
            mainwindow.h \
            glcanvas.h

