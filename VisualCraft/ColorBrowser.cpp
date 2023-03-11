#include "ColorBrowser.h"
#include "ui_ColorBrowser.h"
#include <QImage>
#include <QLabel>
#include <VCWind.h>
#include <VisualCraftL.h>
#include <array>
#include <vector>
/*
class private_class_setup_chart : public QThread {
public:
  ColorBrowser *cb{nullptr};
  QThread *original_thread;

protected:
  void run() {
    cb->setup_table();
    cb->moveToThread(this->original_thread);
  }
};
*/
ColorBrowser::ColorBrowser(QWidget *parent)
    : QWidget(parent), ui(new Ui::ColorBrowser) {
  this->ui->setupUi(this);
}

ColorBrowser::~ColorBrowser() {
  delete this->ui;
  // delete this->thread;
}

void compose_blocks(QImage &dst, const QImage &src, int idx,
                    int margin) noexcept {
  assert(dst.height() == src.height());
  assert(src.height() == src.width());

  const int rows = src.height();

  const int col_begin = idx * (rows + margin);
  const int col_end = col_begin + rows;

  assert(dst.width() >= col_end);

  for (int r = 0; r < rows; r++) {
    uint32_t *const dst_ptr = (uint32_t *)dst.scanLine(r);
    const uint32_t *const src_ptr = (const uint32_t *)src.scanLine(r);

    memcpy(dst_ptr + col_begin, src_ptr, rows * sizeof(uint32_t));
  }
}

void ColorBrowser::setup_table() noexcept {
  const int layers = VCL_get_max_block_layers();
  const int color_count = VCL_num_basic_colors();

  const int col_offset = this->ui->table->columnCount();

  this->ui->table->setRowCount(color_count);
  this->ui->table->setColumnCount(col_offset + layers);

  for (int l = 0; l < layers; l++) {
    this->ui->table->setHorizontalHeaderItem(
        l + col_offset,
        new QTableWidgetItem(ColorBrowser::tr("第%1个方块").arg(l + 1)));
  }

  constexpr int col_color_idx = 0;
  constexpr int col_color_grid = 1;
  constexpr int col_color_hex = 2;
  constexpr int col_block_icons = 3;

  std::vector<std::pair<std::vector<const VCL_block *>, uint32_t>> mat_block;
  mat_block.resize(color_count);

  std::unordered_map<const VCL_block *, QImage> block_images;

  // get color and block composition
  for (int idx = 0; idx < color_count; idx++) {
    auto &pair = mat_block[idx];
    pair.first.resize(layers);

    const int num =
        VCL_get_basic_color_composition(idx, pair.first.data(), &pair.second);

    if (num <= 0) {
#warning here
      abort();
    }

    pair.first.resize(num);

    for (const VCL_block *blkp : pair.first) {
      block_images.emplace(blkp, QImage{});
    }
  }

  // compute projection images
  for (auto &pair : block_images) {
    QImage proj(16, 16, QImage::Format_ARGB32);
    memset(proj.scanLine(0), 0x00, proj.sizeInBytes());

    VCL_model *const model =
        VCL_get_block_model(pair.first, VCL_get_resource_pack());
    if (model == nullptr) {
#warning report as warning

      pair.second = std::move(proj);
      continue;
    }

    const bool ok = VCL_compute_projection_image(
        model, VCL_get_exposed_face(), nullptr, nullptr,
        reinterpret_cast<uint32_t *>(proj.scanLine(0)), proj.sizeInBytes());

    if (!ok) {
#warning report as warning
      pair.second = std::move(proj);
      VCL_destroy_block_model(model);
      continue;
    }

    pair.second = std::move(proj);
    VCL_destroy_block_model(model);
  }

  // display charts

  constexpr int margin = 4;

  for (int r = 0; r < color_count; r++) {
    this->ui->table->setItem(r, col_color_idx,
                             new QTableWidgetItem(QString::number(r)));

    const uint32_t color = mat_block[r].second;
    const auto &blocks = mat_block[r].first;
    {
      auto qtwi = new QTableWidgetItem;
      this->ui->table->setItem(r, col_color_grid, qtwi);
      qtwi->setBackground(QColor(color));
    }

    this->ui->table->setItem(
        r, col_color_hex,
        new QTableWidgetItem("0x" +
                             QString::number(color & 0x00'FF'FF'FF, 16)));

    QImage img(16 * blocks.size() + margin * (blocks.size() - 1), 16,
               QImage::Format_ARGB32);
    memset(img.scanLine(0), 0x00, img.sizeInBytes());

    for (int l = 0; l < (int)blocks.size(); l++) {
      const VCL_block *blkp = blocks[l];

      this->ui->table->setItem(
          r, col_offset + l,
          new QTableWidgetItem(
              QString::fromUtf8(VCL_get_block_name(blkp, ::is_language_ZH))));

      compose_blocks(img, block_images.at(blkp), l, margin);
    }

    QLabel *const label = new QLabel(this);

    label->setPixmap(QPixmap::fromImage(img));

    this->ui->table->setCellWidget(r, col_block_icons, label);
  }
}
/*
void ColorBrowser::setup_table_threaded() noexcept {
  this->thread = new private_class_setup_chart;
  this->thread->cb = this;
  this->thread->original_thread = QThread::currentThread();

  this->moveToThread(this->thread->thread());

  this->thread->start();
}
*/