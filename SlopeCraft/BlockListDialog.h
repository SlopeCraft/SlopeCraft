//
// Created by Joseph on 2024/4/9.
//

#ifndef SLOPECRAFT_BLOCKLISTDIALOG_H
#define SLOPECRAFT_BLOCKLISTDIALOG_H

#include <memory>
#include <functional>
#include <optional>

#include <QDialog>
#include <QAbstractTableModel>

#include "BlockListManager.h"

class BlockListDialog;

namespace Ui {
class BlockListDialog;
}

class BLD_block_list_provider;
class BLD_block_provider;
class BLD_block_info_provider;

class SCWind;
class BlockListDialog : public QDialog {
  Q_OBJECT
 private:
  std::unique_ptr<Ui::BlockListDialog> ui;
  BLD_block_list_provider* block_list_provider{nullptr};
  BLD_block_provider* block_provider{nullptr};
  BLD_block_info_provider* block_info_provider{nullptr};

  void update_info(const SlopeCraft::mc_block_interface*) noexcept;

 public:
  explicit BlockListDialog(SCWind* parent, BlockListManager* blm);
  ~BlockListDialog();
};

#endif  // SLOPECRAFT_BLOCKLISTDIALOG_H
