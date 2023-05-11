#include "TokiSlopeCraft.h"
#include <fmt/format.h>
#include <fstream>
#include <span>
#include <functional>
#include <filesystem>
#include <md5.h>
#include <utilities/ColorManip/seralize_funs.hpp>

#include <zstr.hpp>

namespace stdfs = std::filesystem;
std::string write_hash(std::string_view filename,
                       const std::vector<uint8_t> &hash) noexcept {
  std::ofstream ofs{filename.data(), std::ios::binary};

  if (!ofs) {
    return fmt::format("ofstream failed to open hash file: {}", filename);
  }

  ofs.write((const char *)hash.data(), sizeof(uint8_t) * hash.size());
  ofs.close();
  return {};
}

void TokiSlopeCraft::setCacheDir(const char *d) noexcept {
  this->cache_dir = d;

  std::filesystem::create_directories(d);
}
const char *TokiSlopeCraft::cacheDir() const noexcept {
  if (this->cache_dir.has_value()) {
    return this->cache_dir.value().c_str();
  }

  return nullptr;
}

const std::string_view hash_suffix{".sha3_512"};

std::string TokiSlopeCraft::colorset_hash_file() const noexcept {
  return fmt::format("{}/{}{}", this->cache_dir.value(), "colorset",
                     hash_suffix);
}

std::string TokiSlopeCraft::task_dir() const noexcept {
  return this->task_dir(this->image_cvter.task_hash());
}

std::string TokiSlopeCraft::task_dir(uint64_t hash) const noexcept {
  return fmt::format("{}/{:x}", this->cache_dir.value(), hash);
}
std::string TokiSlopeCraft::task_dir(SCL_convertAlgo algo,
                                     bool dither) const noexcept {
  return this->task_dir(this->image_cvter.task_hash(algo, dither));
}

std::string TokiSlopeCraft::conevrt_cache_filename(
    std::string_view taskdir) noexcept {
  return fmt::format("{}/convert", taskdir);
}

std::string TokiSlopeCraft::cache_owned_hash_filename(
    std::string_view taskdir) noexcept {
  return fmt::format("{}/private_hash{}", taskdir, hash_suffix);
}

void TokiSlopeCraft::save_convert_cache(std::string &err) const noexcept {
  if (this->kernelStep < SlopeCraft::step::converted) {
    err = "Can not save cache before the image is converted.";
    return;
  }

  if (!this->cache_dir.has_value()) {
    err = "cache dir is not set.";
    return;
  }

  const std::string task_dir = this->task_dir();

  namespace stdfs = std::filesystem;
  std::error_code ec;
  if (!stdfs::is_directory(task_dir)) {
    if (!stdfs::create_directories(task_dir, ec)) {
      err = fmt::format(
          "Failed to create dir named \"{}\", error code = {}, message = {}",
          task_dir, ec.value(), ec.message());
      return;
    }
  }
  err.clear();
  std::string temp_err{};

  // write 2 colorset hash files
  {
    const std::string name_hash_colorset = this->colorset_hash_file();
    const auto hash = TokiSlopeCraft::Allowed.hash();

    temp_err = write_hash(name_hash_colorset, hash);
    if (!temp_err.empty()) {
      err = fmt::format("Failed to save colorset hash. Detail: {}", temp_err);
      return;
    }

    temp_err = write_hash(this->cache_owned_hash_filename(task_dir), hash);
    if (!temp_err.empty()) {
      err = fmt::format("Failed to save colorset hash. Detail: {}", temp_err);
      return;
    }
  }

  const std::string convert_cache_file =
      TokiSlopeCraft::conevrt_cache_filename(task_dir);

  if (!this->image_cvter.save_cache(convert_cache_file.c_str())) {
    err = fmt::format("Failed to save image cvter cache.");
    return;
  }

  return;
}

