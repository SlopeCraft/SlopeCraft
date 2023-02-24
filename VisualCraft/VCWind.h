#ifndef SLOPECRAFT_VISUALCRAFT_VCWIND_H
#define SLOPECRAFT_VISUALCRAFT_VCWIND_H

#include <QMainWindow>

namespace Ui {
class VCWind;
}

class VCWind : public QMainWindow {
  Q_OBJECT
private:
  Ui::VCWind *ui;

public:
  explicit VCWind(QWidget *parent = nullptr);
  ~VCWind();
};

#endif // SLOPECRAFT_VISUALCRAFT_VCWIND_H