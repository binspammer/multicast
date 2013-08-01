TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    receiver.cpp

QMAKE_CXXFLAGS += -std=c++11 -lpthread
LIBS += -lboost_system -lpthread
