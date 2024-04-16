//
// Created by joseph on 4/12/24.
//

#ifndef SLOPECRAFT_COLORSET_H
#define SLOPECRAFT_COLORSET_H

#include <ColorManip/imageConvert.hpp>
#include <ColorManip/newColorSet.hpp>
#include <ExternalConverters/GAConverter/GAConverter.h>
#include <MapImageCvter/MapImageCvter.h>
#include <Schem/Schem.h>
#include <memory>
#include "simpleBlock.h"
#include "Colors.h"
#include "SlopeCraftL.h"

#define mapColor2Index(mapColor) (64 * ((mapColor) % 4) + ((mapColor) / 4))
#define index2mapColor(index) (4 * ((index) % 64) + ((index) / 64))
#define mapColor2baseColor(mapColor) ((mapColor) >> 2)
#define index2baseColor(index) (mapColor2baseColor(index2mapColor(index)))
#define mapColor2depth(mapColor) ((mapColor) % 4)
#define index2depth(index) (mapColor2depth(index2mapColor(index)))

struct color_set {
  static const std::unique_ptr<const colorset_basic_t> basic;

  colorset_allowed_t allowed_colorset;

  SlopeCraft::gameVersion mc_version{SCL_gameVersion::MC17};
  SlopeCraft::mapTypes map_type{SCL_mapTypes::Slope};
  std::vector<simpleBlock> palette{};
};

#endif  // SLOPECRAFT_COLORSET_H
