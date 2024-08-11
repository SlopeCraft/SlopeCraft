//
// Created by Joseph on 2024/8/11.
//

#ifndef SLOPECRAFT_BLOCKLIST_H
#define SLOPECRAFT_BLOCKLIST_H

#include <span>
#include <tl/expected.hpp>

#include "mc_block.h"

struct block_list_metainfo {
  std::string prefix_ZH;
  std::string prefix_EN;
  std::vector<std::string> required_mods;
};

class block_list : public ::SlopeCraft::block_list_interface {
 private:
  std::map<std::unique_ptr<mc_block>, uint8_t> m_blocks;

 public:
  block_list() = default;
  block_list(block_list &&) = default;
  ~block_list();

 public:
  size_t size() const noexcept override { return m_blocks.size(); }
  size_t get_blocks(mc_block_interface **dst, uint8_t *,
                    size_t capacity_in_elements) noexcept override;

  size_t get_blocks(const mc_block_interface **dst, uint8_t *,
                    size_t capacity_in_elements) const noexcept override;

  bool contains(const mc_block_interface *cp) const noexcept override {
    const mc_block *ptr = dynamic_cast<const mc_block *>(cp);
    return this->m_blocks.contains(
        reinterpret_cast<std::unique_ptr<mc_block> &>(ptr));
  }

 public:
  const auto &blocks() const noexcept { return this->m_blocks; }
  auto &blocks() noexcept { return this->m_blocks; }

  void clear() noexcept;
};

struct block_list_create_result {
  tl::expected<block_list, std::string> result;
  std::string warnings;
};

[[nodiscard]] block_list_create_result create_block_list_from_file(
    const char *zip_filename) noexcept;

[[nodiscard]] block_list_create_result create_block_list_from_buffer(
    std::span<const uint8_t>) noexcept;

#endif  // SLOPECRAFT_BLOCKLIST_H
