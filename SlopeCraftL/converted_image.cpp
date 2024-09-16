//
// Created by joseph on 4/17/24.
//

#include <fmt/format.h>
#include <boost/uuid/detail/md5.hpp>
#include <utilities/ExternalConverters/GAConverter/GAConverter.h>
#include "SCLDefines.h"
#include "converted_image.h"
#include "color_table.h"
#include "height_line.h"
#include "lossy_compressor.h"
#include "NBTWriter/NBTWriter.h"
#include "structure_3D.h"

converted_image_impl::converted_image_impl(const color_table_impl &table)
    : converter{*SlopeCraft::basic_colorset, *table.allowed},
      game_version{table.mc_version()},
      colorset{table.allowed} {}

converted_image *color_table_impl::convert_image(
    const_image_reference original_img,
    const convert_option &option) const noexcept {
  converted_image_impl cvted{*this};

  const auto algo = (option.algo == convertAlgo::gaCvter)
                        ? convertAlgo::RGB_Better
                        : option.algo;
  cvted.converter.set_raw_image(original_img.data, original_img.rows,
                                original_img.cols, false);
  {
    heu::GAOption opt;
    opt.crossoverProb = option.ai_cvter_opt.crossoverProb;
    opt.mutateProb = option.ai_cvter_opt.mutationProb;
    opt.maxGenerations = option.ai_cvter_opt.maxGeneration;
    opt.maxFailTimes = option.ai_cvter_opt.maxFailTimes;
    opt.populationSize = option.ai_cvter_opt.popSize;

    cvted.converter.convert_image(algo, option.dither, &opt);
  }

  option.progress.set_range(0, 4 * cvted.size(), 4 * cvted.size());
  option.ui.report_working_status(workStatus::none);

  return new converted_image_impl{std::move(cvted)};
}

void converted_image_impl::get_compressed_image(
    const structure_3D &structure_, uint32_t *buffer) const noexcept {
  const auto &structure = dynamic_cast<const structure_3D_impl &>(structure_);
  assert(this->rows() == structure.map_color.rows());
  assert(this->cols() == structure.map_color.cols());

  const auto LUT = LUT_map_color_to_ARGB();
  Eigen::Map<
      Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      dest{buffer, static_cast<int64_t>(this->rows()),
           static_cast<int64_t>(this->cols())};
  dest.fill(0);
  for (size_t r = 0; r < this->rows(); r++) {
    for (size_t c = 0; c < this->cols(); c++) {
      const auto map_color = structure.map_color(r, c);
      assert(map_color >= 0);
      assert(map_color <= 255);
      dest(r, c) = LUT[map_color];
    }
  }
}

