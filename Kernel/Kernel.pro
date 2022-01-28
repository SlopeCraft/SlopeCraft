INCLUDEPATH += D:/CppLibs/eigen-3.4.0 \
                               D:/CppLibs/zlibs \
                               ../SlopeCraftL \
                               ../AiCvterInterface

HEADERS += \
    ../../../CppLibs/zlibs/crc32.h \
    ../../../CppLibs/zlibs/deflate.h \
    ../../../CppLibs/zlibs/gzguts.h \
    ../../../CppLibs/zlibs/inffast.h \
    ../../../CppLibs/zlibs/inffixed.h \
    ../../../CppLibs/zlibs/inflate.h \
    ../../../CppLibs/zlibs/inftrees.h \
    ../../../CppLibs/zlibs/trees.h \
    ../../../CppLibs/zlibs/zconf.h \
    ../../../CppLibs/zlibs/zlib.h \
    ../../../CppLibs/zlibs/zutil.h \
    ../AiCvterInterface/AiConverterInterface.h \    #Comment this line if you compile without AiConverter
    ../AiCvterInterface/AiCvterLib_global.h \    #Comment this line if you compile without AiConverter
    ../SlopeCraftL/SlopeCraftL.h \
    ../SlopeCraftL/SlopeCraftL_global.h \
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
    simpleBlock.h

SOURCES += \
    ../../../CppLibs/zlibs/adler32.c \
    ../../../CppLibs/zlibs/compress.c \
    ../../../CppLibs/zlibs/crc32.c \
    ../../../CppLibs/zlibs/deflate.c \
    ../../../CppLibs/zlibs/gzclose.c \
    ../../../CppLibs/zlibs/gzlib.c \
    ../../../CppLibs/zlibs/gzread.c \
    ../../../CppLibs/zlibs/gzwrite.c \
    ../../../CppLibs/zlibs/infback.c \
    ../../../CppLibs/zlibs/inffast.c \
    ../../../CppLibs/zlibs/inflate.c \
    ../../../CppLibs/zlibs/inftrees.c \
    ../../../CppLibs/zlibs/trees.c \
    ../../../CppLibs/zlibs/uncompr.c \
    ../../../CppLibs/zlibs/zutil.c \
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
