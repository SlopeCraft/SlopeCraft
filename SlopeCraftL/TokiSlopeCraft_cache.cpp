#include "TokiSlopeCraft.h"
#include <fmt/format.h>
#include <fstream>
#include <span>
#include <functional>
#include <filesystem>
#include <md5.h>
#include <sha3.h>
#include <utilities/ColorManip/seralize_funs.hpp>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/filter/zstd.hpp>
#include <zstd.h>

namespace stdfs = std::filesystem;
// common utilities ------------------------------------
std::string write_hash(std::string_view filename,
                       std::span<const uint8_t> hash) noexcept {
  std::ofstream ofs{filename.data(), std::ios::binary};

  if (!ofs) {
    return fmt::format("ofstream failed to open hash file: {}", filename);
  }

  ofs.write((const char *)hash.data(), sizeof(uint8_t) * hash.size());
  ofs.close();
  return {};
}

bool examine_hash_file(std::string_view filename,
                       std::span<const uint8_t> expected) noexcept {
  if (!stdfs::is_regular_file(filename)) {
    return false;
  }
  const uint64_t filesize = stdfs::file_size(filename);
  if (filesize != expected.size()) {
    return false;
  }
  {
    std::ifstream ifs{filename.data(), std::ios::binary};
    if (!ifs) {
      return false;
    }

    for (uint8_t exp_val : expected) {
      uint8_t val{0};
      ifs.read((char *)&val, sizeof(val));
      if (val != exp_val) {
        return false;
      }
    }
    ifs.close();
  }

  return true;
}

