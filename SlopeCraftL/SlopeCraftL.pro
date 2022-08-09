CONFIG -= qt


INCLUDEPATH += \
                               D:/CppLibs/eigen-3.4.0 \
                               ../GAConverter \
                               D:/CppLibs/HeuristicFlow-1.6.2.1

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

DEFINES += SLOPECRAFTL_WITH_AICVETR

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
    SCLDefines.h \
    TokiColor.h \
    TokiSlopeCraft.h \
    WaterItem.h \
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



win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GAConverter/release/ -lGAConverter
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GAConverter/debug/ -lGAConverter
else:unix: LIBS += -L$$OUT_PWD/../GAConverter/ -lGAConverter

INCLUDEPATH += $$PWD/../GAConverter
DEPENDPATH += $$PWD/../GAConverter

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GAConverter/release/libGAConverter.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GAConverter/debug/libGAConverter.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GAConverter/release/GAConverter.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GAConverter/debug/GAConverter.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../GAConverter/libGAConverter.a
