#include "BaseColor.h"
#include "ui_BaseColorWidget.h"
#include "BlockListManager.h"

BaseColorWidget::BaseColorWidget(QWidget* parent, uint8_t _basecolor)
    : QGroupBox(parent), ui(new Ui::BaseColorWidget), basecolor(_basecolor) {
  this->ui->setupUi(this);

  connect(this->ui->cb_enable, &QCheckBox::toggled, this,
          &BaseColorWidget::changed);
}

BaseColorWidget::~BaseColorWidget() {}

void BaseColorWidget::set_color(uint32_t color) noexcept {
  QPalette pl;
  pl.setColor(QPalette::ColorRole::Text, Qt::black);
  pl.setColor(QPalette::ColorRole::Window,
              QColor(qRed(color), qGreen(color), qBlue(color),
                     255 * bool(this->basecolor)));

  this->setPalette(pl);

  if (this->basecolor == 0) {
    this->ui->cb_enable->setDisabled(true);
  }
}

constexpr int basecolor_cols = 3;
static_assert(basecolor_cols >= 1);

void BaseColorWidget::add_block(SlopeCraft::mc_block_interface* ab) noexcept {
  this->place_holders.clear();
  BlockWidget* bw = new BlockWidget(this, ab);

  const int idx = this->blocks.size();

  this->blocks.emplace_back(bw);
  constexpr int cols = basecolor_cols;

  const int col = idx % cols;
  const int row = idx / cols;

  dynamic_cast<QGridLayout*>(this->ui->layout_blocks)->addWidget(bw, row, col);

  connect(bw, &QRadioButton::toggled, this, &BaseColorWidget::changed);
}

void BaseColorWidget::add_placeholders() noexcept {
  if (this->blocks.size() <= 0) {
    this->place_holders.clear();
    return;
  }

  if (this->blocks.size() < basecolor_cols) {
    this->place_holders.clear();
    for (int i = this->blocks.size(); i < basecolor_cols; i++) {
      auto lb = new QLabel("", this);
      // lb->setFrameShape(QLabel::Shape::Box);
      dynamic_cast<QGridLayout*>(this->ui->layout_blocks)->addWidget(lb, 0, i);
      this->place_holders.emplace_back(lb);
    }
    return;
  }

  if (this->blocks.size() % basecolor_cols == 0) {
    this->place_holders.clear();
  }
}

void BaseColorWidget::re_arrange_blocks() noexcept {
  this->place_holders.clear();
  auto layout = dynamic_cast<QGridLayout*>(this->ui->layout_blocks);
  for (auto& bw : this->blocks) {
    assert(layout->indexOf(bw) >= 0);
    layout->removeWidget(bw);
  }

  constexpr int cols = basecolor_cols;
  for (size_t idx = 0; idx < this->blocks.size(); idx++) {
    const int col = idx % cols;
    const int row = idx / cols;
    layout->addWidget(this->blocks[idx], row, col);
  }
  this->finish_blocks();
}

tl::expected<size_t, QString> BaseColorWidget::remove_blocks(
    const std::function<bool(const SlopeCraft::mc_block_interface*)>&
        remove_this_block) noexcept {
  size_t remove_counter = 0;
  for (auto it = this->blocks.begin(); it not_eq this->blocks.end();) {
    const bool remove_current = remove_this_block((*it)->attached_block());
    if (not remove_current) {
      ++it;
      continue;
    }
    if (this->blocks.size() <= 1) {
      return tl::make_unexpected(
          tr("无法删除方块 %1，基色 %2 只拥有 "
             "%3个方块，若继续删除，则该基色将没有方块，SlopeCraft 可能崩溃。")
              .arg((*it)->text())
              .arg(int(this->basecolor), this->blocks.size()));
    }

    (*it)->deleteLater();
    it = this->blocks.erase(it);
    remove_counter++;
  }
  this->re_arrange_blocks();
  return remove_counter;
}

constexpr inline bool should_be_disabled(
    int, SCL_gameVersion block_ver, int num_blocks,
    SCL_gameVersion selected_ver) noexcept {
  if (num_blocks <= 1) {
    return true;
  }

  if (block_ver > selected_ver) {
    return true;
  }

  return false;
}

