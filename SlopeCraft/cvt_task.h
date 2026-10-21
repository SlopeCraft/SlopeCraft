#ifndef SLOPECRAFT_SLOPECRAFT_CVT_TASK_H
#define SLOPECRAFT_SLOPECRAFT_CVT_TASK_H

#include <functional>
#include <QString>
#include <QImage>
#include <QVariant>
#include <expected>

struct convert_input {
  convert_input() = default;
  convert_input(convert_input&&) = default;
  convert_input(const SlopeCraft::color_table* t,
                const SlopeCraft::convert_option& o)
      : table{t}, option{o} {}

  const SlopeCraft::color_table* table{nullptr};
  SlopeCraft::convert_option option;
};

struct hasher {
  static uint64_t operator()(const SlopeCraft::convert_option& opt) noexcept {
    uint64_t h = 0;
    h |= static_cast<uint64_t>(opt.algo);
    h <<= 1;
    h |= static_cast<uint64_t>(opt.dither);
    return h;
  }
  static uint64_t operator()(const convert_input& pair) noexcept {
    return hasher{}(pair.option) ^ std::hash<const void*>{}(pair.table);
  }
  static uint64_t operator()(const SlopeCraft::build_options& opt) noexcept {
    uint64_t h = 0;
    h ^= static_cast<uint64_t>(opt.max_allowed_height);
    h <<= sizeof(opt.max_allowed_height) * 8;  // 16 bits
    h ^= static_cast<uint64_t>(opt.bridge_interval);
    h <<= sizeof(opt.bridge_interval) * 8;  // 32 bits
    h ^= static_cast<uint64_t>(opt.compress_method);
    h <<= sizeof(opt.compress_method) * 8;  // 40 bits
    h ^= static_cast<uint64_t>(opt.glass_method);
    h <<= sizeof(opt.glass_method) * 8;  // 48 bits
    h ^= static_cast<uint64_t>(opt.fire_proof);
    h <<= 1;  // 49 bits
    h ^= static_cast<uint64_t>(opt.enderman_proof);
    h <<= 1;  // 50 bits
    h ^= static_cast<uint64_t>(opt.connect_mushrooms);
    h <<= 2;  // 52 bits
    h ^= static_cast<uint64_t>(opt.support_block_settings);

    return h;
  }
};

struct equal {
  static bool operator()(const SlopeCraft::convert_option& a,
                         const SlopeCraft::convert_option& b) noexcept {
    if (a.algo != b.algo) return false;
    if (a.dither != b.dither) return false;
    return true;
  }
  static bool operator()(const convert_input& a,
                         const convert_input& b) noexcept {
    if (a.table != b.table) return false;
    if (!equal{}(a.option, b.option)) return false;
    return true;
  }
  static bool operator()(const SlopeCraft::build_options& a,
                         const SlopeCraft::build_options& b) noexcept {
    if (a.max_allowed_height != b.max_allowed_height) return false;
    if (a.bridge_interval != b.bridge_interval) return false;
    if (a.compress_method != b.compress_method) return false;
    if (a.glass_method != b.glass_method) return false;
    if (a.fire_proof != b.fire_proof) return false;
    if (a.enderman_proof != b.enderman_proof) return false;
    if (a.support_block_settings != b.support_block_settings) return false;
    if (a.connect_mushrooms != b.connect_mushrooms) return false;
    if (a.support_block_settings != b.support_block_settings) return false;
    return true;
  }
};

struct structure_with_info {
  std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter> handle;

  const std::array<size_t, 3> shape;
  const uint64_t block_count;

  inline explicit structure_with_info(
      std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter>&& src)
      : handle{std::move(src)},
        shape{{handle->shape_x(), handle->shape_y(), handle->shape_z()}},
        block_count{handle->block_count()} {}
};

struct convert_result {
  //  convert_result() = delete;

  std::unique_ptr<SlopeCraft::converted_image, SlopeCraft::deleter>
      converted_image{nullptr};
  std::unordered_map<SlopeCraft::build_options, structure_with_info, hasher,
                     equal>
      built_structures;

  [[nodiscard]] SlopeCraft::structure_3D* get_structure(
      const SlopeCraft::build_options& opt) const noexcept {
    auto it = built_structures.find(opt);
    if (it == built_structures.end()) {
      return nullptr;
    }
    assert(it->second.handle);
    return it->second.handle.get();
  }
  [[nodiscard]] bool is_built_with(
      const SlopeCraft::build_options& opt) const noexcept {
    return get_structure(opt) != nullptr;
  }

