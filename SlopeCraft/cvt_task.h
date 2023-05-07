#ifndef SLOPECRAFT_SLOPECRAFT_CVT_TASK_H
#define SLOPECRAFT_SLOPECRAFT_CVT_TASK_H

#include <QString>
#include <QImage>
#include <QVariant>

struct cvt_task {
  QString filename{""};
  QImage original_image;

  bool is_converted{false};
  bool is_built{false};
  bool is_exported{false};

  static cvt_task load(QString filename, QString& err) noexcept;

  void set_converted() noexcept {
    this->is_converted = true;
    this->is_built = false;
    this->is_exported = false;
  }

  void set_built() noexcept {
    assert(this->is_converted);
    this->is_built = true;
    this->is_exported = false;
  }

  void set_exported() noexcept {
    assert(this->is_converted);
    assert(this->is_built);
    this->is_exported = true;
  }

  void set_unconverted() noexcept {
    this->is_converted = false;
    this->is_built = false;
    this->is_exported = false;
  }
};

Q_DECLARE_METATYPE(cvt_task);

using task_pool_t = std::vector<cvt_task>;

#endif  // SLOPECRAFT_SLOPECRAFT_CVT_TASK_H