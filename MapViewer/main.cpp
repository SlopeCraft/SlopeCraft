#include "MapViewerWind.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "processMapFiles.h"

int main(int argc, char *argv[])
{

    std::vector<uint8_t> uncompressed_file;

    uncompress_map_file("D:\\map_3.dat",&uncompressed_file);

    cout<<"uncompressed_file.size() = "<<uncompressed_file.size()<<endl;

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