  void set_built(SlopeCraft::build_options opt,
                 std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter>
                     structure) noexcept {
    assert(structure);  // mustn't be null
    opt.ui = {};
    opt.main_progressbar = {};
    opt.sub_progressbar = {};
    // this->built_structures.emplace(opt,
    //                                structure_with_info{std::move(structure)});
    auto it = this->built_structures.find(opt);
    if (it == built_structures.end()) {
      this->built_structures.emplace(opt,
                                     structure_with_info{std::move(structure)});
      return;
    }
    it->second.handle = std::move(structure);
  }
};

struct cvt_task {
  cvt_task() = default;
  cvt_task(cvt_task&&) = default;

  QString filename{""};
  QImage original_image;

  std::unordered_map<convert_input, convert_result, hasher, equal>
      converted_images;

  cvt_task& operator=(cvt_task&&) = default;

  [[nodiscard]] bool is_converted_with(
      const SlopeCraft::color_table* table,
      const SlopeCraft::convert_option& option) const noexcept {
    return this->converted_images.contains(convert_input{table, option});
  }

  [[nodiscard]] std::unordered_map<convert_input, convert_result, hasher,
                                   equal>::iterator
  get_convert_result(const SlopeCraft::color_table* table,
                     const SlopeCraft::convert_option& option) noexcept {
    return this->converted_images.find({table, option});
  }

  [[nodiscard]] inline std::unordered_map<convert_input, convert_result, hasher,
                                          equal>::const_iterator
  get_convert_result(const SlopeCraft::color_table* table,
                     const SlopeCraft::convert_option& option) const noexcept {
    return this->converted_images.find({table, option});
  }

  [[nodiscard]] inline const SlopeCraft::converted_image* get_converted_image(
      const SlopeCraft::color_table* table,
      const SlopeCraft::convert_option& option) const noexcept {
    auto it = this->get_convert_result(table, option);
    if (it == this->converted_images.cend()) {
      return nullptr;
    }
    return it->second.converted_image.get();
  }

  inline void set_converted(
      const SlopeCraft::color_table* table, SlopeCraft::convert_option option,
      std::unique_ptr<SlopeCraft::converted_image, SlopeCraft::deleter>
          converted_image) noexcept {
    if (converted_image == nullptr) {
      return;
    }
    option.ui = {};
    option.progress = {};
    convert_input cvt_input = convert_input{table, option};
    auto it = this->converted_images.find(cvt_input);
    if (it == this->converted_images.end()) {
      this->converted_images.emplace(
          std::move(cvt_input),
          convert_result{.converted_image = std::move(converted_image),
                         .built_structures = {}});
      return;
    }
    it->second.converted_image = std::move(converted_image);
  }

  [[nodiscard]] inline bool is_built_with(
      const SlopeCraft::color_table* table,
      const SlopeCraft::convert_option& cvt_option,
      const SlopeCraft::build_options& build_option) const noexcept {
    auto it = this->converted_images.find(convert_input{table, cvt_option});
    if (it == this->converted_images.end()) {
      return false;
    }
    return it->second.is_built_with(build_option);
  }

  static std::expected<cvt_task, QString> load(QString filename) noexcept;
};

struct map_range {
  int first{-1};
  int last{-1};
};

class task_pool : public std::vector<cvt_task> {
 public:
  [[nodiscard]] size_t converted_count(
      const SlopeCraft::color_table* table,
      const SlopeCraft::convert_option& cvt_option) const noexcept;

  [[nodiscard]] const std::vector<std::pair<size_t, cvt_task*>> converted_tasks(
      const SlopeCraft::color_table* table,
      const SlopeCraft::convert_option& cvt_option) noexcept;

  [[nodiscard]] std::optional<std::pair<size_t, cvt_task*>>
  converted_task_at_index(const SlopeCraft::color_table* table,
                          const SlopeCraft::convert_option& cvt_option,
                          size_t idx) noexcept;

  [[nodiscard]] std::optional<size_t> export_index_to_global_index(
      const SlopeCraft::color_table* table,
      const SlopeCraft::convert_option& cvt_option,
      size_t e_idx) const noexcept;

  [[nodiscard]] map_range map_range_of(int map_begin_index,
                                       size_t global_index) const noexcept;
};

// using task_pool_t = ;
//
// int converted_task_count(const task_pool_t&) noexcept;
//
// int map_export_idx_to_full_idx(const task_pool_t&, int eidx) noexcept;
//
// std::vector<int> iteration_map(const task_pool_t& pool) noexcept;

#endif  // SLOPECRAFT_SLOPECRAFT_CVT_TASK_H