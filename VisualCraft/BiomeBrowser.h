#include <QWidget>
#include <VisualCraftL.h>

class BiomeBrowser;

namespace Ui {
class BiomeBrowser;
}

class BiomeBrowser : public QWidget {
  Q_OBJECT
private:
  Ui::BiomeBrowser *ui;

public:
  BiomeBrowser(QWidget *parent);
  ~BiomeBrowser();

private:
  VCL_biome_t biome_selected() const noexcept;
  bool is_grass_selected() const noexcept;
private slots:
  void when_biome_changed() noexcept;
  void refresh_colormap() noexcept;
};