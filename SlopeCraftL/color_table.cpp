//
// Created by joseph on 4/15/24.
//

#include <fmt/format.h>
#include <boost/uuid/detail/md5.hpp>
#include "SCLDefines.h"
#include "color_table.h"
#include "water_item.h"
#include "structure_3D.h"
#include "utilities/ProcessBlockId/process_block_id.h"
#include "utilities/Schem/mushroom.h"

std::optional<color_table_impl> color_table_impl::create(
    const color_table_create_info &args) noexcept {
  color_table_impl result;
  result.mc_version_ = args.mc_version;
  result.map_type_ = args.map_type;
  {
    colorset_allowed_t a;
    a.need_find_side = (args.map_type == mapTypes::Slope);
    result.allowed = std::make_shared<colorset_allowed_t>(std::move(a));
  }

  auto report_err = [&args](errorFlag flag, std::string_view msg) {
    args.ui.report_error(flag, msg.data());
  };

  // block palette
  {
    for (size_t i = 0; i < 64; i++) {
      if (args.blocks[i] == nullptr) {
        result.blocks[i].clear();
        continue;
      }
      args.blocks[i]->copyTo(&result.blocks[i]);
      // fix block id to prevent potential errors
      {
        if (result.blocks[i].id.find(':') == result.blocks[i].id.npos) {
          result.blocks[i].id = "minecraft:" + result.blocks[i].id;
        }

        if (result.blocks[i].idOld.empty()) {
          result.blocks[i].idOld = result.blocks[i].id;
        }

        if (result.blocks[i].idOld.size() > 0 &&
            (result.blocks[i].idOld.find(':') == result.blocks[i].idOld.npos)) {
          result.blocks[i].idOld = "minecraft:" + result.blocks[i].idOld;
        }
      }
    }
  }

  args.ui.report_working_status(workStatus::collectingColors);

  Eigen::ArrayXi baseColorVer(64);  // 基色对应的版本
  baseColorVer.setConstant((int)SCL_gameVersion::FUTURE);
  baseColorVer.segment(0, 52).setConstant((int)SCL_gameVersion::ANCIENT);
  baseColorVer.segment(52, 7).setConstant((int)SCL_gameVersion::MC16);
  baseColorVer.segment(59, 3).setConstant((int)SCL_gameVersion::MC17);

  std::array<bool, 256> m_index;
  for (short index = 0; index < 256; index++) {
    m_index[index] =
        true;  // 默认可以使用这种颜色，逐次判断各个不可以使用的条件

    if (!args.basecolor_allow_LUT[index2baseColor(
            index)]) {  // 在 allowedBaseColor 中被禁止
      m_index[index] = false;
      continue;
    }
    if (index2baseColor(index) == 0) {  // 空气禁用
      m_index[index] = false;
      continue;
    }
    if ((int)result.mc_version_ <
        baseColorVer(index2baseColor(index))) {  // 版本低于基色版本
      m_index[index] = false;
      continue;
    }
    if (result.blocks[index2baseColor(index)].id.empty()) {  // 空 id
      m_index[index] = false;
      continue;
    }
    /*
    if ((mapType == mapTypes::Wall) &&
        !blockPalette[index2baseColor(index)]
             .wallUseable) { //墙面像素画且当前方块不适合墙面

      m_index[index] = false;
      continue;
    }*/
    if (result.is_vanilla() &&
        (index2depth(index) >= 3)) {  // 可实装的地图画不允许第四种阴影
      m_index[index] = false;
      continue;
    }
    if (index2baseColor(index) == 12) {  // 如果是水且非墙面
      if (result.is_flat() && index2depth(index) != 2) {  // 平板且水深不是 1 格
        m_index[index] = false;
        continue;
      }
    } else {
      if (result.is_flat() && index2depth(index) != 1) {  // 平板且阴影不为 1
        m_index[index] = false;
        continue;
      }
    }
  }

  if (!result.allowed->apply_allowed(*SlopeCraft::basic_colorset, m_index)) {
    std::string msg = fmt::format(
        "Too few usable color(s) : only {}  colors\nAvaliable base color(s) : ",
        result.allowed->color_count());

    for (int idx = 0; idx < result.allowed->color_count(); idx++) {
      msg += std::to_string(result.allowed->Map(idx)) + ", ";
    }

    report_err(errorFlag::USEABLE_COLOR_TOO_FEW, msg);
    return std::nullopt;
  }

  args.ui.report_working_status(workStatus::none);

  return result;
}

