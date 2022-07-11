CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

INCLUDEPATH += D:/CppLibs/eigen-3.4.0 \
                               D:/CppLibs/HeuristicFlow-1.6.1


QMAKE_CXXFLAGS += -fopenmp

LIBS += -fopenmp

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    GAConverter.cpp \
    sortColor.cpp

HEADERS += \
    GAConverter.h \
    GACvterDefines.hpp \
    sortColor.h \
    uiPack.h

