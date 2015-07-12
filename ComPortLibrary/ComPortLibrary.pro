#-------------------------------------------------
#
# Project created by QtCreator 2015-06-14T14:44:48
#
#-------------------------------------------------

QT       += core widgets
QT       += serialport

TARGET = ComPortLibrary
TEMPLATE = lib

DEFINES += COMPORTLIBRARY_LIBRARY
SOURCES += comportlibrary.cpp

HEADERS += comportlibrary.h\
        comportlibrary_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
