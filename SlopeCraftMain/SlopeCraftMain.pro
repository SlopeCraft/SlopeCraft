QT       += core gui concurrent network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


TARGET = SlopeCraft

RC_ICONS = others/SlopeCraft.ico

RC_LANG = 0x0004

VERSION += 3.6.0.0

#QMAKE_TARGET_COMPANY = TokiNoBug
#公司名称

QMAKE_TARGET_DESCRIPTION = Map Pixel Art Generator
#描述信息

QMAKE_TARGET_COPYRIGHT = TokiNoBug
#版权信息

QMAKE_TARGET_PRODUCT = SlopeCraft
#产品名称


INCLUDEPATH += D:\CppLibs\eigen-3.4.0 \
                               ../Kernel \
                               ../SlopeCraftL \
                               ../BlockListManager

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
# disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
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
    ../SlopeCraftL/SlopeCraftL.cpp \
    BatchUi.cpp \
    TaskBox.cpp \
    TokiTask.cpp \
    VersionDialog.cpp \
    main.cpp \
    mainwindow.cpp \
    previewwind.cpp \
    tpstrategywind.cpp

HEADERS += \
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
    ../Kernel/object_pool.hpp \
    ../Kernel/simpleBlock.h \
    ../SlopeCraftL/SlopeCraftL.h \
    ../SlopeCraftL/SlopeCraftL_global.h \
    BatchUi.h \
    TaskBox.h \
    TokiTask.h \
    VersionDialog.h \
    mainwindow.h \
    previewwind.h \
    tpstrategywind.h

FORMS += \
    others/BatchUi.ui \
    others/TaskBox.ui \
    others/VersionDialog.ui \
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

SUBDIRS += \
    ../Kernel/Kernel.pro
