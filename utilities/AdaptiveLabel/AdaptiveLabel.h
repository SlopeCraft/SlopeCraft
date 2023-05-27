#ifndef SLOPECRAFT_UTILITIES_ADAPTIVELABEL_ADAPTIVELABEL_H
#define SLOPECRAFT_UTILITIES_ADAPTIVELABEL_ADAPTIVELABEL_H

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class AdaptiveLabel : public QLabel {
  // Q_OBJECT
 private:
  QPixmap real_image;

  void refresh_image() noexcept;

 public:
  explicit AdaptiveLabel(QWidget* parent);
  ~AdaptiveLabel() = default;

  void setPixmap(const QPixmap&) noexcept;

  QPixmap pixmap() const noexcept;

  QLabel& base() noexcept { return *this; }
  const QLabel& base() const noexcept { return *this; }

  void resizeEvent(QResizeEvent* event) noexcept override;

  // using QLabel::;
  using QLabel::setAlignment;
  using QLabel::setFrameShape;
  using QLabel::setMinimumSize;
  using QLabel::setObjectName;
  using QLabel::setScaledContents;
  using QLabel::setSizePolicy;
  using QLabel::setStyleSheet;
  using QLabel::setText;
  using QLabel::sizePolicy;
};

#endif  // SLOPECRAFT_UTILITIES_ADAPTIVELABEL_ADAPTIVELABEL_H