bool converted_image_impl::export_map_data(
    const SlopeCraft::map_data_file_options &option) const noexcept {
  const std::filesystem::path dir{option.folder_path};
  const auto mapPic = this->converter.mapcolor_matrix();
  const int rows = this->map_rows();
  const int cols = this->map_cols();
  //  const int rows = ceil(mapPic.rows() / 128.0f);
  //  const int cols = ceil(mapPic.cols() / 128.0f);
  option.progress.set_range(0, 128 * rows * cols, 0);

  // int offset[2] = {0, 0};  // r,c
  int currentIndex = option.begin_index;

  option.ui.report_working_status(workStatus::writingMapDataFiles);

  int fail_count = 0;
  for (int c = 0; c < cols; c++) {
    for (int r = 0; r < rows; r++) {
      const std::array<int, 2> offset = {r * 128, c * 128};
      std::filesystem::path current_filename = dir;
      current_filename.append(fmt::format("map_{}.dat", currentIndex));

      NBT::NBTWriter<true> MapFile;

      if (!MapFile.open(current_filename.string().c_str())) {
        option.ui.report_error(errorFlag::EXPORT_MAP_DATA_FAILURE,
                               fmt::format("Failed to create nbt file {}",
                                           current_filename.string())
                                   .c_str());
        fail_count += 1;
        continue;
      }
      switch (this->game_version) {
        case SCL_gameVersion::MC12:
        case SCL_gameVersion::MC13:
          break;
        case SCL_gameVersion::MC14:
        case SCL_gameVersion::MC15:
        case SCL_gameVersion::MC16:
        case SCL_gameVersion::MC17:
        case SCL_gameVersion::MC18:
        case SCL_gameVersion::MC19:
        case SCL_gameVersion::MC20:
        case SCL_gameVersion::MC21:
          MapFile.writeInt(
              "DataVersion",
              static_cast<int32_t>(
                  MCDataVersion::suggested_version(this->game_version)));
          break;
        default:
          cerr << "Wrong game version!\n";
          break;
      }

      static const std::string ExportedBy = fmt::format(
          "Exported by SlopeCraft {}, developed by TokiNoBug", SC_VERSION_STR);
      MapFile.writeString("ExportedBy", ExportedBy.data());
      MapFile.writeCompound("data");
      {
        MapFile.writeByte("scale", 0);
        MapFile.writeByte("trackingPosition", 0);
        MapFile.writeByte("unlimitedTracking", 0);
        MapFile.writeInt("xCenter", 0);
        MapFile.writeInt("zCenter", 0);
        switch (this->game_version) {
          case SCL_gameVersion::MC12:
            MapFile.writeByte("dimension", 114);
            MapFile.writeShort("height", 128);
            MapFile.writeShort("width", 128);
            break;
          case SCL_gameVersion::MC13:
            MapFile.writeListHead("banners", NBT::Compound, 0);
            MapFile.writeListHead("frames", NBT::Compound, 0);
            MapFile.writeInt("dimension", 889464);
            break;
          case SCL_gameVersion::MC14:
            MapFile.writeListHead("banners", NBT::Compound, 0);
            MapFile.writeListHead("frames", NBT::Compound, 0);
            MapFile.writeInt("dimension", 0);
            MapFile.writeByte("locked", 1);
            break;
          case SCL_gameVersion::MC15:
            MapFile.writeListHead("banners", NBT::Compound, 0);
            MapFile.writeListHead("frames", NBT::Compound, 0);
            MapFile.writeInt("dimension", 0);
            MapFile.writeByte("locked", 1);
            break;
          case SCL_gameVersion::MC16:
          case SCL_gameVersion::MC17:
          case SCL_gameVersion::MC18:
          case SCL_gameVersion::MC19:
          case SCL_gameVersion::MC20:
          case SCL_gameVersion::MC21:
            MapFile.writeListHead("banners", NBT::Compound, 0);
            MapFile.writeListHead("frames", NBT::Compound, 0);
            MapFile.writeString("dimension", "minecraft:overworld");
            MapFile.writeByte("locked", 1);
            break;
          default:
            cerr << "Wrong game version!\n";
            option.ui.report_error(errorFlag::UNKNOWN_MAJOR_GAME_VERSION,
                                   "Unknown major game version!");
            fail_count += 1;
            continue;
        }

        MapFile.writeByteArrayHead("colors", 16384);
        {
          for (short rr = 0; rr < 128; rr++) {
            for (short cc = 0; cc < 128; cc++) {
              uint8_t ColorCur;
              if (rr + offset[0] < mapPic.rows() &&
                  cc + offset[1] < mapPic.cols())
                ColorCur = mapPic(rr + offset[0], cc + offset[1]);
              else
                ColorCur = 0;
              MapFile.writeByte("this should never be seen", ColorCur);
            }
            option.progress.add(1);
          }
        }
      }
      MapFile.endCompound();
      MapFile.close();

      currentIndex++;
    }
  }
  option.ui.report_working_status(workStatus::none);
  return (fail_count == 0);
}

