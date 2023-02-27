#ifndef SLOPECRAFT_VISUALCRAFT_VC_BLOCK_CLASS_H
#define SLOPECRAFT_VISUALCRAFT_VC_BLOCK_CLASS_H

#include <QGroupBox>
#include <VisualCraftL.h>
#include <utility>
#include <vector>

namespace Ui {
class VC_block_class;
}

class QCheckBox;

class VC_block_class : public QGroupBox {
  Q_OBJECT
private:
  Ui::VC_block_class *ui;
  std::vector<std::pair<VCL_block *, QCheckBox *>> blocks;

public:
  explicit VC_block_class(QWidget *parent);
  ~VC_block_class();

  void set_blocks(size_t num_blocks, VCL_block *const *const blocks,
                  size_t cols = 3) noexcept;

  size_t selected_blocks(std::vector<VCL_block *> *select_blks,
                         bool append_to_select_blks = false) const noexcept;

  const auto &blocks_vector() const noexcept { return this->blocks; }

  QCheckBox *chbox_enabled() noexcept;

private:
  void erase_blocks() noexcept;

  void set_state_for_all(bool checked) noexcept;

private slots:
};

#endif // SLOPECRAFT_VISUALCRAFT_VC_BLOCK_CLASS_H