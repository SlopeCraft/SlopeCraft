#ifndef SLOPECRAFT_SLOPECRAFT_COMPRESSEFFECTVIEWER_H
#define SLOPECRAFT_SLOPECRAFT_COMPRESSEFFECTVIEWER_H

#include <QDialog>
class SCWind;

class CompressEffectViewer;

namespace Ui {
class CompressEffectViewer;
}

class CompressEffectViewer : public QDialog {
  Q_OBJECT
 private:
  Ui::CompressEffectViewer* const ui;

 public:
  explicit CompressEffectViewer(SCWind* parent);
  ~CompressEffectViewer();
 private slots:
  void on_pb_save_image_clicked() noexcept;
};

#endif  // SLOPECRAFT_SLOPECRAFT_COMPRESSEFFECTVIEWER_H