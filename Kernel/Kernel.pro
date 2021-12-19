INCLUDEPATH += D:/CppLibs/eigen-3.4.0 \
                               ../SlopeCraftL

HEADERS += \
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
