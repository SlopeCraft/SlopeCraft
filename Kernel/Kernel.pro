QT       += core gui concurrent

INCLUDEPATH +=  D:\CppLibs\eigen-3.4.0

TEMPLATE = lib
DEFINES += KERNEL_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CIEDE00.cpp \
    HeightLine.cpp \
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
    HeightLine.h \
    NBTWriter.h \
    OptiChain.h \
    PrimGlassBuilder.h \
    TokiColor.h \
    TokiSlopeCraft.h \
    WaterItem.h \
    defines.h \
    lossyCompressor.h \
    simpleBlock.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
