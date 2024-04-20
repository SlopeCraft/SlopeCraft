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

converted_image_impl::converted_image_impl(const color_table_impl &table)
    : converter{*SlopeCraft::basic_colorset, table.allowed},
      game_version{table.mc_version()} {}

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

bool converted_image_impl::export_map_data(
    const SlopeCraft::map_data_file_options &option) const noexcept {
  const std::filesystem::path dir{option.folder_path};
  const auto mapPic = this->converter.mapcolor_matrix();
  const int rows = ceil(mapPic.rows() / 128.0f);
  const int cols = ceil(mapPic.cols() / 128.0f);
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

#warning This may be incorrect
  const bool allow_lossless_compress =
      int(option.compress_method) & int(SCL_compressSettings::NaturalOnly);

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
  base.setZero(this->rows(), this->cols());
  high_map.setZero(this->rows(), this->cols());
  low_map.setZero(this->rows(), this->cols());
  std::unordered_map<rc_pos, water_y_range> water_list;

  lossy_compressor compressor;
  compressor.ui = option.ui;
  compressor.progress_bar = option.sub_progressbar;
  for (int64_t c = 0; c < map_color.cols(); c++) {
    // cerr << "Coloumn " << c << '\n';
    height_line HL;
    // getTokiColorPtr(c,&src[0]);
    HL.make(map_color.col(c), allow_lossless_compress);

    if (HL.maxHeight() > option.max_allowed_height &&
        (option.compress_method == compressSettings::ForcedOnly ||
         option.compress_method == compressSettings::Both)) {
      std::vector<const TokiColor *> ptr(map_color.rows());

      this->converter.col_TokiColor_ptrs(c, ptr);
      // getTokiColorPtr(c, &ptr[0]);

      compressor.setSource(HL.getBase(), ptr);
      bool success = compressor.compress(option.max_allowed_height,
                                         allow_lossless_compress);
      if (!success) {
        option.ui.report_error(
            SCL_errorFlag::LOSSYCOMPRESS_FAILED,
            fmt::format("Failed to compress the 3D structure at column {}.", c)
                .data());
        return std::nullopt;
      }
      Eigen::ArrayXi temp;
      HL.make(&ptr[0], compressor.getResult(), allow_lossless_compress, &temp);
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

// uint64_t converted_image_impl::hash() const noexcept {
//   return this->converter.task_hash();
// }