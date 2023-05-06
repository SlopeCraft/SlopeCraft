#ifndef SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCKLISTMANAGER_H
#define SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCKLISTMANAGER_H

#include <QWidget>
#include <SlopeCraftL.h>
#include <QVBoxLayout>
#include "BaseColor.h"
#include <memory>

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
  void operator()(SlopeCraft::BlockListInterface *ptr) noexcept {
    SlopeCraft::SCL_destroyBlockList(ptr);
  }
};

class BlockListManager : public QWidget {
  Q_OBJECT
 private:
  std::vector<BaseColorWidget *> basecolors;
  std::vector<std::unique_ptr<SlopeCraft::BlockListInterface, BlockListDeleter>>
      blockslists;
  std::function<SCL_gameVersion()> callback_get_version{nullptr};

 public:
  explicit BlockListManager(QWidget *parent = nullptr);
  ~BlockListManager();

  void setup_basecolors(const SlopeCraft::Kernel *kernel) noexcept;

  bool add_blocklist(QString filename, QString dirname) noexcept;

  void finish_blocklist() noexcept;

  void set_version_callback(
      const std::function<SCL_gameVersion()> &cb) noexcept {
    this->callback_get_version = cb;
  }

  SCL_gameVersion version() const noexcept {
    return this->callback_get_version();
  }

  void when_version_updated() noexcept;

 private:
  bool impl_addblocklist(const QString &filename, const QString &dirname,
                         std::unique_ptr<SlopeCraft::BlockListInterface,
                                         BlockListDeleter> &dst) noexcept;
};

#endif  // SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCKLISTMANAGER_H