std::optional<converted_image_impl::height_maps>
converted_image_impl::height_info(const build_options &option) const noexcept {
  //
  //  std::unordered_map<rc_pos, water_y_range> water_list;

  Eigen::ArrayXXi map_color = this->converter.mapcolor_matrix().cast<int>();

  const bool allow_lossless_compress =
      int(option.compress_method) bitand int(SCL_compressSettings::NaturalOnly);
  const bool allow_lossy_compress =
      int(option.compress_method) bitand int(compressSettings::ForcedOnly);

  if (((map_color - 4 * (map_color / 4)) >= 3).any()) {
    std::string msg =
        "Fatal error : SlopeCraftL found map color with depth 3 in a "
        "vanilla map.\n Map contents (map color matrix in col-major) :\n[";
    for (int c = 0; c < map_color.cols(); c++) {
      for (int r = 0; r < map_color.rows(); r++) {
        fmt::format_to(std::back_insert_iterator{msg}, "{},", map_color(r, c));
      }
      msg += ";\n";
    }
    msg += "];\n";
    option.ui.report_error(errorFlag::DEPTH_3_IN_VANILLA_MAP, msg.c_str());
    return std::nullopt;
  }

  Eigen::ArrayXXi base, high_map, low_map;
  base.setZero(this->rows() + 1, this->cols());
  high_map.setZero(this->rows() + 1, this->cols());
  low_map.setZero(this->rows() + 1, this->cols());
  std::unordered_map<rc_pos, water_y_range> water_list;

  lossy_compressor compressor;
  compressor.ui = option.ui;
  compressor.progress_bar = option.sub_progressbar;
  for (int64_t c = 0; c < map_color.cols(); c++) {
    // cerr << "Coloumn " << c << '\n';
    height_line HL;
    // getTokiColorPtr(c,&src[0]);
    HL.make(map_color.col(c), allow_lossless_compress);

    if ((HL.maxHeight() > option.max_allowed_height) and allow_lossy_compress) {
      std::vector<const TokiColor *> ptr(map_color.rows());

      this->converter.col_TokiColor_ptrs(c, ptr);
      // getTokiColorPtr(c, &ptr[0]);

      compressor.setSource(HL.getBase(), ptr);
      bool success = compressor.compress(option.max_allowed_height,
                                         allow_lossless_compress);
      Eigen::ArrayXi temp;
      HL.make(&ptr[0], compressor.getResult(), allow_lossless_compress, &temp);
      if (!success) {
        option.ui.report_error(
            SCL_errorFlag::LOSSYCOMPRESS_FAILED,
            fmt::format("Failed to compress the 3D structure at column {}. You "
                        "have required that max height <= {}, but SlopeCraft "
                        "is only able to this column to max height = {}.",
                        c, option.max_allowed_height, HL.maxHeight())
                .data());
        return std::nullopt;
      }
      map_color.col(c) = temp;
    }
    base.col(c) = HL.getBase();
    high_map.col(c) = HL.getHighLine();
    low_map.col(c) = HL.getLowLine();

    auto hl_water_list = HL.getWaterMap();
    water_list.reserve(water_list.size() + hl_water_list.size());
    for (const auto &[r, water_item] : hl_water_list) {
      water_list.emplace(
          rc_pos{static_cast<int32_t>(r), static_cast<int32_t>(c)}, water_item);
    }

    option.main_progressbar.add(4 * this->size());
  }

  return height_maps{.map_color = map_color,
                     .base = base,
                     .high_map = high_map,
                     .low_map = low_map,
                     .water_list = water_list};
}

uint64_t converted_image_impl::convert_task_hash(
    const_image_reference original_img, const convert_option &option) noexcept {
  boost::uuids::detail::md5 hash;

  SC_HASH_ADD_DATA(hash, option.algo)
  SC_HASH_ADD_DATA(hash, option.dither)
  if (option.algo == SCL_convertAlgo::gaCvter) {
    SC_HASH_ADD_DATA(hash, option.ai_cvter_opt.popSize)
    SC_HASH_ADD_DATA(hash, option.ai_cvter_opt.maxGeneration)
    SC_HASH_ADD_DATA(hash, option.ai_cvter_opt.maxFailTimes)
    SC_HASH_ADD_DATA(hash, option.ai_cvter_opt.crossoverProb)
    SC_HASH_ADD_DATA(hash, option.ai_cvter_opt.mutationProb)
  }

  hash.process_bytes(original_img.data, original_img.rows * original_img.cols);

  decltype(hash)::digest_type dig;
  hash.get_digest(dig);
  std::array<uint64_t, 2> temp;
  memcpy(temp.data(), dig, sizeof(temp));
  return temp[0] ^ temp[1];
}

std::string converted_image_impl::save_cache(
    const std::filesystem::path &file) const noexcept {
  if (this->converter.save_cache(file.string().c_str())) {
    return "Failed to open file.";
  }
  return {};
}

