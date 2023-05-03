#include "AdaptiveListView.h"

inline QSize compute_size(QSize widget_size, double ratio = 0.8) noexcept {
  const int new_w = widget_size.width() * ratio;
  return {new_w, new_w};
}

AdaptiveListView::AdaptiveListView(QWidget* parent) : QListView(parent) {
  this->setIconSize(compute_size(this->size()));
  this->setDragEnabled(true);
  this->setAcceptDrops(true);
  this->setDropIndicatorShown(true);
}

AdaptiveListView::~AdaptiveListView() {}

void AdaptiveListView::resizeEvent(QResizeEvent* event) {
  this->setIconSize(compute_size(event->size()));
  QListView::resizeEvent(event);
}