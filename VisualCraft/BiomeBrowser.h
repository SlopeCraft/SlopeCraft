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
#include <QWidget>
#include <VisualCraftL.h>

class BiomeBrowser;

namespace Ui {
class BiomeBrowser;
}

class BiomeBrowser : public QWidget {
  Q_OBJECT
private:
  Ui::BiomeBrowser *ui;

public:
  BiomeBrowser(QWidget *parent);
  ~BiomeBrowser();

private:
  VCL_biome_t biome_selected() const noexcept;
  bool is_grass_selected() const noexcept;
private slots:
  void when_biome_changed() noexcept;
  void refresh_colormap() noexcept;
};