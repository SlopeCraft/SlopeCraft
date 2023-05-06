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

const std::string_view hash_suffix{"*.sha3_512"};

void TokiSlopeCraft::saveCache(std::string_view cache_dir,
                               std::string &err) const noexcept {
  if (this->kernelStep < SlopeCraft::step::converted) {
    err = "Can not save cache before the image is converted.";
    return;
  }

  const std::string task_dir =
      fmt::format("{}/{}", cache_dir, this->image_cvter.task_hash());

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
    const std::string name_hash_colorset =
        fmt::format("{}/{}{}", cache_dir, "colorset", hash_suffix);

    temp_err = write_hash(name_hash_colorset, TokiSlopeCraft::Allowed.hash());
    if (!temp_err.empty()) {
      err = fmt::format("Failed to save colorset hash. Detail: {}", temp_err);
      return;
    }
  }

  const std::string convert_cache_file =
      fmt::format("{}/convert.sc_cache.gz", task_dir);

  if (!this->image_cvter.save_cache(convert_cache_file.c_str())) {
    err = fmt::format("Failed to save image cvter cache.");
    return;
  }

  return;
}