tl::expected<converted_image_impl, std::string>
converted_image_impl::load_cache(const color_table_impl &table,
                                 const std::filesystem::path &file) noexcept {
  converted_image_impl ret{table};
  if (!std::filesystem::is_regular_file(file)) {
    return tl::make_unexpected("No such file");
  }
  if (!ret.converter.load_cache(file.string().c_str())) {
    return tl::make_unexpected("Failed to load cache, the cache is incorrect");
  }
  return ret;
}

bool converted_image_impl::is_converted_from(
    const color_table &table_) const noexcept {
  const auto &table = dynamic_cast<const color_table_impl &>(table_);
  return (this->colorset.get() == table.allowed.get());
}
#include <cstdint>
#include <cmath>
#include <nbt_tags.h>
#include <text/json_formatter.h>
#include <boost/multi_array.hpp>
#include <sNBT_formatter/sNBT_formatter.h>

constexpr int chest_rows = 3;
constexpr int chest_cols = 9;

int8_t chest_index_to_slot(int row, int col) noexcept {
  assert(row >= 0 and row < chest_rows);
  assert(col >= 0 and col < chest_cols);
  return static_cast<int8_t>(row * chest_cols + col);
}

nbt::tag_compound &get_or_setup_field(nbt::tag_compound &parent,
                                      const std::string &key) noexcept {
  if (parent.has_key(key, nbt::tag_type::Compound)) {
    return parent[key].as<nbt::tag_compound>();
  }
  auto ret = parent.emplace<nbt::tag_compound>(key, nbt::tag_compound{});
  return ret.first->second.as<nbt::tag_compound>();
}

// Merge each 3*9 zone into a chest recursively, until whole item matrix is
// merged into one chest item
nbt::tag_compound merge_with_chest(
    boost::multi_array<nbt::tag_compound, 2> item_matrix,
    const map_data_file_give_command_options &option) noexcept {
  const size_t new_rows = std::ceil(float(item_matrix.shape()[0]) / chest_rows);
  const size_t new_cols = std::ceil(float(item_matrix.shape()[1]) / chest_cols);
  boost::multi_array<nbt::tag_compound, 2> merged_chests{
      boost::extents[new_rows][new_cols]};
  // Move and merge previous data into new 2d array
  for (int merged_col = 0; merged_col < new_cols; merged_col++) {
    for (int merged_row = 0; merged_row < new_rows; merged_row++) {
      nbt::tag_list item_list;
      for (int c_offset = 0; c_offset < chest_cols; c_offset++) {
        for (int r_offset = 0; r_offset < chest_rows; r_offset++) {
          const int c_original = c_offset + merged_col * chest_cols;
          const int r_original = r_offset + merged_row * chest_rows;
          if (r_original >= item_matrix.shape()[0] or
              c_original >= item_matrix.shape()[1]) {
            continue;
          }
          nbt::tag_compound cur_item{
              std::move(item_matrix[r_original][c_original])};
          if (cur_item.size() <= 0) {
            // skip empty item
            continue;
          }
          const int8_t slot = chest_index_to_slot(r_offset, c_offset);
          if (not option.after_1_20_5) {
            // set slot
            cur_item.emplace<nbt::tag_byte>("Slot", slot);
            item_list.emplace_back<nbt::tag_compound>(std::move(cur_item));
          } else {
            nbt::tag_compound item;
            item.emplace<nbt::tag_compound>("item", std::move(cur_item));
            item.emplace<nbt::tag_byte>("slot", slot);
            item_list.emplace_back<nbt::tag_compound>(std::move(item));
          }
        }
      }

      nbt::tag_compound chest_item;
      chest_item.emplace<nbt::tag_string>("id", "minecraft:chest");
      if (not option.after_1_20_5) {
        chest_item.emplace<nbt::tag_byte>("Count", 1);
      } else {
        chest_item.emplace<nbt::tag_int>("count", 1);
      }
      if (not option.after_1_20_5) {
        nbt::tag_compound blk_entity_tag;
        blk_entity_tag.emplace<nbt::tag_list>("Items", std::move(item_list));
        get_or_setup_field(chest_item, "tag")
            .emplace<nbt::tag_compound>("BlockEntityTag",
                                        std::move(blk_entity_tag));
      } else {
        get_or_setup_field(chest_item, "components")
            .emplace<nbt::tag_list>("minecraft:container",
                                    std::move(item_list));
      }

      merged_chests[merged_row][merged_col] = std::move(chest_item);
    }
  }

  if (new_rows <= 1 and new_cols <= 1) {
    return merged_chests[0][0];
  }

  return merge_with_chest(std::move(merged_chests), option);
}

