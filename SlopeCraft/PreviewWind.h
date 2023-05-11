#ifndef SLOPECRAFT_SLOPECRAFT_PREVIEWWIND_H
#define SLOPECRAFT_SLOPECRAFT_PREVIEWWIND_H

#include <QDialog>
#include <SlopeCraftL.h>
class PreviewWind;

namespace Ui {
class PreviewWind;
}

class PreviewWind : public QDialog {
  Q_OBJECT
 private:
  Ui::PreviewWind* ui;

  void setup_widget(const SlopeCraft::Kernel* kernel) noexcept;

 public:
  explicit PreviewWind(const SlopeCraft::Kernel* kernel,
                       QWidget* parent = nullptr);
  ~PreviewWind();
};

#endif  // SLOPECRAFT_SLOPECRAFT_PREVIEWWIND_H