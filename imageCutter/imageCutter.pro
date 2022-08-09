QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = imageCutter

RC_ICONS = ../SlopeCraftMain/others/imageCutterIconNew.ico

RC_LANG = 0x0004

VERSION += 3.9.0

#QMAKE_TARGET_COMPANY = TokiNoBug
#公司名称

QMAKE_TARGET_DESCRIPTION = SlopeCraft preprocesser
#描述信息

QMAKE_TARGET_COPYRIGHT = TokiNoBug
#版权信息

QMAKE_TARGET_PRODUCT = imageCutter
#产品名称

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    CutterWind.cpp

HEADERS += \
    CutterWind.h

FORMS += \
    CutterWind.ui

TRANSLATIONS += \
    imageCutter_en_US.ts

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
