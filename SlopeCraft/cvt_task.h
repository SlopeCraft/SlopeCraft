#ifndef SLOPECRAFT_SLOPECRAFT_CVT_TASK_H
#define SLOPECRAFT_SLOPECRAFT_CVT_TASK_H

#include <functional>
#include <QString>
#include <QImage>
#include <QVariant>
#include <tl/expected.hpp>

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
  uint64_t operator()(
      const SlopeCraft::GA_converter_option& opt) const noexcept {
    uint64_t h = 0;
    h ^= std::hash<size_t>()(opt.popSize);
    h ^= std::hash<size_t>()(opt.maxGeneration);
    h ^= std::hash<size_t>()(opt.maxFailTimes);
    h ^= std::hash<double>()(opt.crossoverProb);
    h ^= std::hash<double>()(opt.mutationProb);
    return h;
  }
  uint64_t operator()(const SlopeCraft::convert_option& opt) const noexcept {
    uint64_t h = 0;
    h |= static_cast<uint64_t>(opt.algo);
    h <<= 1;
    h |= static_cast<uint64_t>(opt.dither);
    h ^= hasher{}(opt.ai_cvter_opt);
    return h;
  }
  uint64_t operator()(const convert_input& pair) const noexcept {
    return hasher{}(pair.option) ^ std::hash<const void*>{}(pair.table);
  }
  uint64_t operator()(const SlopeCraft::build_options& opt) const noexcept {
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
    h <<= 1;  // 51 bits

    return h;
  }
};

struct equal {
  bool operator()(const SlopeCraft::GA_converter_option& a,
                  const SlopeCraft::GA_converter_option& b) const noexcept {
    if (a.popSize != b.popSize) return false;
    if (a.maxGeneration != b.maxGeneration) return false;
    if (a.maxFailTimes != b.maxFailTimes) return false;
    if (a.crossoverProb != b.crossoverProb) return false;
    if (a.mutationProb != b.mutationProb) return false;
    return true;
  }
  bool operator()(const SlopeCraft::convert_option& a,
                  const SlopeCraft::convert_option& b) const noexcept {
    if (a.algo != b.algo) return false;
    if (a.dither != b.dither) return false;
    if (!equal{}(a.ai_cvter_opt, b.ai_cvter_opt)) return false;
    return true;
  }
  bool operator()(const convert_input& a,
                  const convert_input& b) const noexcept {
    if (a.table != b.table) return false;
    if (!equal{}(a.option, b.option)) return false;
    return true;
  }
  bool operator()(const SlopeCraft::build_options& a,
                  const SlopeCraft::build_options& b) const noexcept {
    if (a.max_allowed_height != b.max_allowed_height) return false;
    if (a.bridge_interval != b.bridge_interval) return false;
    if (a.compress_method != b.compress_method) return false;
    if (a.glass_method != b.glass_method) return false;
    if (a.fire_proof != b.fire_proof) return false;
    if (a.enderman_proof != b.enderman_proof) return false;
    return true;
  }
};

struct structure_with_info {
  std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter> handle;

  const std::array<size_t, 3> shape;
  const uint64_t block_count;

