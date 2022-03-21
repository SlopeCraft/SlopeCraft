#include "CutterWind.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    if(uiLanguages.contains("zh-CN"))
        goto makeWindow;


    for (const QString &locale : uiLanguages) {
        const QString baseName = "imageCutter_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

makeWindow:


    CutterWind w;

    if(argc>=2)
    {
        int ret
                =QMessageBox::warning(&w,"SlopeCraft compile-time ERROR",
                                      "Cannot find hand_x128.dll : Hardware not invented(0x01919810).\n\n\nSLOPECRAFT_FAILED_TO_HAVE_A_HANDSHAKE_WITH_YOU\nHAVE_A_NICE_DAY_DONT_REPORT_THIS_AS_AN_ERROR_ITS_JUST_A_JOKE",
                                      "ReportError","Exit","Ok",2);
        if(ret<=0) {
            QUrl url("https://github.com/ToKiNoBug/SlopeCraft/issues/new/choose");
            QDesktopServices::openUrl(url);
        }
        exit(0);
        return 0;
    }
    else{
        w.show();
    }


    return a.exec();
}