structure_3D *color_table_impl::build(
    const converted_image &cvted, const build_options &option) const noexcept {
  auto opt = structure_3D_impl::create(
      *this, dynamic_cast<const converted_image_impl &>(cvted), option);
  if (opt) {
    return new structure_3D_impl{std::move(opt.value())};
  }
  return nullptr;
}

std::vector<std::string_view> color_table_impl::block_id_list(
    bool contain_air) const noexcept {
  std::vector<std::string_view> dest;
  dest.reserve(this->blocks.size() + 1);
  if (contain_air) {
    dest.emplace_back("minecraft:air");
  }
  for (auto &blk : this->blocks) {
    dest.emplace_back(blk.idForVersion(this->mc_version_));
  }
  return dest;
}

const mc_block *color_table_impl::find_block_for_index(
    int idx, std::string_view blkid) const noexcept {
  if (idx < 0) {
    return nullptr;
  }

  if (idx < (int)this->blocks.size()) {
    // assert(this->blocks[idx].id == blkid);
    return &this->blocks[idx];
  }

  // the block must be mushroom
  namespace lsi = libSchem::internal;
  using lsi::mushroom_type;

  blkid::char_range pure_id_range;
  // invalid block id
  if (!blkid::process_blk_id(blkid, nullptr, &pure_id_range, nullptr)) {
    return nullptr;
  }

  std::string_view pure_id{pure_id_range.begin(), pure_id_range.end()};

  auto mush_type_opt = lsi::pureid_to_type(pure_id);
  if (!mush_type_opt.has_value()) {
    return nullptr;
  }

  uint8_t expected_basecolor = 0;
  switch (mush_type_opt.value()) {
    case mushroom_type::red:
      expected_basecolor = 28;
      break;
    case mushroom_type::brown:
      expected_basecolor = 10;
      break;
    case mushroom_type::stem:
      expected_basecolor = 3;
      break;
  }

  const auto *blkp = this->find_block_for_index(expected_basecolor, {});

  if (blkp == nullptr) {
    return nullptr;
  }

  if (lsi::pureid_to_type(pure_id) != mush_type_opt) {
    return nullptr;
  }

  return blkp;
}

uint64_t color_table_impl::hash() const noexcept {
  boost::uuids::detail::md5 hash;
  SC_HASH_ADD_DATA(hash, this->map_type_)
  SC_HASH_ADD_DATA(hash, this->mc_version_)

  this->allowed->hash_add_data(hash);
  decltype(hash)::digest_type digest;
  hash.get_digest(digest);
  std::array<uint64_t, 2> result;
  memcpy(result.data(), digest, sizeof digest);
  static_assert(sizeof(digest) == sizeof(result));
  return result[0] ^ result[1];
}

std::filesystem::path color_table_impl::self_cache_dir(
    const char *cache_root_dir) const noexcept {
  return fmt::format("{}/{:x}", cache_root_dir, this->hash());
}

std::filesystem::path color_table_impl::convert_task_cache_filename(
    const_image_reference original_img, const convert_option &option,
    const char *cache_root_dir) const noexcept {
  auto self_cache_dir = this->self_cache_dir(cache_root_dir);
  self_cache_dir.append("convert");
  self_cache_dir.append(fmt::format(
      "{:x}", converted_image_impl::convert_task_hash(original_img, option)));
  return self_cache_dir;
}

bool color_table_impl::has_convert_cache(
    const_image_reference original_img, const convert_option &option,
    const char *cache_root_dir) const noexcept {
  auto path =
      this->convert_task_cache_filename(original_img, option, cache_root_dir);
  return std::filesystem::is_regular_file(path);
}

