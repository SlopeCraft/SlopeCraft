QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11



RC_ICONS = others/SlopeCraft.ico

VERSION += 3.6.0.0

TARGET = SlopeCraft


INCLUDEPATH += D:\CppLibs\zlibs \
                               D:\CppLibs\eigen-3.4.0 \
                               ../Kernel \
                               ../BlockListManager

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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
    ../BlockListManager/BlockListManager.cpp \
    ../BlockListManager/TokiBaseColor.cpp \
    ../BlockListManager/TokiBlock.cpp \
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
    main.cpp \
    mainwindow.cpp \
    previewwind.cpp \
    tpstrategywind.cpp

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
    ../BlockListManager/BlockListManager.h \
    ../BlockListManager/TokiBaseColor.h \
    ../BlockListManager/TokiBlock.h \
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
    ../Kernel/simpleBlock.h \
    mainwindow.h \
    previewwind.h \
    tpstrategywind.h

FORMS += \
    others/mainwindow.ui \
    others/previewwind.ui \
    others/tpstrategywind.ui

TRANSLATIONS += \
    ./others/SlopeCraft_zh_CN.ts \
    ./others/SlopeCraft_en_US.ts

CONFIG += lrelease
CONFIG += embed_translations

RESOURCES += \
    others/BlockTextures.qrc \
    others/Pics.qrc

DISTFILES += \
    others/SlopeCraft.ico

# Default rules for deployment.

#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Kernel/release/ -lKernel
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Kernel/debug/ -lKernel
#else:unix: LIBS += -L$$OUT_PWD/../Kernel/ -lKernel

#INCLUDEPATH += $$PWD/../Kernel
#DEPENDPATH += $$PWD/../Kernel

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../BlockListManager/release/ -lBlockListManager
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../BlockListManager/debug/ -lBlockListManager
#else:unix: LIBS += -L$$OUT_PWD/../BlockListManager/ -lBlockListManager

#INCLUDEPATH += $$PWD/../BlockListManager
#DEPENDPATH += $$PWD/../BlockListManager


