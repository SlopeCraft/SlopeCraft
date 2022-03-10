#ifndef CUTTERWIND_H
#define CUTTERWIND_H

#include <QMainWindow>
#include <QImage>
QT_BEGIN_NAMESPACE
namespace Ui { class CutterWind; }
QT_END_NAMESPACE

class CutterWind : public QMainWindow
{
    Q_OBJECT

public:
    CutterWind(QWidget *parent = nullptr);
    ~CutterWind();
private slots:
    void loadImg();
    void saveImg();

    void cutImg();

private:
    void updateImg() const;
    void resizeImg();
    QImage img;
    QString netRawFileName;
    QString rawFileSuffix;
    Ui::CutterWind *ui;
};
#endif // CUTTERWIND_H
