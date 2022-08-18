QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += D:/CppLibs/libEigen3/include/eigen3 \
                               D:/CppLibs/zlib/include

SOURCES += \
    main.cpp \
    MapViewerWind.cpp \
    ../SlopeCraftL/ColorSource.cpp \
    processMapFiles.cpp

HEADERS += \
    MapViewerWind.h \
    processMapFiles.h

FORMS += \
    MapViewerWind.ui

TRANSLATIONS += \
    MapViewer_en_US.ts

CONFIG += lrelease
CONFIG += embed_translations

LIBS += -fopenmp -lz

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