bool TokiSlopeCraft::check_coloset_hash_by_filename(
    std::string_view filename) const noexcept {
  if (this->kernelStep < SCL_step::wait4Image) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can check for colorset only after you set the map type");
    return false;
  }

  std::vector<uint8_t> buf;
  {
    std::ifstream ifs{filename.data(), std::ios::binary};

    if (!ifs) {  // file not exist
      return false;
    }

    const size_t file_size = std::filesystem::file_size(filename.data());
    if (file_size <= 0) {  // empty file
      return false;
    }

    buf.resize(file_size);

    ifs.read((char *)buf.data(), file_size);
  }

  const auto current_hash = TokiSlopeCraft::Allowed.hash();

  if (current_hash.size() != buf.size()) {  // length mismatch
    return false;
  }

  for (size_t idx = 0; idx < buf.size(); idx++) {
    if (buf[idx] != current_hash[idx]) {  // hash checksum failed.
      return false;
    }
  }

  return true;
}

bool TokiSlopeCraft::checkColorsetHash() const noexcept {
  return this->check_coloset_hash_by_filename(this->colorset_hash_file());
}

bool TokiSlopeCraft::check_cache_owned_hash(uint64_t task_hash) const noexcept {
  return this->check_coloset_hash_by_filename(
      this->cache_owned_hash_filename(this->task_dir(task_hash)));
}

bool TokiSlopeCraft::loadConvertCache(SCL_convertAlgo algo,
                                      bool dither) noexcept {
  if (this->kernelStep < SCL_step::convertionReady) {
    reportError(
        wind, errorFlag::HASTY_MANIPULATION,
        "You can load convert cache only after you set the original image");
    return false;
  }

  if (!this->checkColorsetHash()) {
    return false;
  }

  const uint64_t expected_task_hash = this->image_cvter.task_hash(algo, dither);

  if (!this->check_cache_owned_hash(expected_task_hash)) {
    return false;
  }

  const std::string expected_task_dir = this->task_dir(expected_task_hash);

  const std::string expected_cache_file =
      this->conevrt_cache_filename(expected_task_dir);

  const bool ok = this->image_cvter.load_cache(expected_cache_file.c_str(),
                                               expected_task_hash);

  if (ok) {
    this->mapPic = this->image_cvter.mapcolor_matrix().cast<int>();
    this->kernelStep = SCL_step::converted;
    return true;
  }
  return false;
}

uint64_t TokiSlopeCraft::build_task_hash(const Eigen::ArrayXXi &mapPic,
                                         std::span<std::string_view> blkid,
                                         const build_options &bo) noexcept {
  Chocobo1::MD5 hash;
  {
    const int64_t rows{mapPic.rows()}, cols{mapPic.cols()};
    hash.addData(&rows, sizeof(rows));
    hash.addData(&cols, sizeof(cols));
  }
  hash.addData(mapPic.data(), mapPic.size() * sizeof(mapPic(0)));

  for (auto str : blkid) {
    hash.addData(str.data(), str.size());
  }

  hash.addData(&bo.maxAllowedHeight, sizeof(bo.maxAllowedHeight));
  hash.addData(&bo.bridgeInterval, sizeof(bo.bridgeInterval));
  hash.addData(&bo.compressMethod, sizeof(bo.compressMethod));
  hash.addData(&bo.glassMethod, sizeof(bo.glassMethod));
  hash.addData(&bo.fire_proof, sizeof(bo.fire_proof));
  hash.addData(&bo.enderman_proof, sizeof(bo.enderman_proof));
  hash.addData(&bo.connect_mushrooms, sizeof(bo.connect_mushrooms));

  hash.finalize();

  auto bytes = hash.toArray();

  static_assert(bytes.size() == 16);

  const uint64_t *const hash_data = (const uint64_t *)bytes.data();
  return hash_data[0] ^ hash_data[1];
}

uint64_t TokiSlopeCraft::build_task_hash() const noexcept {
  assert(this->kernelStep >= SCL_step::builded);
  auto sbil = this->schem_block_id_list();
  return build_task_hash(this->mapPic, sbil, this->build_opt);
}

std::string TokiSlopeCraft::build_task_dir(std::string_view cvt_task_dir,
                                           uint64_t build_task_hash) noexcept {
  return fmt::format("{}/{:x}", cvt_task_dir, build_task_hash);
}

