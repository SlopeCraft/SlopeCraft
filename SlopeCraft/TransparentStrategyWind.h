#ifndef SLOPECRAFT_SLOPECRAFT_TRANSPARENCYSTRATEGYWIND_H
#define SLOPECRAFT_SLOPECRAFT_TRANSPARENCYSTRATEGYWIND_H
#include <QDialog>
#include <SlopeCraftL.h>
#include <optional>

class TransparentStrategyWind;
namespace Ui {
class TransparentStrategyWind;
}

class TransparentStrategyWind : public QDialog {
  Q_OBJECT
 private:
  Ui::TransparentStrategyWind *const ui;

 public:
  explicit TransparentStrategyWind(QWidget *parent = nullptr);
  ~TransparentStrategyWind();

  struct strategy {
    SCL_PureTpPixelSt pure_transparent;
    SCL_HalfTpPixelSt half_transparent;
    uint32_t background_color;
  };

  SCL_PureTpPixelSt pure_strategy() const noexcept;
  SCL_HalfTpPixelSt half_strategy() const noexcept;
  uint32_t background_color() const noexcept;

  strategy current_strategy() const noexcept;

  static std::optional<strategy> ask_for_strategy(QWidget *parent) noexcept;
};
#endif  // SLOPECRAFT_SLOPECRAFT_TRANSPARENCYSTRATEGYWIND_H