#ifndef SLOPECRAFT_VISUALCRAFT_COLORBROWSER_H
#define SLOPECRAFT_VISUALCRAFT_COLORBROWSER_H
#include <QWidget>

class ColorBrowser;

namespace Ui {
class ColorBrowser;
}

class ColorBrowser : public QWidget {
  Q_OBJECT
private:
  Ui::ColorBrowser *ui;

public:
  explicit ColorBrowser(QWidget *parent);
  ~ColorBrowser();
};

#endif // SLOPECRAFT_VISUALCRAFT_COLORBROWSER_H