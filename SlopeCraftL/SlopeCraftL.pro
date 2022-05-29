CONFIG -= qt


INCLUDEPATH += \
                               D:/CppLibs/eigen-3.4.0 \
                               ../AiCvterInterface \
                               D:\CppLibs\HeuristicFlow-1.5.0

RC_LANG = 0x0004

VERSION += 3.8.1.0

QMAKE_TARGET_DESCRIPTION = SlopeCraft Library# (with AiConverter)
#描述信息

QMAKE_TARGET_COPYRIGHT = TokiNoBug
#版权信息

QMAKE_TARGET_PRODUCT = SlopeCraftL
#产品名称

QMAKE_TARGET_INTERNALNAME = SlopeCraftL

QMAKE_CXXFLAGS += -fopenmp

LIBS += -fopenmp

TEMPLATE = lib
DEFINES += SLOPECRAFTL_LIBRARY

CONFIG += c++17

LIBS += D:\CppLibs\zlib\lib\libzlibstatic.a

#Comment following lines if you compile without AiConverter
#LIBS += D:\Git\build-SlopeCraft-Desktop_Qt_6_1_0_MinGW_64_bit-Release\SlopeCraftL\release\AiCvterLib3.dll
#DEFINES += SLOPECRAFTL_WITH_AICVETR

SOURCES += \
    AiCvterOpt.cpp \
    CIEDE00.cpp \
    ColorSource.cpp \
    HeightLine.cpp \
    NBTWriter.cpp \
    OptiChain.cpp \
    PrimGlassBuilder.cpp \
    SlopeCraftL_CAPI.cpp \
    TokiSlopeCraft.cpp \
    WaterItem.cpp \
    colorset.cpp \
    imagePreprocess.cpp \
    lossyCompressor.cpp \
    simpleBlock.cpp \
    tokicolor.cpp \
    SlopeCraftL.cpp

HEADERS += \
    AiCvterOpt.h \
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
#unix {
#    target.path = /usr/lib
#}
#!isEmpty(target.path): INSTALLS += target


