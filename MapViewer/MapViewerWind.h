#ifndef MAPVIEWERWIND_H
#define MAPVIEWERWIND_H

#include <QMainWindow>

#include <vector>
#include <QString>

#include <Eigen/Dense>

#include <iostream>

using std::cout,std::endl;

QT_BEGIN_NAMESPACE
namespace Ui { class MapViewerWind; }
QT_END_NAMESPACE

namespace SlopeCraft{
extern const float RGBBasicSource[256*3];
}

using ARGB=uint32_t;

extern const std::array<ARGB,256> map_color_to_ARGB;

struct map
{
    QString filename;
    Eigen::ArrayXX<uint8_t> map_content;
    QPixmap image;
};

class MapViewerWind : public QMainWindow
{
    Q_OBJECT

public:
    MapViewerWind(QWidget *parent = nullptr);
    ~MapViewerWind();

private:
    Ui::MapViewerWind *ui;

    std::vector<QString> map_filenames;

private slots:
    void reshape_tables();
    void clear_all();
};



#endif // MAPVIEWERWIND_H
