#ifndef SLOPECRAFT_VISUALCRAFT_BLOCKBROWSER_H
#define SLOPECRAFT_VISUALCRAFT_BLOCKBROWSER_H

#include <QWidget>
#include <VisualCraftL.h>
#include <vector>

class BlockBrowser;

namespace Ui {
class BlockBrowser;
}

class VCWind;

class BlockBrowser : public QWidget {
  Q_OBJECT
private:
  Ui::BlockBrowser *ui;

  void fecth_content() noexcept;

private slots:
  // manuually connected
  void update_display() noexcept;

  // auto connected
  void on_pb_save_current_image_clicked() noexcept;
  void on_combobox_select_blk_all_currentIndexChanged(int idx) noexcept;

public:
  explicit BlockBrowser(QWidget *parent);
  ~BlockBrowser();

  VCWind *parent() noexcept;
  const VCWind *parent() const noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFT_BLOCKBROWSER_H