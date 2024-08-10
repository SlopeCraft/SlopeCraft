#ifndef SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCK_H
#define SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCK_H

#include <QWidget>
#include <QRadioButton>

#include <SlopeCraftL.h>

class BlockWidget : public QRadioButton {
  Q_OBJECT
 private:
  SlopeCraft::mc_block_interface* const block;

 public:
  explicit BlockWidget(QWidget* parent, SlopeCraft::mc_block_interface* _blk);
  ~BlockWidget() = default;

  auto attached_block() const noexcept { return this->block; }

  void update_lang(SCL_language lang) noexcept;
};

#endif  // SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCK_H
