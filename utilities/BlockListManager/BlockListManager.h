#ifndef SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCKLISTMANAGER_H
#define SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCKLISTMANAGER_H

#include <memory>
#include <tl/expected.hpp>

#include <QWidget>
#include <SlopeCraftL.h>
#include <QVBoxLayout>
#include "BaseColor.h"

struct basecolorOption {
  uint8_t baseColor{0xFF};
  bool enabled{false};
  QString blockId{""};
};

struct blockListPreset {
  std::vector<std::pair<bool, QString>> values;
};

blockListPreset load_preset(QString filename) noexcept(false);
blockListPreset load_preset(QString filename, QString &err) noexcept;

QString serialize_preset(const blockListPreset &preset) noexcept;

class BlockListDeleter {
 public:
  void operator()(SlopeCraft::block_list_interface *ptr) const noexcept {
    SlopeCraft::SCL_destroy_block_list(ptr);
  }
};

struct selection {
  std::vector<std::string> ids;

  [[nodiscard]] bool operator==(const selection &b) const noexcept {
    if (this->ids.size() != b.ids.size()) {
      return false;
    }
    for (size_t i = 0; i < this->ids.size(); i++) {
      if (this->ids[i] != b.ids[i]) {
        return false;
      }
    }
    return true;
  }
};

template <>
struct std::hash<selection> {
  uint64_t operator()(const selection &s) const noexcept;
};

class BlockListManager : public QWidget {
  Q_OBJECT
 private:
  std::vector<std::unique_ptr<BaseColorWidget>> basecolor_widgets;
  std::vector<
      std::pair<QString, std::unique_ptr<SlopeCraft::block_list_interface,
                                         BlockListDeleter>>>
      blockslists;
  std::function<SCL_gameVersion()> callback_get_version{nullptr};

 public:
  explicit BlockListManager(QWidget *parent = nullptr);
  ~BlockListManager();

  void setup_basecolors() noexcept;

  bool add_blocklist(QString filename) noexcept;

  tl::expected<size_t, QString> remove_blocklist(
      QString blocklist_name) noexcept;

  void finish_blocklist() noexcept;

  void set_version_callback(
      const std::function<SCL_gameVersion()> &cb) noexcept {
    this->callback_get_version = cb;
  }

  SCL_gameVersion version() const noexcept {
    return this->callback_get_version();
  }

  void when_version_updated() noexcept;

  void when_lang_updated(SCL_language lang) noexcept {
    for (auto &bcw : this->basecolor_widgets) {
      bcw->update_lang(lang);
    }
  }

  void select_block_by_callback(const select_callback_t &fun) noexcept {
    for (auto &bcw : this->basecolor_widgets) {
      bcw->select_by_callback(fun);
    }
  }

  void get_blocklist(std::vector<uint8_t> &enable_list,
                     std::vector<const SlopeCraft::mc_block_interface *>
                         &block_list) const noexcept;

  bool loadPreset(const blockListPreset &preset) noexcept;

  blockListPreset to_preset() const noexcept;

  size_t num_basecolor_widgets() const noexcept {
    assert(this->basecolor_widgets.size() < 256);
    return this->basecolor_widgets.size();
  }

  BaseColorWidget *basecolorwidget_at(size_t basecolor) noexcept {
    return this->basecolor_widgets[basecolor].get();
  }

  const BaseColorWidget *basecolorwidget_at(size_t basecolor) const noexcept {
    return this->basecolor_widgets[basecolor].get();
  }

  [[nodiscard]] selection current_selection() const noexcept;

  [[nodiscard]] std::vector<
      std::pair<QString, const SlopeCraft::block_list_interface *>>
  get_block_lists() const noexcept {
    std::vector<std::pair<QString, const SlopeCraft::block_list_interface *>>
        ret;
    for (auto &[name, list] : this->blockslists) {
      ret.emplace_back(name, list.get());
    }
    return ret;
  }

 signals:
  void changed();

 private:
  std::unique_ptr<SlopeCraft::block_list_interface, BlockListDeleter>
  impl_addblocklist(const QByteArray &file_content) noexcept;
};

#endif  // SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCKLISTMANAGER_H