void BaseColorWidget::finish_blocks() noexcept {
  this->add_placeholders();
  assert(this->blocks.size() > 0);

  this->select_block_direct(0);

  const SCL_gameVersion v =
      reinterpret_cast<const BlockListManager*>(this->parent())->version();

  for (int idx = 0; idx < (int)this->blocks.size(); idx++) {
    this->blocks[idx]->setDisabled(should_be_disabled(
        idx, (SCL_gameVersion)this->blocks[idx]->attached_block()->getVersion(),
        this->blocks.size(), v));
  }
}

void BaseColorWidget::select_block_direct(int idx) noexcept {
  assert(idx >= 0);

  assert(idx < (int)this->blocks.size());
  /*
  const SCL_gameVersion v =
      reinterpret_cast<const BlockListManager*>(this->parent())->version();

   assert(this->blocks[idx]->attachted_block()->getVersion() <= (int)v);
   */

  this->blocks[idx]->setChecked(true);
}

void BaseColorWidget::select_block_soft(int idx) noexcept {
  const int new_select = this->prefered_block_idx(
      idx, dynamic_cast<BlockListManager*>(this->parent())->version());

  this->select_block_direct(new_select);
}

void BaseColorWidget::when_version_updated(SCL_gameVersion v) noexcept {
  const int new_select = this->prefered_block_idx(v);

  this->select_block_direct(new_select);

  for (int idx = 0; idx < int(this->blocks.size()); idx++) {
    this->blocks[idx]->setDisabled(should_be_disabled(
        idx, (SCL_gameVersion)this->blocks[idx]->attached_block()->getVersion(),
        this->blocks.size(), v));
  }
  if (this->basecolor == 0) {  // basecolor 0 (air) must be selected
    return;
  }
  const bool is_version_ok =
      SlopeCraft::SCL_basecolor_version(this->basecolor) <= v;
  this->ui->cb_enable->setChecked(is_version_ok);
  this->ui->cb_enable->setEnabled(is_version_ok);
}

int BaseColorWidget::selected_idx() const noexcept {
  for (int idx = 0; idx < (int)this->blocks.size(); idx++) {
    if (this->blocks[idx]->isChecked()) {
      return idx;
    }
  }
  assert(false);
  return 0;
}

int BaseColorWidget::prefered_block_idx(int checked_idx,
                                        SCL_gameVersion ver) const noexcept {
  std::vector<int> scores;
  scores.resize(this->blocks.size());
  assert(checked_idx < (int)scores.size());
  assert(checked_idx >= 0);

  for (int idx = 0; idx < int(scores.size()); idx++) {
    int& score = scores[idx];
    score = 0;

    const SCL_gameVersion blk_ver =
        (SCL_gameVersion)this->blocks[idx]->attached_block()->getVersion();

    if (blk_ver <= ver) {
      score = 100;
    } else {
      score = 50;
    }

    if (idx == checked_idx) {
      score += 1;
    }
  }

  int max_idx{0};

  for (int idx = 0; idx < int(scores.size()); idx++) {
    if (scores[idx] > scores[max_idx]) {
      max_idx = idx;
    }
  }

  return max_idx;
}

bool BaseColorWidget::is_enabled() const noexcept {
  return this->ui->cb_enable->isChecked();
}

void BaseColorWidget::set_enabled(bool enabled) noexcept {
  this->ui->cb_enable->setChecked(enabled);
}

void BaseColorWidget::select_by_callback(const select_callback_t& fun) {
  std::vector<const SlopeCraft::mc_block_interface*> blks;
  blks.reserve(this->blocks.size());
  for (auto bw : this->blocks) {
    blks.emplace_back(bw->attached_block());
  }

  const int output = fun(blks);

  if (output < 0 || output >= (int)this->blocks.size()) {
    return;
  }

  this->select_block_soft(output);
}

void BaseColorWidget::update_lang(SCL_language lang) noexcept {
  this->ui->retranslateUi(this);
  for (auto bw : this->blocks) {
    bw->update_lang(lang);
  }
}