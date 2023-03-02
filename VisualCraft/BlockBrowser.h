#ifndef SLOPECRAFT_VISUALCRAFT_BLOCKBROWSER_H
#define SLOPECRAFT_VISUALCRAFT_BLOCKBROWSER_H

#include <QWidget>

class BlockBrowser;

namespace Ui {
class BlockBrowser;
}

class BlockBrowser : public QWidget {
  Q_OBJECT
private:
  Ui::BlockBrowser *ui;

public:
  explicit BlockBrowser(QWidget *parent);
  ~BlockBrowser();
};

#endif // SLOPECRAFT_VISUALCRAFT_BLOCKBROWSER_H