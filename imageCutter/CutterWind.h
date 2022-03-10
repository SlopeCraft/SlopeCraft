#ifndef CUTTERWIND_H
#define CUTTERWIND_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class CutterWind; }
QT_END_NAMESPACE

class CutterWind : public QMainWindow
{
    Q_OBJECT

public:
    CutterWind(QWidget *parent = nullptr);
    ~CutterWind();

private:
    Ui::CutterWind *ui;
};
#endif // CUTTERWIND_H
