#include "MapViewerWind.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "processMapFiles.h"
#include <stdio.h>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MapViewer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MapViewerWind w;
    w.show();
    return a.exec();
}