std::string TokiSlopeCraft::build_task_dir() const noexcept {
  return build_task_dir(this->task_dir(), this->build_task_hash());
}

std::string TokiSlopeCraft::build_cache_filename(
    std::string_view build_task_dir) noexcept {
  return fmt::format("{}/build.gz", build_task_dir);
}
namespace cereal {
template <class archive>
void save(archive &ar, const SlopeCraft::Kernel::build_options &opt) noexcept {
  ar(opt.maxAllowedHeight, opt.bridgeInterval, opt.compressMethod,
     opt.glassMethod);
  ar(opt.fire_proof, opt.enderman_proof, opt.connect_mushrooms);
}

template <class archive>
void load(archive &ar, SlopeCraft::Kernel::build_options &opt) noexcept {
  ar(opt.maxAllowedHeight, opt.bridgeInterval, opt.compressMethod,
     opt.glassMethod);
  ar(opt.fire_proof, opt.enderman_proof, opt.connect_mushrooms);
}
}  // namespace cereal

std::string TokiSlopeCraft::make_build_cache() const noexcept {
  if (this->kernelStep < SlopeCraft::step::builded) {
    return "Can not save cache before the 3d structure id built.";
  }

  const std::string build_cache_dir = build_task_dir();
  if (!stdfs::is_directory(build_cache_dir)) {
    std::error_code ec;

    if (!stdfs::create_directories(build_cache_dir, ec)) {
      return fmt::format(
          "Failed to create dir named \"{}\", error code = {}, message = {}",
          build_cache_dir, ec.value(), ec.message());
    }
  }
  const std::string build_cache_file = build_cache_filename(build_cache_dir);
  {
    // std::ofstream ofs{build_cache_file, std::ios::binary};
    zstr::ofstream ofs{build_cache_file, std::ios::binary, Z_BEST_COMPRESSION};
    if (!ofs) {
      return fmt::format("ofstream failed to open cache file {}",
                         build_cache_file);
    }

    {
      cereal::BinaryOutputArchive boa{ofs};
      boa(this->mapPic);
      boa(this->build_opt);
      boa(this->schem);
    }
    ofs.close();
  }

  return {};
}

bool TokiSlopeCraft::exmaine_build_cache(std::string_view filename,
                                         uint64_t expected_task_hash,
                                         build_cache_ir *ir) noexcept {
  build_cache_ir temp;
  {
    if (!stdfs::is_regular_file(filename)) {
      return false;
    }
    // std::ifstream ifs{filename.data(), std::ios::binary};
    zstr::ifstream ifs{filename.data(), std::ios::binary};
    cereal::BinaryInputArchive bia{ifs};
    try {
      bia(temp.mapPic);
      bia(temp.build_option);
      bia(temp.schem);
    } catch (std::exception &) {
      return false;
    }
  }
  {
    std::vector<std::string_view> blkids;
    blkids.reserve(temp.schem.palette().size());
    for (const auto &bid : temp.schem.palette()) {
      blkids.emplace_back(bid);
    }

    const uint64_t hash =
        TokiSlopeCraft::build_task_hash(temp.mapPic, blkids, temp.build_option);

    if (hash != expected_task_hash) {
      return false;
    }
  }

  if (ir != nullptr) {
    *ir = std::move(temp);
  }
  return true;
}

bool TokiSlopeCraft::loadBuildCache(const build_options &option) noexcept {
  if (this->kernelStep < SCL_step::converted) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can load build cache only after you convert a image");
    return false;
  }
  auto sbil = this->schem_block_id_list();
  const uint64_t expected_hash = build_task_hash(this->mapPic, sbil, option);
  const std::string cache_filename = this->build_cache_filename(
      this->build_task_dir(this->task_dir(), expected_hash));

  build_cache_ir ir;
  if (!this->exmaine_build_cache(cache_filename, expected_hash, &ir)) {
    return false;
  }

  this->mapPic = std::move(ir.mapPic);
  this->build_opt = option;
  this->schem = std::move(ir.schem);

  this->kernelStep = SCL_step::builded;
  return true;
}