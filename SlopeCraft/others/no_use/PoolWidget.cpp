#include "PoolWidget.h"
#include "ui_PoolWidget.h"

PoolWidget::PoolWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::PoolWidget) {
  this->ui->setupUi(this);
}

PoolWidget::~PoolWidget() { delete this->ui; }

void PoolWidget::set_task(const cvt_task& task) noexcept {
  this->ui->lb_image->setPixmap(QPixmap::fromImage(task.original_image));
  this->ui->lb_name->setText(task.filename);
}