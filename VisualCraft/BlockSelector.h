#ifndef SLOPECRAFT_VISUALCRAFT_BLOCKSELECTOR_H
#define SLOPECRAFT_VISUALCRAFT_BLOCKSELECTOR_H

#include <QWidget>
#include <VisualCraftL.h>
#include <vector>

class BlockSelector;
class BlockSelectorCriteria;

namespace Ui {
class BlockSelector;
class BlockSelectorCriteria;
} // namespace Ui

class bs_criteria {
public:
  struct alignas(4) statement {
    bool logic_is_and{true};
    VCL_block_attribute_t key;
    bool value;
  };

  std::vector<statement> statements;

  bool match(const VCL_block *blk) const noexcept;
};

class BlockSelector : public QWidget {
  Q_OBJECT
private:
  Ui::BlockSelector *ui;

  std::vector<BlockSelectorCriteria *> criterias;

public:
  BlockSelector(QWidget *parent);
  ~BlockSelector();

private:
  void update_criteria_roles() noexcept;

  std::function<bool(const VCL_block *)> match_functor() const noexcept;

private slots:
  void emplace_back() noexcept;

  void remove_one(BlockSelectorCriteria *) noexcept;

  void when_criteria_changed() noexcept;

  void on_pb_select_matched_clicked() noexcept;
  void on_pb_deselect_matched_clicked() noexcept;
};

class BlockSelectorCriteria : public QWidget {
  Q_OBJECT
private:
  Ui::BlockSelectorCriteria *ui;

signals:
  void options_changed();

  void append();
  void remove(BlockSelectorCriteria *);

public:
  BlockSelectorCriteria(QWidget *parent);
  ~BlockSelectorCriteria();

  void set_role(bool is_first, bool is_last) noexcept;

  void update_criteria(bs_criteria &cr) const noexcept;

private slots:
  // auto connected
  void on_tb_append_clicked() noexcept;
  void on_tb_remove_clicked() noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFT_BLOCKSELECTOR_H