
#include <QObject>
#include "cvt_task.h"
#include "ExportTableModel.h"

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

size_t task_pool::converted_count(
    const SlopeCraft::color_table* table,
    const SlopeCraft::convert_option& cvt_option) const noexcept {
  size_t counter = 0;
  for (const auto& t : *this) {
    if (t.is_converted_with(table, cvt_option)) {
      counter += 1;
    }
  }
  return counter;
}

const std::vector<std::pair<size_t, cvt_task*>> task_pool::converted_tasks(
    const SlopeCraft::color_table* table,
    const SlopeCraft::convert_option& cvt_option) noexcept {
  std::vector<std::pair<size_t, cvt_task*>> ret;
  ret.reserve(this->size());
  for (size_t idx = 0; idx < this->size(); idx++) {
    auto& task = this->at(idx);
    if (task.is_converted_with(table, cvt_option)) {
      ret.emplace_back(std::pair<size_t, cvt_task*>{idx, &task});
    }
  }
  return ret;
}
std::optional<std::pair<size_t, cvt_task*>> task_pool::converted_task_at_index(
    const SlopeCraft::color_table* table,
    const SlopeCraft::convert_option& cvt_option, size_t eidx) noexcept {
  for (size_t idx = 0; idx < this->size(); idx++) {
    auto& task = this->at(idx);
    if (task.is_converted_with(table, cvt_option)) {
      if (eidx == 0) {
        return std::pair<size_t, cvt_task*>{idx, &task};
      }
      eidx--;
    }
  }
  return std::nullopt;
}

std::optional<size_t> task_pool::export_index_to_global_index(
    const SlopeCraft::color_table* table,
    const SlopeCraft::convert_option& cvt_option, size_t e_idx) const noexcept {
  for (size_t gidx = 0; gidx < this->size(); gidx++) {
    auto& task = this->at(gidx);
    if (!task.is_converted_with(table, cvt_option)) {
      continue;
    }

    if (e_idx <= 0) {
      return gidx;
    }
    e_idx--;
  }
  return std::nullopt;
}

map_range task_pool::map_range_of(int map_begin_index,
                                  size_t global_index) const noexcept {
  assert(global_index < this->size());
  for (size_t i = 0; i < global_index; i++) {
    const auto& task = this->at(i);
    const auto current_map_size =
        map_size_of_images(task.original_image.size());
    const int current_map_num =
        current_map_size.height() * current_map_size.width();
    map_begin_index += current_map_num;
  }
  const auto cms =
      map_size_of_images(this->at(global_index).original_image.size());
  const int cmn = cms.height() * cms.width();
  return map_range{map_begin_index, map_begin_index + cmn - 1};
}

// int converted_task_count(const task_pool_t& pool) noexcept {
//   int num = 0;
//   for (const auto& i : pool) {
//     if (i.is_converted()) {
//       num++;
//     }
//   }
//   return num;
// }
//
// int map_export_idx_to_full_idx(const task_pool_t& pool, int eidx) noexcept {
//   assert(eidx >= 0);
//   for (int fidx = 0; fidx < (int)pool.size(); fidx++) {
//     if (pool.at(fidx).is_converted()) {
//       eidx--;
//     }
//     if (eidx < 0) {
//       return fidx;
//     }
//   }
//
//   assert(false);
//   return INT_MAX;
// }

// std::vector<int> iteration_map(const task_pool_t& pool) noexcept {
//   std::vector<int> ret;
//   ret.reserve(pool.size());
//   for (int i = 0; i < (int)pool.size(); i++) {
//     if (pool.at(i).is_converted()) {
//       ret.emplace_back(i);
//     }
//   }
//   return ret;
// }