  explicit structure_with_info(
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

  [[nodiscard]] bool is_built_with(
      const SlopeCraft::build_options& opt) const noexcept {
    return this->built_structures.contains(opt);
  }

  struct cache_report {
    size_t cache_num{0};
  };

  cache_report cache_all_structures(const SlopeCraft::color_table& table,
                                    const SlopeCraft::converted_image& cvted,
                                    const QString& cache_root_dir) noexcept {
    return this->cache_all_structures(table, cvted, cache_root_dir,
                                      [](auto) { return true; });
  }

  cache_report cache_all_structures(
      const SlopeCraft::color_table& table,
      const SlopeCraft::converted_image& cvted, const QString& cache_root_dir,
      const std::function<bool(const SlopeCraft::build_options& opt)>&
          cache_or_not) noexcept {
    size_t num = 0;
    for (auto& pair : this->built_structures) {
      if (pair.second.handle.get() == nullptr) {  // already cached
        continue;
      }
      if (!cache_or_not(pair.first)) {
        continue;
      }
      const bool ok =
          table.save_build_cache(cvted, pair.first, *pair.second.handle,
                                 cache_root_dir.toLocal8Bit().data(), nullptr);
      pair.second.handle.reset();
      if (ok) {
        num++;
      }
    }
    return cache_report{.cache_num = num};
  }

  void cache_structure(const SlopeCraft::color_table& table,
                       const SlopeCraft::build_options& opt,
                       const QString& cache_root_dir) noexcept {
    auto it = this->built_structures.find(opt);
    if (it == this->built_structures.end()) {
      return;
    }
    if (it->second.handle == nullptr) {
      return;
    }
    const bool ok =
        table.save_build_cache(*this->converted_image, opt, *it->second.handle,
                               cache_root_dir.toLocal8Bit().data(), nullptr);
    if (ok) {
      it->second.handle.reset();
    }
    return;
  }

  const structure_with_info* get_build_cache_with_info_noload(
      const SlopeCraft::color_table& table,
      const SlopeCraft::build_options& opt,
      const QString& cache_root_dir) const noexcept {
    auto it = this->built_structures.find(opt);
    if (it == this->built_structures.end()) {
      return nullptr;
    }
    return &it->second;
  }

  const SlopeCraft::structure_3D* load_build_cache(
      const SlopeCraft::color_table& table,
      const SlopeCraft::build_options& opt,
      const QString& cache_root_dir) noexcept {
    auto it = this->built_structures.find(opt);
    if (it == this->built_structures.end()) {
      return nullptr;
    }

    if (it->second.handle != nullptr) {  // the structure exist in memory
      return it->second.handle.get();
    }

    // the structure is cached
    SlopeCraft::structure_3D* p =
        table.load_build_cache(*this->converted_image, opt,
                               cache_root_dir.toLocal8Bit().data(), nullptr);
    if (p == nullptr) {  // failed to load cache
      return nullptr;
    }
    it->second.handle.reset(p);
    return it->second.handle.get();
  }

  void set_built(SlopeCraft::build_options opt,
                 std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter>
                     structure) noexcept {
    opt.ui = {};
    opt.main_progressbar = {};
    opt.sub_progressbar = {};
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

  //    std::unique_ptr<SlopeCraft::structure_3D, SlopeCraft::deleter>
  //    structure;

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

  [[nodiscard]] std::unordered_map<convert_input, convert_result, hasher,
                                   equal>::const_iterator
  get_convert_result(const SlopeCraft::color_table* table,
                     const SlopeCraft::convert_option& option) const noexcept {
    return this->converted_images.find({table, option});
  }

  [[nodiscard]] const SlopeCraft::converted_image* get_converted_image(
      const SlopeCraft::color_table* table,
      const SlopeCraft::convert_option& option) const noexcept {
    auto it = this->get_convert_result(table, option);
    if (it == this->converted_images.cend()) {
      return nullptr;
    }
    return it->second.converted_image.get();
  }

  void set_converted(
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

  [[nodiscard]] bool is_built_with(
      const SlopeCraft::color_table* table,
      const SlopeCraft::convert_option& cvt_option,
      const SlopeCraft::build_options& build_option) const noexcept {
    auto it = this->converted_images.find(convert_input{table, cvt_option});
    if (it == this->converted_images.end()) {
      return false;
    }
    return it->second.is_built_with(build_option);
  }

  //  [[nodiscard]] const SlopeCraft::structure_3D* load_build_cache(
  //      const SlopeCraft::color_table* table,
  //      const SlopeCraft::convert_option& cvt_option,
  //      const SlopeCraft::build_options& build_option,
  //      const QString& cache_dir) noexcept {
  //
  //  }

  void cache_all_structures(const SlopeCraft::color_table& table,
                            const QString& cache_root_dir) noexcept {
    this->cache_all_structures(table, cache_root_dir,
                               [](auto, auto) { return true; });
  }

  void cache_all_structures(
      const SlopeCraft::color_table& table, const QString& cache_root_dir,
      const std::function<bool(const SlopeCraft::convert_option& cvtopt,
                               const SlopeCraft::build_options& build_opt)>&
          cache_or_not) noexcept {
    for (auto& cvt : this->converted_images) {
      const auto& cvted_img = cvt.second.converted_image;
      cvt.second.cache_all_structures(
          table, *cvted_img, cache_root_dir,
          [cache_or_not, &cvt](const SlopeCraft::build_options& bopt) {
            return cache_or_not(cvt.first.option, bopt);
          });
    }
  }

  static tl::expected<cvt_task, QString> load(QString filename) noexcept;
};

// Q_DECLARE_METATYPE(cvt_task)

struct map_range {
  int first{-1};
  int last{-1};
};

// map_range map_range_at_index(const task_pool_t& pool, int first_map_seq_num,
//                              int asked_idx) noexcept;
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