#Use this line when in non-qt mode
CONFIG -= qt

#Use this line when in with-qt mode
#CONFIG += qt core gui concurrent

INCLUDEPATH += D:/Git/SlopeCraft/Kernel \
                               D:/CppLibs/eigen-3.4.0
                               #D:/CppLibs/eigen-3.4.0

RC_LANG = 0x0004

VERSION += 3.6.0.0

QMAKE_TARGET_DESCRIPTION = SlopeCraft Library
#描述信息

QMAKE_TARGET_COPYRIGHT = TokiNoBug
#版权信息

QMAKE_TARGET_PRODUCT = SlopeCraftL
#产品名称

QMAKE_TARGET_INTERNALNAME = SlopeCraftL

TEMPLATE = lib
DEFINES += SLOPECRAFTL_LIBRARY

CONFIG += c++17



# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Kernel/CIEDE00.cpp \
    ../Kernel/HeightLine.cpp \
    ../Kernel/NBTWriter.cpp \
    ../Kernel/OptiChain.cpp \
    ../Kernel/PrimGlassBuilder.cpp \
    ../Kernel/TokiSlopeCraft.cpp \
    ../Kernel/WaterItem.cpp \
    ../Kernel/colorset.cpp \
    ../Kernel/lossyCompressor.cpp \
    ../Kernel/simpleBlock.cpp \
    ../Kernel/tokicolor.cpp \
    SlopeCraftL.cpp

HEADERS += \
    ../Kernel/ColorSet.h \
    ../Kernel/HeightLine.h \
    ../Kernel/NBTWriter.h \
    ../Kernel/OptiChain.h \
    ../Kernel/PrimGlassBuilder.h \
    ../Kernel/TokiColor.h \
    ../Kernel/TokiSlopeCraft.h \
    ../Kernel/WaterItem.h \
    ../Kernel/defines.h \
    ../Kernel/lossyCompressor.h \
    ../Kernel/object_pool.hpp \
    ../Kernel/simpleBlock.h \
    SlopeCraftL.h \
    SlopeCraftL_global.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
