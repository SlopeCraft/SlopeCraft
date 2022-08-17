#ifndef MAPVIEWERWIND_H
#define MAPVIEWERWIND_H

#include <QMainWindow>

#include <vector>
#include <QString>

#include <Eigen/Dense>

#include <iostream>

#include <memory>

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
public:
    map() : map_content(new Eigen::Array<uint8_t,128,128,Eigen::RowMajor>){};
    ~map()=default;
    map(map&&)=default;
    map(const map & another) : filename(another.filename), image(another.image) {
        *map_content=*another.map_content;
    }

    map & operator=(const map & another) {
        filename=another.filename;
        image=another.image;
        memcpy(map_content->data(),another.map_content->data(),128*128);

        return *this;
    }

    QString filename;
    std::unique_ptr<Eigen::Array<uint8_t,128,128,Eigen::RowMajor>> map_content;
    QPixmap image;

    inline Eigen::Array<uint8_t,128,128,Eigen::RowMajor>& content() {
        return *map_content;
    }

    inline const Eigen::Array<uint8_t,128,128,Eigen::RowMajor>& content() const {
        return *map_content;
    }
};

class MapViewerWind : public QMainWindow
{
    Q_OBJECT

public:
    MapViewerWind(QWidget *parent = nullptr);
    ~MapViewerWind();

private:
    Ui::MapViewerWind *ui;

    std::vector<map> maps;

private slots:
    void update_contents();
    void reshape_tables();
    void clear_all();
    void on_button_load_maps_clicked();
};



#endif // MAPVIEWERWIND_H