bool converted_image_impl::get_map_command(
    const map_data_file_give_command_options &option) const {
  if (option.destination == nullptr) {
    return false;
  }

  const int map_rows = this->map_rows();
  const int map_cols = this->map_cols();
  if (map_rows <= 0 or map_cols <= 0) {
    std::string err_msg =
        fmt::format("Invalid map size: {} rows, {} cols", map_rows, map_cols);
    option.destination->write(err_msg.c_str(), err_msg.size());
    return false;
  }

  auto item_of_location = [map_rows, map_cols, option](
                              int r, int c) -> nbt::tag_compound {
    assert(r >= 0 and r < map_rows);
    assert(c >= 0 and c < map_cols);
    // Maps are placed in col-major
    const int index = option.begin_index + r + c * map_rows;

    std::string name;
    if (option.set_name_as_index)
      name = fmt::format("{{\"text\":\"[{},{}]\"}}", r, c);

    nbt::tag_compound result;
    nbt::tag_compound tag;
    result.emplace<nbt::tag_string>("id", "minecraft:filled_map");
    // set up item name
    if (option.set_name_as_index and not option.after_1_20_5) {
      nbt::tag_compound display;
      display.emplace<nbt::tag_string>("Name",
                                       nbt::tag_string{std::move(name)});
      tag.emplace<nbt::tag_compound>("display", std::move(display));
    }
    // set up item properties
    if (not option.after_1_12) {
      result.emplace<nbt::tag_byte>("Count", option.stack_count);
      result.emplace<nbt::tag_int>("Damage", index);
      result.emplace<nbt::tag_compound>("tag", std::move(tag));
      return result;
    }
    if (not option.after_1_20_5) {
      result.emplace<nbt::tag_byte>("Count", option.stack_count);
      tag.emplace<nbt::tag_int>("map", index);
      result.emplace<nbt::tag_compound>("tag", std::move(tag));
      return result;
    }
    result.emplace<nbt::tag_int>("count", option.stack_count);
    tag.emplace<nbt::tag_int>("map_id", index);
    tag.emplace<nbt::tag_string>("custom_name",
                                 nbt::tag_string{std::move(name)});
    result.emplace<nbt::tag_compound>("components", std::move(tag));
    return result;
  };

  nbt::tag_compound chest_all_in_one;
  auto erase_if = [&chest_all_in_one](const char *key) noexcept {
    if (chest_all_in_one.has_key(key)) {
      chest_all_in_one.erase(key);
    }
  };
  {
    boost::multi_array<nbt::tag_compound, 2> maps{
        boost::extents[map_rows][map_cols]};
    for (int r = 0; r < map_rows; r++) {
      for (int c = 0; c < map_cols; c++) {
        maps[r][c] = item_of_location(r, c);
      }
    }
    chest_all_in_one = merge_with_chest(std::move(maps), option);
  }

  const std::string item_id =
      chest_all_in_one.at("id").as<nbt::tag_string>().get();
  option.destination->write(fmt::format("/give @p {}", item_id).c_str());
  erase_if("Count");
  erase_if("count");
  erase_if("id");
  std::ostringstream oss;
  {
    sNBT::sNBT_format_visitor formatter{oss};
    if (not option.after_1_20_5) {
      chest_all_in_one.at("tag").as<nbt::tag_compound>().accept(formatter);
    } else {
      const nbt::tag_compound &components =
          chest_all_in_one.at("components").as<nbt::tag_compound>();
      oss << '[';
      for (auto &[key, val] : components) {
        oss << key << '=';
        val.get().accept(formatter);
        oss << ',';
      }
      oss << ']';
    }
  }
  std::string_view snbt = oss.view();
  option.destination->write(snbt.data(), snbt.size());
  option.destination->write(" 1");

  return true;
}

