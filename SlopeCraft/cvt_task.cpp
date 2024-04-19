#include "cvt_task.h"
#include <QObject>

tl::expected<cvt_task, QString> cvt_task::load(QString filename) noexcept {
  cvt_task ret;

  ret.filename = filename;

  QImage temp;
  if (!temp.load(filename)) {
    return tl::make_unexpected(QObject::tr("加载图片 %1 失败。").arg(filename));
  }

  ret.original_image = temp.convertToFormat(QImage::Format_ARGB32);
  return ret;
}

int converted_task_count(const task_pool_t& pool) noexcept {
  int num = 0;
  for (const auto& i : pool) {
    if (i.is_converted()) {
      num++;
    }
  }
  return num;
}

int map_export_idx_to_full_idx(const task_pool_t& pool, int eidx) noexcept {
  assert(eidx >= 0);
  for (int fidx = 0; fidx < (int)pool.size(); fidx++) {
    if (pool.at(fidx).is_converted()) {
      eidx--;
    }
    if (eidx < 0) {
      return fidx;
    }
  }

  assert(false);
  return INT_MAX;
}

std::vector<int> iteration_map(const task_pool_t& pool) noexcept {
  std::vector<int> ret;
  ret.reserve(pool.size());
  for (int i = 0; i < (int)pool.size(); i++) {
    if (pool.at(i).is_converted()) {
      ret.emplace_back(i);
    }
  }
  return ret;
}