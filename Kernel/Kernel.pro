#Use this line in WITH_QT mode
#QT       += core gui concurrent

#Use this line in when compiling without qt
QT -= core gui

INCLUDEPATH +=  D:/CppLibs/eigen-3.4.0

TEMPLATE = lib
DEFINES += KERNEL_LIBRARY

CONFIG += c++17
#CONFIG += staticlib

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CIEDE00.cpp \
    HeightLine.cpp \
    Kernel.cpp \
    NBTWriter.cpp \
    OptiChain.cpp \
    PrimGlassBuilder.cpp \
    TokiSlopeCraft.cpp \
    WaterItem.cpp \
    colorset.cpp \
    lossyCompressor.cpp \
    simpleBlock.cpp \
    tokicolor.cpp


HEADERS += \
    ColorSet.h \
    Kernel \
    Kernel.h \
    Kernel_global.h \
    TokiColor.h \
    TokiSlopeCraft.h \
    defines.h \
    simpleBlock.h \
    lossyCompressor.h \
    object_pool.hpp \
    PrimGlassBuilder.h \
    NBTWriter.h \
    HeightLine.h \
    WaterItem.h \
    OptiChain.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