Eigen::Matrix<int, 3, 2> transform_mat_of(SCL_map_facing facing) noexcept {
  switch (facing) {
    case SCL_map_facing::wall_west:
      return Eigen::Matrix<int, 3, 2>{{0, 0},   //
                                      {-1, 0},  // r y-
                                      {0, 1}};  // c z+
    case SCL_map_facing::wall_north:
      return Eigen::Matrix<int, 3, 2>{{0, -1},  // c x-
                                      {-1, 0},  // r y-
                                      {0, 0}};  //
    case SCL_map_facing::wall_east:
      return Eigen::Matrix<int, 3, 2>{{0, 0},    //
                                      {-1, 0},   // r y-
                                      {0, -1}};  // c z-
    case SCL_map_facing::wall_south:
      return Eigen::Matrix<int, 3, 2>{{0, 1},   // c x+
                                      {-1, 0},  // r y-
                                      {0, 0}};  //
    case SCL_map_facing::top_south:
      return Eigen::Matrix<int, 3, 2>{{0, -1},   // c x-
                                      {0, 0},    //
                                      {-1, 0}};  // r z-
    case SCL_map_facing::top_north:
      return Eigen::Matrix<int, 3, 2>{{0, 1},   // c x+
                                      {0, 0},   //
                                      {1, 0}};  // r z+
    case SCL_map_facing::top_east:
      return Eigen::Matrix<int, 3, 2>{{-1, 0},  // r x-
                                      {0, 0},   //
                                      {0, 1}};  // c z+
    case SCL_map_facing::top_west:
      return Eigen::Matrix<int, 3, 2>{{1, 0},    // r x+
                                      {0, 0},    //
                                      {0, -1}};  // c z-
    case SCL_map_facing::bottom_north:
      return Eigen::Matrix<int, 3, 2>{{0, -1},  // c x-
                                      {0, 0},   //
                                      {1, 0}};  // r z+
    case SCL_map_facing::bottom_south:
      return Eigen::Matrix<int, 3, 2>{{0, 1},    // c x+
                                      {0, 0},    //
                                      {-1, 0}};  // r z-
    case SCL_map_facing::bottom_east:
      return Eigen::Matrix<int, 3, 2>{{-1, 0},   // r x-
                                      {0, 0},    //
                                      {0, -1}};  // c z-
    case SCL_map_facing::bottom_west:
      return Eigen::Matrix<int, 3, 2>{{1, 0},   // r x+
                                      {0, 0},   //
                                      {0, 1}};  // c z+
  }
  std::unreachable();
}

uint8_t rotation_of(SCL_map_facing facing) noexcept {
  switch (facing) {
    case SCL_map_facing::top_south:
    case SCL_map_facing::bottom_north:
      return 2;
    case SCL_map_facing::top_north:
    case SCL_map_facing::bottom_south:
      return 0;
    case SCL_map_facing::top_east:
    case SCL_map_facing::bottom_east:
      return 1;
    case SCL_map_facing::top_west:
    case SCL_map_facing::bottom_west:
      return 3;
    default:
      return 0;
  }
}

