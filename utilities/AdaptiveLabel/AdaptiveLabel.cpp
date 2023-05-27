#include "AdaptiveLabel.h"
#include <QWidget>
#include <QImage>
#include <cmath>

AdaptiveLabel::AdaptiveLabel(QWidget* parent) : QLabel(parent) {}

// AdaptiveLabel::~AdaptiveLabel() {}

QPixmap AdaptiveLabel::pixmap() const noexcept { return this->real_image; }

void AdaptiveLabel::setPixmap(const QPixmap& img) noexcept {
  this->real_image = img;

  this->refresh_image();
}

void AdaptiveLabel::refresh_image() noexcept {
  const auto raw_image_size = this->real_image.size();

  if (raw_image_size.height() <= 0 || raw_image_size.width() <= 0) {
    static_cast<QLabel*>(this)->setPixmap(QPixmap{});
    return;
  }
  auto label_size = this->size();

  label_size.setHeight(std::max(label_size.height() - 2, 1));
  label_size.setWidth(std::max(label_size.width() - 2, 1));

  QPixmap scaled =
      this->real_image.scaled(label_size, Qt::AspectRatioMode::KeepAspectRatio,
                              Qt::TransformationMode::SmoothTransformation);

  static_cast<QLabel*>(this)->setPixmap(scaled);
}

void AdaptiveLabel::resizeEvent(QResizeEvent* event) noexcept {
  QLabel::resizeEvent(event);

  this->refresh_image();
}