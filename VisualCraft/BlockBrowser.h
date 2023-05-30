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

#ifndef SLOPECRAFT_VISUALCRAFT_BLOCKBROWSER_H
#define SLOPECRAFT_VISUALCRAFT_BLOCKBROWSER_H

#include <QWidget>
#include <VisualCraftL.h>
#include <vector>

class BlockBrowser;

namespace Ui {
class BlockBrowser;
}

class VCWind;

class BlockBrowser : public QWidget {
  Q_OBJECT
 private:
  Ui::BlockBrowser *ui;

  void fetch_content() noexcept;

 private slots:
  // manually connected
  void update_display() noexcept;

  // auto connected
  void on_pb_save_current_image_clicked() noexcept;
  void on_combobox_select_blk_all_currentIndexChanged(int idx) noexcept;

 public:
  explicit BlockBrowser(QWidget *parent);
  ~BlockBrowser();

  VCWind *parent() noexcept;
  const VCWind *parent() const noexcept;
};

#endif  // SLOPECRAFT_VISUALCRAFT_BLOCKBROWSER_H