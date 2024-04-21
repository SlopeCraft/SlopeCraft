#ifndef SLOPECRAFT_SLOPECRAFT_COMPRESSEFFECTVIEWER_H
#define SLOPECRAFT_SLOPECRAFT_COMPRESSEFFECTVIEWER_H

#include <QDialog>
#include <memory>
#include <SlopeCraftL.h>

class SCWind;

class CompressEffectViewer;

namespace Ui {
class CompressEffectViewer;
}

class CompressEffectViewer : public QDialog {
  Q_OBJECT
 private:
  std::unique_ptr<Ui::CompressEffectViewer> ui;

 public:
  explicit CompressEffectViewer(SCWind* parent,
                                const SlopeCraft::converted_image&,
                                const SlopeCraft::structure_3D&);
  ~CompressEffectViewer();
 private slots:
  void on_pb_save_image_clicked() noexcept;
};

#endif  // SLOPECRAFT_SLOPECRAFT_COMPRESSEFFECTVIEWER_H