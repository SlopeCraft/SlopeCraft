#include "TokiSlopeCraft.h"
#include <fmt/format.h>
#include <fstream>
#include <span>
#include <functional>
#include <filesystem>

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
  return fmt::format("{}/{}", this->cache_dir.value(),
                     this->image_cvter.task_hash());
}

void TokiSlopeCraft::saveCache(std::string &err) const noexcept {
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
  if (!stdfs::create_directories(task_dir, ec)) {
    err = fmt::format(
        "Failed to create dir named \"{}\", error code = {}, message = {}",
        task_dir, ec.value(), ec.message());
    return;
  }

  err.clear();
  std::string temp_err{};

  // write colorset hash
  {
    const std::string name_hash_colorset = this->colorset_hash_file();

    temp_err = write_hash(name_hash_colorset, TokiSlopeCraft::Allowed.hash());
    if (!temp_err.empty()) {
      err = fmt::format("Failed to save colorset hash. Detail: {}", temp_err);
      return;
    }
  }

  const std::string convert_cache_file = fmt::format("{}/convert", task_dir);

  if (!this->image_cvter.save_cache(convert_cache_file.c_str())) {
    err = fmt::format("Failed to save image cvter cache.");
    return;
  }

  return;
}

bool TokiSlopeCraft::check_colorset_hash() const noexcept {
  const std::string hash_file = this->colorset_hash_file();
  std::vector<uint8_t> buf;
  {
    std::ifstream ifs{hash_file, std::ios::binary};

    if (!ifs) {  // file not exist
      return false;
    }

    const size_t file_size = std::filesystem::file_size(hash_file);
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