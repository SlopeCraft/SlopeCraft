#ifndef SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BASECOLOR_H
#define SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BASECOLOR_H

#include <QGroupBox>
#include <Block.h>
#include <functional>
#include <QLabel>

#include <tl/expected.hpp>

class BaseColorWidget;

namespace Ui {
class BaseColorWidget;
}

using select_callback_t = std::function<int(
    const std::vector<const SlopeCraft::mc_block_interface*>& blks)>;

class BaseColorWidget : public QGroupBox {
  Q_OBJECT
 private:
  std::unique_ptr<Ui::BaseColorWidget> ui;
  const uint8_t basecolor{255};
  std::vector<BlockWidget*> blocks;
  std::vector<std::unique_ptr<QLabel>> place_holders;

 public:
  explicit BaseColorWidget(QWidget* parent, uint8_t _basecolor);
  ~BaseColorWidget();

  void set_color(uint32_t argb32) noexcept;

  void add_block(SlopeCraft::mc_block_interface* ab) noexcept;

  // return the num of removed blocks
  tl::expected<size_t, QString> remove_blocks(
      const std::function<bool(const SlopeCraft::mc_block_interface*)>&
          remove_this_block) noexcept;

  void finish_blocks() noexcept;

  void select_block_soft(int idx) noexcept;

  void when_version_updated(SCL_gameVersion v) noexcept;

  int selected_idx() const noexcept;

  bool is_enabled() const noexcept;

  const SlopeCraft::mc_block_interface* selected_block() const noexcept {
    return this->blocks[this->selected_idx()]->attached_block();
  }

  void update_lang(SCL_language lang) noexcept;

  void set_enabled(bool enabled) noexcept;

  inline auto& block_widgets() noexcept { return this->blocks; }

  inline const auto& block_widgets() const noexcept { return this->blocks; }

  void select_by_callback(const select_callback_t& fun);

 signals:
  void changed();

 private:
  void add_placeholders() noexcept;

  void re_arrange_blocks() noexcept;

  void select_block_direct(int idx) noexcept;

  int prefered_block_idx(int checked_idx, SCL_gameVersion ver) const noexcept;

  int prefered_block_idx(SCL_gameVersion ver) const noexcept {
    return this->prefered_block_idx(this->selected_idx(), ver);
  }
};

#endif  // SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BASECOLOR_H