bool create_dir_r_if_not_exist(std::string_view dir,
                               std::string &err) noexcept {
  err.clear();
  if (stdfs::is_directory(dir)) {
    return true;
  }
  std::error_code ec;
  if (stdfs::create_directories(dir, ec)) {
    return true;
  }

  err = fmt::format(
      "Cache directory \"{}\" does not exists, and std::filesystem failed to "
      "create this dir. Error code = {}, message = {}.",
      dir, ec.value(), ec.message());
  return false;
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

#define SC_HASH_ADD_DATA(hasher, obj) hasher.addData(&obj, sizeof(obj));

uint64_t to_short_hash(std::span<const uint8_t> full_hash) noexcept {
  assert(full_hash.size() >= 8);
  return *reinterpret_cast<const uint64_t *>(full_hash.data());
  /*
  uint64_t ret = 0;
  std::span<const uint64_t> hash_u64{(const uint64_t *)full_hash.data(),
                                     full_hash.size() / sizeof(uint64_t)};
  for (uint64_t val : hash_u64) {
    ret = ret ^ val;
  }
  return ret;
  */
}
// type hash and dir ------------------------------------
std::vector<uint8_t> TokiSlopeCraft::type_hash() noexcept {
  Chocobo1::SHA3_512 hash;
  SC_HASH_ADD_DATA(hash, TokiSlopeCraft::mapType);
  SC_HASH_ADD_DATA(hash, TokiSlopeCraft::mcVer);

  TokiSlopeCraft::Allowed.hash_add_data(hash);

  hash.finalize();
  return hash.toVector();
}

std::string TokiSlopeCraft::type_dir_of(std::string_view cache_dir) noexcept {
  return fmt::format("{}/{:x}", cache_dir, to_short_hash(type_hash()));
}

std::string TokiSlopeCraft::type_hash_filename() const noexcept {
  assert(this->kernelStep >= SCL_step::wait4Image);
  return fmt::format("{}/{}{}", this->current_type_dir(), "type", hash_suffix);
}

std::string TokiSlopeCraft::save_type_hash() const noexcept {
  if (this->kernelStep < SCL_step::wait4Image) {
    return "Set map type before you export type hash.";
  }
  std::string err;
  if (!create_dir_r_if_not_exist(this->current_type_dir(), err)) {
    return err;
  }

  err = write_hash(this->type_hash_filename(), this->type_hash());
  return err;
}

bool TokiSlopeCraft::check_type_hash() const noexcept {
  return examine_hash_file(this->type_hash_filename(), this->type_hash());
}

// convert hash, dir and cache ------------------------------------
std::string TokiSlopeCraft::convert_dir(std::string_view type_dir,
                                        uint64_t short_hash) noexcept {
  return fmt::format("{}/{:x}", type_dir, short_hash);
}

std::string TokiSlopeCraft::convert_dir_of(SCL_convertAlgo algo,
                                           bool dither) const noexcept {
  assert(this->kernelStep >= SCL_step::convertionReady);
  return convert_dir(this->current_type_dir(),
                     this->image_cvter.task_hash(algo, dither));
}

std::string TokiSlopeCraft::current_convert_dir() const noexcept {
  assert(this->kernelStep >= SCL_step::converted);
  return this->convert_dir_of(this->image_cvter.convert_algo(),
                              this->image_cvter.is_dither());
}

std::string TokiSlopeCraft::convert_cache_filename_of(
    std::string_view convert_dir) noexcept {
  return fmt::format("{}/convert", convert_dir);
}

std::string TokiSlopeCraft::current_convert_cache_filename() const noexcept {
  assert(this->kernelStep >= SCL_step::converted);
  return convert_cache_filename_of(this->current_convert_dir());
}

void TokiSlopeCraft::save_convert_cache(std::string &err) const noexcept {
  err.clear();
  if (this->kernelStep < SlopeCraft::step::converted) {
    err = "Can not save cache before the image is converted.";
    return;
  }
  this->save_type_hash();
  const std::string convert_dir = this->current_convert_dir();
  if (!create_dir_r_if_not_exist(convert_dir, err)) {
    return;
  }

  std::string filename = convert_cache_filename_of(convert_dir);
  if (!this->image_cvter.save_cache(filename.c_str())) {
    err = fmt::format("this->image_cvter.save_cache failed to create {}",
                      filename);
    return;
  }
  return;
}

bool TokiSlopeCraft::load_convert_cache(SCL_convertAlgo algo,
                                        bool dither) noexcept {
  if (this->kernelStep < SlopeCraft::step::convertionReady) {
    // "Can not save cache before the image is converted.";
    return false;
  }

  if (!this->check_type_hash()) {  // type hash mismatch
    return false;
  }

  libMapImageCvt::MapImageCvter temp;
  const uint64_t expected_hash = this->image_cvter.task_hash(algo, dither);
  const std::string filename = this->convert_cache_filename_of(
      convert_dir(this->current_type_dir(), expected_hash));
  if (!this->image_cvter.load_cache(filename.c_str(), expected_hash)) {
    return false;
  }

  this->mapPic = this->image_cvter.mapcolor_matrix().cast<int>();
  this->kernelStep = SCL_step::converted;
  return true;
}

// build hash, dir and cache ------------------------------------
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
std::vector<uint8_t> TokiSlopeCraft::build_task_hash_of(
    const Eigen::ArrayXXi &mapPic, std::span<std::string_view> blkids,
    const build_options &opt) noexcept {
  Chocobo1::SHA3_512 hash;
  {
    const int64_t rows{mapPic.rows()};
    const int64_t cols{mapPic.cols()};
    SC_HASH_ADD_DATA(hash, rows);
    SC_HASH_ADD_DATA(hash, cols);
    hash.addData(mapPic.data(), mapPic.size() * sizeof(uint32_t));
  }

  for (auto blkid : blkids) {
    hash.addData(blkid.data(), blkid.size());
  }
  {
    SC_HASH_ADD_DATA(hash, opt.maxAllowedHeight);
    SC_HASH_ADD_DATA(hash, opt.bridgeInterval);
    SC_HASH_ADD_DATA(hash, opt.compressMethod);
    SC_HASH_ADD_DATA(hash, opt.glassMethod);
    SC_HASH_ADD_DATA(hash, opt.fire_proof);
    SC_HASH_ADD_DATA(hash, opt.enderman_proof);
    SC_HASH_ADD_DATA(hash, opt.connect_mushrooms);
  }

  return hash.finalize().toVector();
}

std::vector<uint8_t> TokiSlopeCraft::current_build_task_hash() const noexcept {
  assert(this->kernelStep >= SCL_step::converted);
  auto abil = this->schem_block_id_list();
  return build_task_hash_of(this->mapPic, abil, this->build_opt);
}

std::string TokiSlopeCraft::build_dir_of(std::string_view convert_dir,
                                         uint64_t short_hash) noexcept {
  return fmt::format("{}/{}", convert_dir, short_hash);
}

std::string TokiSlopeCraft::current_build_dir() const noexcept {
  assert(this->kernelStep >= SCL_step::converted);
  return build_dir_of(this->current_convert_dir(),
                      to_short_hash(this->current_build_task_hash()));
}

std::string TokiSlopeCraft::build_cache_filename_of(
    std::string_view build_dir) noexcept {
  return fmt::format("{}/build.zst", build_dir);
}

std::string TokiSlopeCraft::current_build_cache_filename() const noexcept {
  assert(this->kernelStep >= SCL_step::builded);
  return build_cache_filename_of(this->current_build_dir());
}

std::string TokiSlopeCraft::build_hash_filename_of(
    std::string_view build_dir) noexcept {
  return fmt::format("{}/build_task_hash{}", build_dir, hash_suffix);
}

void TokiSlopeCraft::save_build_cache(std::string &err) const noexcept {
  err.clear();

  if (this->kernelStep < SCL_step::builded) {
    err = "Can not save cache before 3d is built.";
    return;
  }

  const std::string build_dir = this->current_build_dir();
  if (!create_dir_r_if_not_exist(build_dir, err)) {
    return;
  }

  const std::string filename = build_cache_filename_of(build_dir);
  {
    // std::ofstream ofs{build_cache_file, std::ios::binary};
    boost::iostreams::filtering_ostream ofs{};
    ofs.set_auto_close(true);
    {
      boost::iostreams::zstd_params params;
      params.level = uint32_t(ZSTD_defaultCLevel());
      ofs.push(boost::iostreams::zstd_compressor{params});
      ofs.push(boost::iostreams::file_sink{filename, std::ios::binary});
    }

    // zstr::ofstream ofs{filename, std::ios::binary, Z_DEFAULT_COMPRESSION};
    if (!ofs) {
      err = fmt::format("ofstream failed to open cache file {}", filename);
      return;
    }

    {
      cereal::BinaryOutputArchive boa{ofs};
      boa(this->mapPic);
      boa(this->build_opt);
      boa(this->schem);
    }
    // ofs.close();
  }

  auto sha3_512 = this->current_build_task_hash();
  err += write_hash(build_hash_filename_of(build_dir), sha3_512);
  return;
}

bool TokiSlopeCraft::examine_build_cache(
    [[maybe_unused]] const build_options &opt,
    std::span<const uint8_t> expected_sha3_512,
    build_cache_ir *ir) const noexcept {
  assert(this->kernelStep >= SCL_step::converted);

  // this->build_task_hash_of(this->mapPic,this->schem_block_id_list(),opt);

  const std::string build_dir = build_dir_of(this->current_convert_dir(),
                                             to_short_hash(expected_sha3_512));

  if (!examine_hash_file(this->build_hash_filename_of(build_dir),
                         expected_sha3_512)) {
    return false;
  }

  build_cache_ir temp;
  try {
    auto filename = this->build_cache_filename_of(build_dir);
    // zstr::ifstream ifs{filename, std::ios::binary};
    boost::iostreams::filtering_istream ifs;
    ifs.set_auto_close(true);
    {
      boost::iostreams::zstd_params params;
      params.level = uint32_t(ZSTD_defaultCLevel());
      ifs.push(boost::iostreams::zstd_decompressor{});
      ifs.push(boost::iostreams::file_source{filename, std::ios::binary});
    }
    if (!ifs) {
      return false;
    }

    {
      cereal::BinaryInputArchive bia{ifs};
      bia(temp.mapPic);
      bia(temp.build_option);
      bia(temp.schem);
    }
    // ifs.close();
  } catch (...) {
    return false;
  }

  if (ir != nullptr) {
    *ir = std::move(temp);
  }

  return true;
}

bool TokiSlopeCraft::load_build_cache(const build_options &opt) noexcept {
  if (this->kernelStep < SCL_step::converted) {
    return false;
  }

  auto sbil = this->schem_block_id_list();
  auto expected_hash = this->build_task_hash_of(this->mapPic, sbil, opt);

  build_cache_ir ir;

  if (!this->examine_build_cache(opt, expected_hash, &ir)) {
    return false;
  }

  this->mapPic = std::move(ir.mapPic);
  this->build_opt = opt;
  this->schem = std::move(ir.schem);

  this->kernelStep = SCL_step::builded;
  return true;
}