std::string color_table_impl::save_convert_cache(
    const_image_reference original_img, const convert_option &option,
    const converted_image &cvted, const char *cache_root_dir) const noexcept {
  try {
    auto filename =
        this->convert_task_cache_filename(original_img, option, cache_root_dir);
    std::filesystem::create_directories(filename.parent_path());

    auto err =
        dynamic_cast<const converted_image_impl &>(cvted).save_cache(filename);
    if (!err.empty()) {
      return fmt::format("Failed to save cache to file \"{}\": {}",
                         filename.string(), err);
    }
  } catch (const std::exception &e) {
    return fmt::format("Caught exception: {}", e.what());
  }

  return {};
}

[[nodiscard]] tl::expected<converted_image_impl, std::string>
color_table_impl::load_convert_cache(
    const_image_reference original_img, const convert_option &option,
    const char *cache_root_dir) const noexcept {
  return converted_image_impl::load_cache(
      *this,
      this->convert_task_cache_filename(original_img, option, cache_root_dir));
}

std::filesystem::path color_table_impl::build_task_cache_filename(
    const converted_image &cvted_, const build_options &opt,
    const char *cache_root_dir) const noexcept {
  boost::uuids::detail::md5 hash;
  SC_HASH_ADD_DATA(hash, opt.max_allowed_height)
  SC_HASH_ADD_DATA(hash, opt.bridge_interval)
  SC_HASH_ADD_DATA(hash, opt.compress_method)
  SC_HASH_ADD_DATA(hash, opt.glass_method)
  SC_HASH_ADD_DATA(hash, opt.fire_proof)
  SC_HASH_ADD_DATA(hash, opt.enderman_proof)
  SC_HASH_ADD_DATA(hash, opt.connect_mushrooms)

  auto &cvted = dynamic_cast<const converted_image_impl &>(cvted_);
  // this can be optimized
  auto map_mat = cvted.converter.mapcolor_matrix();
  hash.process_bytes(map_mat.data(), map_mat.size() * sizeof(uint8_t));

  decltype(hash)::digest_type dig;
  hash.get_digest(dig);
  std::array<uint64_t, 2> dig_u64{0, 0};
  memcpy(dig_u64.data(), dig, sizeof(dig));
  const uint64_t hash_u64 = dig_u64[0] ^ dig_u64[1];

  auto path = this->self_cache_dir(cache_root_dir);
  path.append("build");
  path.append(fmt::format("{:x}", hash_u64));
  return path;
}

bool color_table_impl::save_build_cache(const converted_image &cvted,
                                        const build_options &option,
                                        const structure_3D &structure,
                                        const char *cache_root_dir,
                                        string_deliver *error) const noexcept {
  const auto filename =
      this->build_task_cache_filename(cvted, option, cache_root_dir);
  auto err_msg =
      dynamic_cast<const structure_3D_impl &>(structure).save_cache(filename);
  write_to_sd(error, err_msg);

  return err_msg.empty();
}

bool color_table_impl::has_build_cache(
    const SlopeCraft::converted_image &cvted,
    const SlopeCraft::build_options &option,
    const char *cache_root_dir) const noexcept {
  const auto filename =
      this->build_task_cache_filename(cvted, option, cache_root_dir);
  return std::filesystem::is_regular_file(filename);
}

structure_3D *color_table_impl::load_build_cache(
    const SlopeCraft::converted_image &cvted,
    const SlopeCraft::build_options &option, const char *cache_root_dir,
    SlopeCraft::string_deliver *error) const noexcept {
  const auto filename =
      this->build_task_cache_filename(cvted, option, cache_root_dir);
  auto res = structure_3D_impl::load_cache(filename);
  if (res) {
    write_to_sd(error, "");
    return new structure_3D_impl{std::move(res.value())};
  }
  write_to_sd(error, res.error());
  return nullptr;
}

