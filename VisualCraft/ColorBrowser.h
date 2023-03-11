#ifndef SLOPECRAFT_VISUALCRAFT_COLORBROWSER_H
#define SLOPECRAFT_VISUALCRAFT_COLORBROWSER_H
#include <QWidget>

class ColorBrowser;

namespace Ui {
class ColorBrowser;
}

// class private_class_setup_chart;

class ColorBrowser : public QWidget {
  Q_OBJECT
private:
  Ui::ColorBrowser *ui;
  // private_class_setup_chart *thread{nullptr};

  void setup_table(const uint16_t *const color_id_list,
                   const size_t color_count) noexcept;

public:
  explicit ColorBrowser(QWidget *parent);
  ~ColorBrowser();

  void setup_table_basic() noexcept;
  void setup_table_allowed() noexcept;

  // void setup_table_threaded() noexcept;
};

#endif // SLOPECRAFT_VISUALCRAFT_COLORBROWSER_H