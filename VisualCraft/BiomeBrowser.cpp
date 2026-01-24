/*
 Copyright © 2021-2026  TokiNoBug
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

#include "BiomeBrowser.h"
#include "VCWind.h"
#include "ui_BiomeBrowser.h"
#include <QPainter>
#include <QPalette>
#include <VisualCraftL.h>
#include <magic_enum/magic_enum.hpp>

BiomeBrowser::BiomeBrowser(QWidget *parent)
    : QWidget(parent), ui(new Ui::BiomeBrowser) {
  this->ui->setupUi(this);

  const auto biomes = magic_enum::enum_values<VCL_biome_t>();
  for (auto b : biomes) {
    QString name = QString::fromUtf8(VCL_biome_name(b, ::is_language_ZH));
    this->ui->cb_select_biome->addItem(name, QVariant::fromValue(b));
  }

  assert(VCL_is_basic_colorset_ok());

  connect(this->ui->cb_select_biome, &QComboBox::currentIndexChanged, this,
          &BiomeBrowser::when_biome_changed);

  connect(this->ui->sb_scale, &QSpinBox::valueChanged, this,
          &BiomeBrowser::refresh_colormap);
  connect(this->ui->cb_show_plot, &QCheckBox::toggled, this,
          &BiomeBrowser::refresh_colormap);
  connect(this->ui->cb_select_colormap, &QComboBox::currentIndexChanged, this,
          &BiomeBrowser::refresh_colormap);
}

BiomeBrowser::~BiomeBrowser() { delete this->ui; }

VCL_biome_t BiomeBrowser::biome_selected() const noexcept {
  assert(this->ui->cb_select_biome->currentIndex() >= 0);

  return this->ui->cb_select_biome->currentData().value<VCL_biome_t>();
}

bool BiomeBrowser::is_grass_selected() const noexcept {
  assert(this->ui->cb_select_colormap->currentIndex() >= 0);

  return this->ui->cb_select_colormap->currentIndex() == 0;
}

void BiomeBrowser::when_biome_changed() noexcept {
  const int idx = this->ui->cb_select_biome->currentIndex();
  if (idx < 0) {
    return;
  }

  const VCL_biome_t b = this->biome_selected();

  const VCL_biome_info info = VCL_get_biome_info(b);

  this->ui->label_show_temp->setText(QString::number(info.temperature));
  this->ui->label_show_downfall->setText(QString::number(info.downfall));

  this->refresh_colormap();
}

void BiomeBrowser::refresh_colormap() noexcept {
  if (this->ui->cb_select_biome->currentIndex() < 0 ||
      this->ui->cb_select_colormap->currentIndex() < 0) {
    this->ui->le_show_color_code->setText("");
    this->ui->label_show_color->setAutoFillBackground(false);
    this->ui->label_show_colormap->setPixmap({});
    return;
  }

  const VCL_biome_t biome = this->biome_selected();
  const VCL_biome_info info = VCL_get_biome_info(biome);

  int r, c;
  const uint32_t current_color = VCL_locate_colormap(
      VCL_get_resource_pack(), this->is_grass_selected(), info, &r, &c);

  this->ui->le_show_color_code->setText(
      BiomeBrowser::tr("当前颜色：") + QStringLiteral("0x") +
      QString::number(current_color & 0x00FF'FF'FF, 16));

  {
    QPalette palette;
    palette.setColor(QPalette::ColorRole::Window, QColor(current_color));
    this->ui->label_show_color->setPalette(palette);
    this->ui->label_show_color->setAutoFillBackground(true);
  }
  int rows, cols;
  const uint32_t *argb = VCL_get_colormap(
      VCL_get_resource_pack(), !this->is_grass_selected(), &rows, &cols);

  const QImage colormap(reinterpret_cast<const uint8_t *>(argb), cols, rows,
                        QImage::Format::Format_ARGB32);

  const int ratio = this->ui->sb_scale->value();
  QImage scaled(cols * ratio, rows * ratio, QImage::Format::Format_ARGB32);
  memset(scaled.scanLine(0), 0xFF, scaled.sizeInBytes());

  for (int sr = 0; sr < rows * ratio; sr++) {
    const int o_r = sr / ratio;
    const uint32_t *const src = (const uint32_t *)colormap.scanLine(o_r);
    uint32_t *const dst = (uint32_t *)scaled.scanLine(sr);
    for (int sc = 0; sc < cols * ratio; sc++) {
      dst[sc] = src[sc / ratio];
    }
  }

  if (this->ui->cb_show_plot->isChecked()) {
    const int scaled_xpos = c * ratio;
    const int scaled_ypos = r * ratio;

    QPainter painter(&scaled);
    {
      QPen pen;
      pen.setColor(Qt::GlobalColor::darkRed);
      pen.setWidth(ratio * 5);
      painter.setPen(pen);
    }
    painter.drawPoint(QPoint(scaled_xpos, scaled_ypos));
    painter.end();
  }

  this->ui->label_show_colormap->setPixmap(QPixmap::fromImage(scaled));
}