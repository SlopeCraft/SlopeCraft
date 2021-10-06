QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += BLOCKLISTMANAGER_LIBRARY

CONFIG += c++17

INCLUDEPATH += ../Kernel

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Kernel/simpleBlock.cpp \
    BlockListManager.cpp \
    TokiBaseColor.cpp \
    TokiBlock.cpp

HEADERS += \
    ../Kernel/simpleBlock.h \
    BlockListManager_global.h \
    BlockListManager.h \
    TokiBaseColor.h \
    TokiBlock.h

TRANSLATIONS += \
    BlockListManager_zh_CN.ts

# Default rules for deployment.
#unix {
#    target.path = /usr/lib
#}
#!isEmpty(target.path): INSTALLS += target

#win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Kernel/release/ -lKernel
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Kernel/debug/ -lKernel
#else:unix: LIBS += -L$$OUT_PWD/../Kernel/ -lKernel

#INCLUDEPATH += $$PWD/../Kernel
#DEPENDPATH += $$PWD/../Kernel