void color_table_impl::stat_blocks(const structure_3D &s,
                                   size_t buffer[64]) const noexcept {
  std::fill(buffer, buffer + 64, 0);
  const auto &structure = dynamic_cast<const structure_3D_impl &>(s);

  const auto schem_stat = structure.schem.stat_blocks();
  for (size_t schem_blk_id = 0; schem_blk_id < structure.schem.palette_size();
       schem_blk_id++) {
    if (schem_blk_id == 0) {  // ignore air
      continue;
    }
    assert(schem_blk_id > 0);
    if (schem_stat[schem_blk_id] <= 0) {
      continue;
    }

    const auto strid = structure.schem.palette()[schem_blk_id];
    const auto blkp = this->find_block_for_index(schem_blk_id - 1, strid);
    if (blkp == nullptr) {
      std::cerr << fmt::format(
          "Failed to find \"{}\" in color_table, this type of block will not "
          "be counted.\n",
          strid);
      continue;
    }
    const ptrdiff_t index_in_palette = blkp - this->blocks.data();
    assert(index_in_palette >= 0);
    assert(index_in_palette < this->blocks.size());
    buffer[index_in_palette] += schem_stat[schem_blk_id];
  }
}

std::string color_table_impl::impl_generate_test_schematic(
    std::string_view filename,
    const test_blocklist_options &option) const noexcept {
  if (!filename.ends_with(".nbt")) {
    return "File name should ends with \".nbt\"";
  }
  libSchem::Schem test;
  test.set_MC_major_version_number(this->mc_version_);
  test.set_MC_version_number(
      MCDataVersion::suggested_version(this->mc_version_));
  // const simpleBlock ** realSrc=(const simpleBlock **)src;
  std::vector<const mc_block *> realSrc;
  std::vector<uint8_t> realBaseColor;
  realSrc.clear();
  realBaseColor.clear();
  for (size_t idx = 0; idx < option.block_count; idx++) {
    if (option.block_ptrs[idx]->getVersion() > (int)this->mc_version_) {
      continue;
    }
    realSrc.emplace_back(static_cast<const mc_block *>(option.block_ptrs[idx]));
    realBaseColor.emplace_back(option.basecolors[idx]);
  }

  std::vector<std::vector<int>> block_counter;
  block_counter.resize(64);

  for (uint32_t idx = 0; idx < realSrc.size(); idx++) {
    block_counter[realBaseColor[idx]].push_back(idx);
  }

  {
    std::vector<const char *> ids;
    ids.reserve(realSrc.size() + 1);
    ids.emplace_back("minecraft:air");
    for (auto i : realSrc) {
      ids.emplace_back(i->idForVersion(this->mc_version_));
    }

    test.set_block_id(ids.data(), ids.size());
  }

  int xSize = 0;
  constexpr int zSize = 64, ySize = 2;
  for (const auto &it : block_counter) {
    xSize = std::max(size_t(xSize), it.size());
  }
  test.resize(xSize + 1, ySize, zSize);
  test.set_zero();

  for (uint8_t base = 0; base < 64; base++) {
    for (uint32_t idx = 0; idx < block_counter[base].size(); idx++) {
      int xPos = idx;
      int yPos = 0;
      int zPos = base;

      test(xPos, yPos, zPos) = block_counter[base][idx] + 1;
    }
    test(block_counter[base].size(), 1, base) = 1;  // glass block
  }

  SCL_errorFlag err;
  std::string detail;
  const bool success = test.export_structure(filename, true, &err, &detail);

  if (!success) {
    return fmt::format(
        "Failed to export structure file {}, error code = {}, detail: {}",
        filename, int(err), detail);
  } else {
    return {};
  }
}

std::array<uint32_t, 256> LUT_map_color_to_ARGB() noexcept {
  const auto &basic = *SlopeCraft::basic_colorset;
  std::array<uint32_t, 256> ret;
  ret.fill(0);
  for (size_t idx = 0; idx < 256; idx++) {
    const auto map_color = index2mapColor(idx);
    if (index2baseColor(idx) == 0) {
      ret[map_color] = 0x00000000;
      continue;
    }
    ret[map_color] =
        RGB2ARGB(basic.RGB(idx, 0), basic.RGB(idx, 1), basic.RGB(idx, 2));
  }
  return ret;
}