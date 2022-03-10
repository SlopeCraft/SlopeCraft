#include "CutterWind.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

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
    w.show();
    return a.exec();
}
