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

#ifndef SLOPECRAFT_VISUALCRAFT_COLORBROWSER_H
#define SLOPECRAFT_VISUALCRAFT_COLORBROWSER_H
#include <QWidget>

class ColorBrowser;

namespace Ui {
class ColorBrowser;
}

// class private_class_setup_chart;

class ColorBrowser : public QWidget {
  Q_OBJECT
private:
  Ui::ColorBrowser *ui;
  // private_class_setup_chart *thread{nullptr};

  void setup_table(const uint16_t *const color_id_list,
                   const size_t color_count) noexcept;

public:
  explicit ColorBrowser(QWidget *parent);
  ~ColorBrowser();

  void setup_table_basic() noexcept;
  void setup_table_allowed() noexcept;

  // void setup_table_threaded() noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFT_COLORBROWSER_H