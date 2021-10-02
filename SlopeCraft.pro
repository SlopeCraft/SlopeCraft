QT       += core gui concurrent

INCLUDEPATH += D:\eigen-3.4.0 \
                                ./Headers \
                                ./Sources \
                                ./others \
                                D:\zlibs \
                                ./Kernel \
                                ./BlockListManager

DEFINES += WIN32_LEAN_AND_MEAN
RC_ICONS = others/SlopeCraft.ico
# 版本号
VERSION = 3.6.0.0
# 语言
# 0x0004 表示 简体中文
RC_LANG = 0x0004
# 公司名

#QMAKE_TARGET_COMPANY =  TokiNoBug
# 产品名称
#QMAKE_TARGET_PRODUCT = SlopeCraft.exe
# 详细描述
#QMAKE_TARGET_DESCRIPTION = SlopeCraft is a generator for 3 dimensional pixel painting in minecraft.
# 版权
#QMAKE_TARGET_COPYRIGHT = CopyRight TokiNoBug


#DEFINES += QT_NO_WARNING_OUTPUT\
#                QT_NO_DEBUG_OUTPUT
#仅在发布时取消注释

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../zlibs/adler32.c \
    ../../zlibs/compress.c \
    ../../zlibs/crc32.c \
    ../../zlibs/deflate.c \
    ../../zlibs/gzclose.c \
    ../../zlibs/gzlib.c \
    ../../zlibs/gzread.c \
    ../../zlibs/gzwrite.c \
    ../../zlibs/infback.c \
    ../../zlibs/inffast.c \
    ../../zlibs/inflate.c \
    ../../zlibs/inftrees.c \
    ../../zlibs/trees.c \
    ../../zlibs/uncompr.c \
    ../../zlibs/zutil.c \
    BlockListManager/BlockListManager.cpp \
    BlockListManager/TokiBaseColor.cpp \
    BlockListManager/TokiBlock.cpp \
    Kernel/HeightLine.cpp \
    Kernel/PrimGlassBuilder.cpp \
    Kernel/TokiSlopeCraft.cpp \
    Kernel/CIEDE00.cpp \
    Kernel/NBTWriter.cpp \
    Kernel/OptiChain.cpp \
    Kernel/WaterItem.cpp \
    Kernel/colorset.cpp \
    Kernel/lossyCompressor.cpp \
    Sources/main.cpp \
    Sources/mainwindow.cpp \
    Sources/previewwind.cpp \
    Kernel/simpleBlock.cpp \
    Kernel/tokicolor.cpp \
    Sources/tpstrategywind.cpp \

HEADERS += \
    ../../zlibs/crc32.h \
    ../../zlibs/deflate.h \
    ../../zlibs/gzguts.h \
    ../../zlibs/inffast.h \
    ../../zlibs/inffixed.h \
    ../../zlibs/inflate.h \
    ../../zlibs/inftrees.h \
    ../../zlibs/trees.h \
    ../../zlibs/zconf.h \
    ../../zlibs/zlib.h \
    ../../zlibs/zutil.h \
    BlockListManager/BlockListManager.h \
    BlockListManager/TokiBaseColor.h \
    BlockListManager/TokiBlock.h \
    Kernel/ColorSet.h \
    Kernel/HeightLine.h \
    Kernel/NBTWriter.h \
    Kernel/OptiChain.h \
    Headers/TokiBaseColor.h \
    Kernel/PrimGlassBuilder.h \
    Kernel/TokiColor.h \
    Kernel/TokiSlopeCraft.h \
    Kernel/WaterItem.h \
    Headers/mainwindow.h \
    Headers/previewwind.h \
    Kernel/lossyCompressor.h \
    Kernel/simpleBlock.h \
    Headers/tpstrategywind.h \
    Kernel/defines.h

FORMS += \
    others/mainwindow.ui \
 \    #previewer.ui
    others/previewwind.ui \
    others/tpstrategywind.ui

TRANSLATIONS += \
    others/Slope_zh_CN.ts \
    others/Slope_en_US.ts

CODECFORTR = utf-8



CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    others/BlockTextures.qrc \
    others/Languages.qrc \
    others/Pics.qrc

QT += widgets
