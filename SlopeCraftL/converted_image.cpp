//
// Created by joseph on 4/17/24.
//

#include <fmt/format.h>
#include "converted_image.h"
#include "color_table.h"
#include "height_line.h"
#include "lossyCompressor.h"

converted_image_impl::converted_image_impl(const color_table_impl &table)
    : converter{*color_set::basic, table.allowed} {}

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

std::optional<converted_image_impl::height_maps>
converted_image_impl::height_info(const build_options &option) const noexcept {
  //
  //  std::unordered_map<rc_pos, water_y_range> water_list;

  Eigen::ArrayXXi map_color = this->converter.mapcolor_matrix().cast<int>();

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
  for (int64_t c = 0; c < map_color.cols(); c++) {
    // cerr << "Coloumn " << c << '\n';
    height_line HL;
    // getTokiColorPtr(c,&src[0]);
    HL.make(map_color.col(c), allow_lossless_compress);

    if (HL.maxHeight() > option.max_allowed_height &&
        (option.compress_method == compressSettings::ForcedOnly ||
         option.compress_method == compressSettings::Both)) {
      std::vector<const TokiColor *> ptr(map_color.rows());

      this->converter.col_TokiColor_ptrs(c, ptr.data());
      // getTokiColorPtr(c, &ptr[0]);

      compressor.setSource(HL.getBase(), &ptr[0]);
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