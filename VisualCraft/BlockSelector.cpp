#include "BlockSelector.h"
#include "ui_BlockSelector.h"
#include <VCWind.h>

BlockSelector::BlockSelector(QWidget *parent)
    : QWidget(parent), ui(new Ui::BlockSelector) {
  this->ui->setupUi(this);

  this->emplace_back();

  this->when_criteria_changed();
}

BlockSelector::~BlockSelector() { delete this->ui; }

void BlockSelector::emplace_back() noexcept {
  BlockSelectorCriteria *cr = new BlockSelectorCriteria(this);

  this->criterias.emplace_back(cr);

  this->ui->layout_criterias->addWidget(cr);

  connect(cr, &BlockSelectorCriteria::append, this,
          &BlockSelector::emplace_back);
  connect(cr, &BlockSelectorCriteria::remove, this, &BlockSelector::remove_one);
  connect(cr, &BlockSelectorCriteria::options_changed, this,
          &BlockSelector::when_criteria_changed);

  this->update_criteria_roles();
  this->when_criteria_changed();
  // cr->set_role(this->criterias.size() <= 1, true);
}

void BlockSelector::update_criteria_roles() noexcept {
  for (size_t idx = 0; idx < this->criterias.size(); idx++) {
    const bool is_first = idx <= 0;
    const bool is_last = (idx + 1) >= this->criterias.size();

    this->criterias[idx]->set_role(is_first, is_last);
  }
}

void BlockSelector::remove_one(BlockSelectorCriteria *bsc) noexcept {
  auto it = this->criterias.begin();

  for (; it != this->criterias.end(); ++it) {
    if (*it == bsc) {
      break;
    }
  }

  assert(it != this->criterias.end());

  this->criterias.erase(it);

  delete bsc;

  this->update_criteria_roles();
  this->when_criteria_changed();
}

void BlockSelector::when_criteria_changed() noexcept {
  const int count = dynamic_cast<VCWind *>(this->parent())
                        ->count_block_matched(this->match_functor());

  this->ui->label_show_count->setText(
      BlockSelector::tr("匹配到%1个方块").arg(count));
}

std::function<bool(const VCL_block *)>
BlockSelector::match_functor() const noexcept {

  bs_criteria cr;

  for (auto bsc : this->criterias) {
    bsc->update_criteria(cr);
  }
  return std::bind(&bs_criteria::match, cr, std::placeholders::_1);
}

void BlockSelector::on_pb_select_matched_clicked() noexcept {
  dynamic_cast<VCWind *>(this->parent())->select_blocks(this->match_functor());
}

void BlockSelector::on_pb_deselect_matched_clicked() noexcept {
  dynamic_cast<VCWind *>(this->parent())
      ->deselect_blocks(this->match_functor());
}