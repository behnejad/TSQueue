TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x -pthread -Wall -O2

LIBS += -pthread

SOURCES += main.c \
    queue.c

HEADERS += \
    queue.h
