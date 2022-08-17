#include "MapViewerWind.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include "processMapFiles.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
/*
    std::vector<uint8_t> uncompressed_file;

    uncompress_map_file("D:\\map_3.dat",&uncompressed_file);

    cout<<"uncompressed_file.size() = "<<uncompressed_file.size()<<endl;

    const uint8_t *const map_content_ptr=find_color_begin(uncompressed_file);
    {
        FILE * file=::fopen("D:\\map_3.dat_generated","wb");
        if(file==NULL) {
            return 1;
        }

        ::fwrite(uncompressed_file.data(),1,uncompressed_file.size(),file);
        ::fclose(file);
    }
    if(map_content_ptr==nullptr) {
        cout<<"failed to find data"<<endl;
    }
    else {
        cout<<"Found data. The contents are : "<<endl;
        for(int idx=0;idx<128*128;idx++) {
            ::printf("%x ",uint32_t(map_content_ptr[idx]));
        }
        cout<<endl;
    }
    */

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
