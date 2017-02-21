#-------------------------------------------------
#
# Project created by QtCreator 2016-05-04T23:34:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = sample
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

OBJECTS_DIR = obj
DESTDIR = bin
LIBS += -L/usr/local/lib  -lprotobuf

SOURCES += main.cpp \
    proto/sample.pb.cc

HEADERS += \
    proto/sample.pb.h
