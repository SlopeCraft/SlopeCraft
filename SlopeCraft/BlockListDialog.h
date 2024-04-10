//
// Created by Joseph on 2024/4/9.
//

#ifndef SLOPECRAFT_BLOCKLISTDIALOG_H
#define SLOPECRAFT_BLOCKLISTDIALOG_H

#include <QDialog>
#include <memory>

class BlockListDialog;

namespace Ui {
class BlockListDialog;
}

class BlockListDialog : public QDialog {
  Q_OBJECT
 private:
  std::unique_ptr<Ui::BlockListDialog> ui;

 public:
  explicit BlockListDialog(QWidget* parent);
  ~BlockListDialog();
};

#endif  // SLOPECRAFT_BLOCKLISTDIALOG_H
