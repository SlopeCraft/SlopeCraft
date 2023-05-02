#include "cvt_task.h"
#include <QObject>

cvt_task cvt_task::load(QString filename, QString& err) noexcept {
  err.clear();
  cvt_task ret;

  ret.filename = filename;

  QImage temp;
  if (!temp.load(filename)) {
    err = QObject::tr("加载图片 %1 失败。").arg(filename);
    return {};
  }

  ret.original_image = temp.convertToFormat(QImage::Format_ARGB32);
  return ret;
}