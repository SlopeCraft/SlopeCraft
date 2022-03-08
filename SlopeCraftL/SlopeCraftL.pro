#Use this line when in non-qt mode
CONFIG -= qt

#Use this line when in with-qt mode
#CONFIG += qt core gui concurrent

INCLUDEPATH += \
                               D:/CppLibs/eigen-3.4.0 \
                               ../AiCvterInterface \
                               D:\CppLibs\HeuristicFlow-1.3.3

RC_LANG = 0x0004

VERSION += 3.6.2.0

QMAKE_TARGET_DESCRIPTION = SlopeCraft Library (without AiConverter)
#描述信息

QMAKE_TARGET_COPYRIGHT = TokiNoBug
#版权信息

QMAKE_TARGET_PRODUCT = SlopeCraftL
#产品名称

QMAKE_TARGET_INTERNALNAME = SlopeCraftL

QMAKE_CXXFLAGS += -fopenmp -std=c++2a

LIBS += -fopenmp

TEMPLATE = lib
DEFINES += SLOPECRAFTL_LIBRARY

CONFIG += std::c++20

LIBS += D:\Git\build-SlopeCraft-Desktop_Qt_6_1_0_MinGW_64_bit-Release\SlopeCraftL\release\libzlibstatic.a

#Comment following lines if you compile without AiConverter
#LIBS += D:\Git\build-SlopeCraft-Desktop_Qt_6_2_1_MinGW_64_bit-Release\SlopeCraftL\release\AiCvterLib.dll
#DEFINES += SLOPECRAFTL_WITH_AICVETR

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
    tokicolor.cpp \
    SlopeCraftL.cpp

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
    object_pool.hpp \
    simpleBlock.h \
    SlopeCraftL.h \
    SlopeCraftL_global.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
