#include <zip.h>
#include <fmt/format.h>
#include <optional>
#include <cassert>
#include <tl/expected.hpp>
#include <vector>

struct block_list {
 public:
  std::vector<uint8_t> block_list_file;
};

tl::expected<block_list, std::string> parse_archive(zip_t* archive) noexcept {
  assert(archive != nullptr);

  auto read_file =
      [archive](const char* filename,
                std::vector<uint8_t>& dest) -> tl::expected<void, std::string> {
    auto fp = zip_fopen(archive, filename, ZIP_FL_UNCHANGED);
    if (fp == nullptr) {
      return tl::unexpected(
          fmt::format("Failed to open {}, last error: {}", filename,
                      zip_error_strerror(zip_get_error(archive))));
    }
    const auto index = zip_name_locate(archive, filename, ZIP_FL_UNCHANGED);
    assert(index >= 0);

    zip_stat_t stat{};
    if (zip_stat(archive, filename, ZIP_FL_UNCHANGED, &stat) != ZIP_ER_OK) {
      return tl::unexpected(
          fmt::format("Failed to open {}, last error: {}", filename,
                      zip_error_strerror(zip_get_error(archive))));
    }

    dest.resize(stat.size);
    const auto bytes = zip_fread(fp, dest.data(), dest.size());
    if (bytes != int64_t(stat.size)) {
      return tl::unexpected(
          fmt::format("Failed to read {}, the file contains {} bytes, but {} "
                      "decoded. Last error: {}",
                      filename, stat.size, bytes,
                      zip_error_strerror(zip_get_error(archive))));
    }
    return {};
  };

  std::vector<uint8_t> bytes{};
  auto res = read_file("BlockList.json", bytes);
  assert(res);

  return ::block_list{bytes};
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fmt::println("Must provide 1 arguments\n");
    return __LINE__;
  }

  const char* archive_path = argv[1];
  fmt::println("Try parsing {}", archive_path);

  int ec = ZIP_ER_OK;
  zip_t* archive = zip_open(archive_path, ZIP_RDONLY | ZIP_CHECKCONS, &ec);
  if (archive == nullptr || ec != ZIP_ER_OK) {
    fmt::println("Failed to open {}, error code = {}", archive_path, ec);
    return __LINE__;
  }

  //  libzippp::ZipArchive archive{archive_path, ""};
  //  if (archive.getZipHandle() == nullptr) {
  //    fmt::println("Failed to open {}", archive_path);
  //    return __LINE__;
  //  };
  //  if (!archive.isOpen() ||
  //      !archive.open(libzippp::ZipArchive::OpenMode::ReadOnly, true)) {
  //    fmt::println("Failed to open {}", archive_path);
  //    return __LINE__;
  //  }
  //
  auto list = parse_archive(archive).value();

  fmt::println("BlockList.json: \n{}",
               reinterpret_cast<const char*>(list.block_list_file.data()));

  return 0;
  // if (archive.getE) return 0;
}