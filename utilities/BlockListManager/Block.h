#ifndef SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCK_H
#define SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCK_H

#include <QWidget>
#include <QRadioButton>

#include <SlopeCraftL.h>

class BlockWidget : public QRadioButton {
  Q_OBJECT
 private:
  SlopeCraft::AbstractBlock* const block;

 public:
  explicit BlockWidget(QWidget* parent, SlopeCraft::AbstractBlock* _blk);
  ~BlockWidget() = default;

  auto attachted_block() const noexcept { return this->block; }
};

#endif  // SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCK_H