libSchem::Schem converted_image_impl::assembled_maps(
    const assembled_maps_options &option) const noexcept {
  const auto transform_mat = transform_mat_of(option.map_facing);
  const auto transform_mat_abs = transform_mat.array().abs().matrix();
  const Eigen::Vector3i offset =
      (transform_mat_abs - transform_mat) / 2 *
      Eigen::Vector2i{{int(this->map_rows() - 1), int(this->map_cols() - 1)}};
  // Shape of schematic
  const Eigen::Vector3i shape = [this, transform_mat_abs]() {
    Eigen::Vector3i s =
        transform_mat_abs *
        Eigen::Vector2i{{int(this->map_rows()), int(this->map_cols())}};
    s = s.array().max(1).matrix();
    return s;
  }();

  const MCDataVersion::MCDataVersion_t data_version = [option]() {
    if (option.mc_version == SCL_gameVersion::MC20) {
      if (not option.after_1_20_5) {
        return MCDataVersion::MCDataVersion_t::Java_1_20_4;
      }
      return MCDataVersion::MCDataVersion_t::Java_1_20_5;
    } else {
      return MCDataVersion::suggested_version(option.mc_version);
    }
  }();

  libSchem::Schem schem;
  {
    std::string_view id_list[1]{std::string_view{"minecraft:air"}};
    schem.set_block_id(id_list);
    schem.resize(shape[0], shape[1], shape[2]);
    schem.fill(0);
    schem.set_MC_major_version_number(option.mc_version);
    schem.set_MC_version_number(data_version);
  }

  const libSchem::item_frame_variant variant = [&option, this]() {
    if (this->game_version < SCL_gameVersion::MC17) {
      return libSchem::item_frame_variant::common;
    }
    if (option.frame_variant == SCL_item_frame_variant::common) {
      return libSchem::item_frame_variant::common;
    } else {
      return libSchem::item_frame_variant::glowing;
    }
  }();

  for (int r = 0; r < this->map_rows(); r++) {
    for (int c = 0; c < this->map_cols(); c++) {
      const int map_index = option.begin_index + r + c * this->map_rows();
      const Eigen::Vector3i position =
          transform_mat * Eigen::Vector2i{{r, c}} + offset;
      // Check map position
      for (int dim = 0; dim < 3; dim++) {
        assert(position[dim] >= 0);
        assert(position[dim] < shape[dim]);
      }
      auto map_entity = std::make_unique<libSchem::item_frame>();
      map_entity->tile_position_ = {position[0], position[1], position[2]};
      map_entity->variant_ = variant;
      map_entity->invisible_ = option.invisible_frame;
      map_entity->fixed_ = option.fixed_frame;
      map_entity->direction_ =
          [](SCL_map_facing f) -> libSchem::hangable_facing_direction {
        switch (f) {
          case SCL_map_facing::wall_south:
            return libSchem::hangable_facing_direction::south;
          case SCL_map_facing::wall_east:
            return libSchem::hangable_facing_direction::east;
          case SCL_map_facing::wall_north:
            return libSchem::hangable_facing_direction::north;
          case SCL_map_facing::wall_west:
            return libSchem::hangable_facing_direction::west;
          case SCL_map_facing::top_north:
          case SCL_map_facing::top_south:
          case SCL_map_facing::top_east:
          case SCL_map_facing::top_west:
            return libSchem::hangable_facing_direction::top;
          case SCL_map_facing::bottom_north:
          case SCL_map_facing::bottom_south:
          case SCL_map_facing::bottom_east:
          case SCL_map_facing::bottom_west:
            return libSchem::hangable_facing_direction::bottom;
        }
        return {};
      }(option.map_facing);
      map_entity->item_rotation = rotation_of(option.map_facing);
      {
        auto map_item = std::make_unique<libSchem::filled_map>();
        map_item->map_id = map_index;
        map_entity->item_ = std::move(map_item);
      }

      schem.entity_list().emplace_back(std::move(map_entity));
    }
  }

  return schem;
}

bool converted_image_impl::export_assembled_maps_litematic(
    const char *filename, const SlopeCraft::assembled_maps_options &map_opt,
    const SlopeCraft::litematic_options &export_opt) const noexcept {
  auto schem = this->assembled_maps(map_opt);
  libSchem::litematic_info info{};
  info.litename_utf8 = export_opt.litename_utf8;
  info.regionname_utf8 = export_opt.region_name_utf8;

  auto err = schem.export_litematic(filename, info);
  if (not err) {
    export_opt.ui.report_error(err.error().first, err.error().second.c_str());
    return false;
  }
  return true;
}

bool converted_image_impl::export_assembled_maps_vanilla_structure(
    const char *filename, const SlopeCraft::assembled_maps_options &map_opt,
    const SlopeCraft::vanilla_structure_options &export_opt) const noexcept {
  auto schem = this->assembled_maps(map_opt);
  auto err = schem.export_structure(filename, export_opt.is_air_structure_void);
  if (not err) {
    export_opt.ui.report_error(err.error().first, err.error().second.c_str());
    return false;
  }
  return true;
}
