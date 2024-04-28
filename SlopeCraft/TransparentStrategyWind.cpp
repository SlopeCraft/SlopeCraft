#include "TransparentStrategyWind.h"
#include "ui_TransparentStrategyWind.h"
#include <QColorDialog>

TransparentStrategyWind::TransparentStrategyWind(QWidget* parent)
    : QDialog{parent}, ui{new Ui::TransparentStrategyWind} {
  this->ui->setupUi(this);

  this->on_pb_reset_clicked();
}

TransparentStrategyWind::~TransparentStrategyWind() {}

SCL_PureTpPixelSt TransparentStrategyWind::pure_strategy() const noexcept {
  if (this->ui->cb_pure_air->isChecked()) {
    return SCL_PureTpPixelSt::ReserveAsAir;
  }
  return SCL_PureTpPixelSt::ReplaceWithBackGround;
}

SCL_HalfTpPixelSt TransparentStrategyWind::half_strategy() const noexcept {
  if (this->ui->cb_half_background->isChecked()) {
    return SCL_HalfTpPixelSt::ReplaceWithBackGround;
  }
  if (this->ui->cb_half_compose->isChecked()) {
    return SCL_HalfTpPixelSt::ComposeWithBackGround;
  }
  return SCL_HalfTpPixelSt::IgnoreAlpha;
}

uint32_t TransparentStrategyWind::background_color() const noexcept {
  if (this->ui->cb_background_white->isChecked()) {
    return 0xFFFFFFFF;
  }
  if (this->ui->cb_background_gray->isChecked()) {
    return 0xFFDCDCDC;
  }
  auto pl = this->ui->lb_show_custom_color->palette();
  auto color = pl.color(QPalette::ColorRole::Window).toRgb();

  return qRgb(color.red(), color.green(), color.blue());
}

TransparentStrategyWind::strategy TransparentStrategyWind::current_strategy()
    const noexcept {
  return {this->pure_strategy(), this->half_strategy(),
          this->background_color()};
}

void TransparentStrategyWind::on_pb_confirm_clicked() noexcept {
  emit this->accept();
}

void TransparentStrategyWind::on_pb_cancel_clicked() noexcept {
  emit this->reject();
}

void TransparentStrategyWind::on_cb_background_custom_clicked() noexcept {
  auto color =
      QColorDialog::getColor(Qt::GlobalColor::white, this, tr("设置背景色"));
  if (!color.isValid()) {
    this->ui->cb_background_gray->setChecked(true);
  }

  QPalette pl;
  pl.setColor(QPalette::ColorRole::Window, color);

  this->ui->lb_show_custom_color->setPalette(pl);
}

void TransparentStrategyWind::on_pb_reset_clicked() noexcept {
  this->ui->cb_pure_background->setChecked(true);
  this->ui->cb_half_compose->setChecked(true);
  this->ui->cb_background_gray->setChecked(true);
  QPalette pl;
  pl.setColor(QPalette::ColorRole::Window, QColor(Qt::GlobalColor::white));

  this->ui->lb_show_custom_color->setPalette(pl);
}

std::optional<TransparentStrategyWind::strategy>
TransparentStrategyWind::ask_for_strategy(QWidget* parent) noexcept {
  TransparentStrategyWind tpswind{parent};

  const auto dialog_code = tpswind.exec();

  if (dialog_code == QDialog::DialogCode::Rejected) {
    return std::nullopt;
  }

  const auto ret = tpswind.current_strategy();
  return ret;
}