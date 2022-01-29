#Use this line when in non-qt mode
CONFIG -= qt

#Use this line when in with-qt mode
#CONFIG += qt core gui concurrent

INCLUDEPATH += D:/CppLibs/zlib-1.2.11 \
                               D:/CppLibs/eigen-3.4.0 \
                               ../AiCvterInterface

RC_LANG = 0x0004

VERSION += 3.6.1.0

QMAKE_TARGET_DESCRIPTION = SlopeCraft Library (without AiConverter)
#描述信息

QMAKE_TARGET_COPYRIGHT = TokiNoBug
#版权信息

QMAKE_TARGET_PRODUCT = SlopeCraftL
#产品名称

QMAKE_TARGET_INTERNALNAME = SlopeCraftL

TEMPLATE = lib
DEFINES += SLOPECRAFTL_LIBRARY

CONFIG += c++17

#Comment following lines if you compile without AiConverter
#LIBS += D:\Git\build-SlopeCraft-Desktop_Qt_6_2_1_MinGW_64_bit-Release\SlopeCraftL\release\AiCvterLib.dll
#DEFINES += SLOPECRAFTL_WITH_AICVETR

SOURCES += \
    ../../../CppLibs/zlib-1.2.11/adler32.c \
    ../../../CppLibs/zlib-1.2.11/compress.c \
    ../../../CppLibs/zlib-1.2.11/crc32.c \
    ../../../CppLibs/zlib-1.2.11/deflate.c \
    ../../../CppLibs/zlib-1.2.11/gzclose.c \
    ../../../CppLibs/zlib-1.2.11/gzlib.c \
    ../../../CppLibs/zlib-1.2.11/gzread.c \
    ../../../CppLibs/zlib-1.2.11/gzwrite.c \
    ../../../CppLibs/zlib-1.2.11/infback.c \
    ../../../CppLibs/zlib-1.2.11/inffast.c \
    ../../../CppLibs/zlib-1.2.11/inflate.c \
    ../../../CppLibs/zlib-1.2.11/inftrees.c \
    ../../../CppLibs/zlib-1.2.11/trees.c \
    ../../../CppLibs/zlib-1.2.11/uncompr.c \
    ../../../CppLibs/zlib-1.2.11/zutil.c \
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
    ../../../CppLibs/zlib-1.2.11/crc32.h \
    ../../../CppLibs/zlib-1.2.11/deflate.h \
    ../../../CppLibs/zlib-1.2.11/gzguts.h \
    ../../../CppLibs/zlib-1.2.11/inffast.h \
    ../../../CppLibs/zlib-1.2.11/inffixed.h \
    ../../../CppLibs/zlib-1.2.11/inflate.h \
    ../../../CppLibs/zlib-1.2.11/inftrees.h \
    ../../../CppLibs/zlib-1.2.11/trees.h \
    ../../../CppLibs/zlib-1.2.11/zconf.h \
    ../../../CppLibs/zlib-1.2.11/zlib.h \
    ../../../CppLibs/zlib-1.2.11/zutil.h \
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
