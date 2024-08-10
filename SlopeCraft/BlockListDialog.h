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

class BLD_block_list_provider : public QAbstractListModel {
  Q_OBJECT
 public:
  const std::function<std::vector<
      std::pair<QString, const SlopeCraft::block_list_interface*>>()>
      available_block_lists;

 public:
  explicit BLD_block_list_provider(
      QWidget* parent,
      std::function<std::vector<
          std::pair<QString, const SlopeCraft::block_list_interface*>>()>
          cb)
      : QAbstractListModel{parent}, available_block_lists{std::move(cb)} {}
  BLD_block_list_provider(const BLD_block_list_provider&) = delete;

  int rowCount(const QModelIndex& parent = QModelIndex()) const final;

  QVariant data(const QModelIndex& index,
                int role = Qt::DisplayRole) const final;
};
class BLD_block_provider : public QAbstractListModel {
  Q_OBJECT
 private:
  const std::function<const SlopeCraft::block_list_interface*()>
      available_block_list;
  const std::function<SCL_language()> current_lang;

 public:
  explicit BLD_block_provider(
      QWidget* parent,
      std::function<const SlopeCraft::block_list_interface*()>&& cb,
      std::function<SCL_language()>&& lang_cb)
      : QAbstractListModel{parent},
        available_block_list{std::move(cb)},
        current_lang{std::move(lang_cb)} {}

  std::vector<const SlopeCraft::mc_block_interface*> available_blocks()
      const noexcept;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
};

class BLD_block_info_provider : public QAbstractTableModel {
  Q_OBJECT
 public:
  const std::function<const SlopeCraft::mc_block_interface*()> selected_block;

 public:
  explicit BLD_block_info_provider(
      QWidget* parent,
      std::function<const SlopeCraft::mc_block_interface*()>&& cb)
      : QAbstractTableModel{parent}, selected_block{std::move(cb)} {}

  int rowCount(const QModelIndex& qmi) const final;

  int columnCount(const QModelIndex& qmi) const final;

  static QString key_name(int index) noexcept;
  static QVariant value_of_attribute(const SlopeCraft::mc_block_interface& blk,
                                     int index) noexcept;

  QVariant data(const QModelIndex& qmi, int role) const noexcept final;
};

class SCWind;
class BlockListDialog : public QDialog {
  Q_OBJECT
 private:
  std::unique_ptr<Ui::BlockListDialog> ui;
  std::unique_ptr<BLD_block_list_provider> block_list_provider{nullptr};
  std::unique_ptr<BLD_block_provider> block_provider{nullptr};
  std::unique_ptr<BLD_block_info_provider> block_info_provider{nullptr};

  BlockListManager* const block_list_manager;

  void update_info(const SlopeCraft::mc_block_interface*) noexcept;

 public:
  explicit BlockListDialog(SCWind* parent, BlockListManager* blm);
  ~BlockListDialog();

 private slots:
  void on_pb_add_block_list_clicked() noexcept;
  void on_pb_remove_block_list_clicked() noexcept;
};

#endif  // SLOPECRAFT_BLOCKLISTDIALOG_H
