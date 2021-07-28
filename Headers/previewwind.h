#ifndef PREVIEWWIND_H
#define PREVIEWWIND_H

#include <QMainWindow>
#include <vector>
#include <QMainWindow>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <iostream>
using namespace std;

namespace Ui {
class PreviewWind;
}

class PreviewWind : public QMainWindow
{
    Q_OBJECT

public:
    explicit PreviewWind(QWidget *parent = nullptr);

    vector<QRadioButton *> Src;
    vector<int>BlockCount;
    vector<QLabel*>CountLabel;
    QRadioButton*Water;
    int size[3];
    int TotalBlockCount;
    void ShowMaterialList();
    ~PreviewWind();

private slots:
    void on_SwitchUnit_clicked(bool checked);

private:
    Ui::PreviewWind *ui;
};

QString blockCount2string(int,int=64);
#endif // PREVIEWWIND_H
