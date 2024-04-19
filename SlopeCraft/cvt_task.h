#ifndef SLOPECRAFT_SLOPECRAFT_CVT_TASK_H
#define SLOPECRAFT_SLOPECRAFT_CVT_TASK_H

#include <QString>
#include <QImage>
#include <QVariant>
#include <tl/expected.hpp>

struct cvt_task {
  QString filename{""};
  QImage original_image;

  std::unique_ptr<SlopeCraft::converted_image, SlopeCraft::deleter>
      converted_img;
  std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter> structure;

  [[nodiscard]] bool is_converted() const noexcept {
    return this->converted_img != nullptr;
  }

  [[nodiscard]] bool is_built() const noexcept {
    return this->is_converted() && (this->structure != nullptr);
  }

  static tl::expected<cvt_task, QString> load(QString filename) noexcept;
};

Q_DECLARE_METATYPE(cvt_task)

using task_pool_t = std::vector<cvt_task>;

int converted_task_count(const task_pool_t&) noexcept;

int map_export_idx_to_full_idx(const task_pool_t&, int eidx) noexcept;

std::vector<int> iteration_map(const task_pool_t& pool) noexcept;

#endif  // SLOPECRAFT_SLOPECRAFT_CVT_TASK_H