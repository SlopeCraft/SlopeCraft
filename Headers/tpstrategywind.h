#ifndef TPSTRATEGYWIND_H
#define TPSTRATEGYWIND_H

#include <QMainWindow>
#include"mainwindow.h"
namespace Ui {
class tpStrategyWind;
}
class MainWindow;

#ifndef TPS__
#define TPS__
class tpS{
public:
    tpS(char _pTpS='B',char _hTpS='C',QRgb _BGC=qRgb(220,220,220)){
            pTpS=_pTpS;
            hTpS=_hTpS;
            BGC=_BGC;    }
    ~tpS();
    char pTpS;
    char hTpS;
    QRgb BGC;
};
#endif

class tpStrategyWind : public QMainWindow
{
    Q_OBJECT

public:
    explicit tpStrategyWind(QWidget *parent = nullptr);
    ~tpStrategyWind();
    MainWindow*parent;
    char pTpS;
    char hTpS;
    QRgb BackGroundColor;
    void setVal(tpS);
protected:
    void closeEvent(QCloseEvent*event);
private:
    Ui::tpStrategyWind *ui;
signals:
    void Confirm(tpS);
public slots:
    void grabTpSInfo();
private slots:
    void on_Confirm_clicked();
    void on_isBGCCustom_clicked();
};



#endif // TPSTRATEGYWIND_H
