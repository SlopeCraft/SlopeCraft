#ifndef SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCKLISTMANAGER_H
#define SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCKLISTMANAGER_H

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

class BlockListManager : public QWidget {
  Q_OBJECT
 private:
  std::vector<BaseColorWidget *> basecolors;

 public:
  explicit BlockListManager(QWidget *parent = nullptr);
  ~BlockListManager();

  void setup_basecolors(SlopeCraft::Kernel *kernel) noexcept;
};

#endif  // SLOPECRAFT_UTILITIES_BLOCKLISTMANAGER_BLOCKLISTMANAGER_H
