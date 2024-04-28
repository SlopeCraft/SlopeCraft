/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef MAPVIEWERWIND_H
#define MAPVIEWERWIND_H

#include <QMainWindow>
#include <QLabel>
#include <QString>
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <memory>

using std::cout, std::endl;

QT_BEGIN_NAMESPACE
namespace Ui {
class MapViewerWind;
}
QT_END_NAMESPACE

using ARGB = uint32_t;
using u8Array128RowMajor = Eigen::Array<uint8_t, 128, 128, Eigen::RowMajor>;
using u32Array128RowMajor = Eigen::Array<ARGB, 128, 128, Eigen::RowMajor>;

extern const std::array<ARGB, 256> map_color_to_ARGB;

enum single_map_draw_type {  // the value of draw_type is the digits required
  color_only = 0,
  map_color = 3,
  base_color = 2,
  shade = 1
};

struct map {
 public:
  map() : map_content(new u8Array128RowMajor){};
  ~map() = default;
  map(map &&) = default;
  map(const map &another) : filename(another.filename), image(another.image) {
    *map_content = *another.map_content;
  }

  map &operator=(const map &another) {
    filename = another.filename;
    image = another.image;
    memcpy(map_content->data(), another.map_content->data(), 128 * 128);

    return *this;
  }

  QString filename;
  std::unique_ptr<u8Array128RowMajor> map_content;
  QImage image;

  inline u8Array128RowMajor &content() { return *map_content; }

  inline const u8Array128RowMajor &content() const { return *map_content; }
};

class MapViewerWind : public QMainWindow {
  Q_OBJECT

 public:
  MapViewerWind(QWidget *parent = nullptr);
  ~MapViewerWind();

 private:
  Ui::MapViewerWind *ui;

  std::vector<map> maps;
  std::vector<QLabel *> labels;

 private:
 private slots:
  void update_contents();
  void reshape_tables();
  void render_single_image();
  void render_composed();
  void clear_all();
  void on_button_load_maps_clicked();
  void on_checkbox_composed_show_spacing_toggled(bool);
  void on_button_save_single_clicked();
  void on_button_save_composed_clicked();
};

#endif  // MAPVIEWERWIND_H
