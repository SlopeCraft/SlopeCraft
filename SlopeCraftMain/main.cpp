#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    qDebug()<<uiLanguages;
    /*
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SlopeCraft_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }*/
    MainWindow w;
    w.show();
    if(uiLanguages.contains("zh-CN")||uiLanguages.contains("zh"))
        w.turnCh();
    else
        w.turnEn();
    w.InitializeAll();

    return a.exec();
}
