/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#include "SlopeCraftL.h"
#include "TokiSlopeCraft.h"
#include "simpleBlock.h"
#include <fmt/format.h>
#include <json.hpp>

using namespace SlopeCraft;

// AbstractBlock *AbstractBlock::create() { return new simpleBlock; }

void AbstractBlock::clear() noexcept {
  setBurnable(false);
  setDoGlow(false);
  setEndermanPickable(false);
  setId("minecraft:air");
  setIdOld("");
  setNeedGlass(false);
  setVersion(0);
  setNameZH("");
  setNameEN("");
  // setWallUseable(false);
}

Kernel::Kernel() {}

using namespace SlopeCraft;

std::pair<uint8_t, simpleBlock> parse_block(
    const nlohmann::json &jo, std::string_view image_dir) noexcept(false) {
  simpleBlock ret;
  const int basecolor = jo.at("baseColor");
  if (basecolor < 0 || basecolor >= 64) {
    throw std::runtime_error{fmt::format("Invalid base color: {}", basecolor)};
  }

  ret.id = jo.at("id");
  ret.nameZH = jo.at("nameZH");
  ret.nameEN = jo.at("nameEN");
  {
    std::string filename = image_dir.data();
    filename += '/';
    filename += jo.at("icon");
    ret.imageFilename = filename;
  }
  ret.version = jo.at("version");
  if (jo.contains("idOld")) {
    ret.idOld = jo.at("idOld");
  } else {
    ret.idOld = ret.id;
  }

  if (jo.contains("endermanPickable")) {
    ret.endermanPickable = jo.at("endermanPickable");
  }

  if (jo.contains("isGlowing")) {
    ret.doGlow = jo.at("isGlowing");
  }

  if (jo.contains("burnable")) {
    ret.burnable = jo.at("burnable");
  }

  if (jo.contains("needGlass")) {
    ret.needGlass = jo.at("needGlass");
  }

  return {basecolor, ret};
}

BlockListInterface *impl_createBlockList(const char *filename,
                                         const blockListOption &option,
                                         std::string &errmsg) noexcept {
  errmsg.reserve(4096);
  errmsg.clear();

  BlockList *bl = new BlockList;
  using njson = nlohmann::json;
  try {
    std::ifstream ifs(filename);
    njson jo = njson::parse(ifs, nullptr, true, true);

    njson::array_t arr;
    if (jo.contains("FixedBlocks")) {
      arr = std::move(jo.at("FixedBlocks"));
    } else {
      arr = std::move(jo.at("CustomBlocks"));
    }

    // parse blocks
    for (size_t idx = 0; idx < arr.size(); idx++) {
      auto temp = parse_block(arr[idx], option.image_dir);

      auto ptr = new simpleBlock;
      *ptr = std::move(temp.second);
      bl->blocks().emplace(ptr, temp.first);
    }

  } catch (std::exception &e) {
    delete bl;
    errmsg += fmt::format(
        "Exception occured when parsing blocklist json: \"{}\"\n", e.what());
    return nullptr;
  }

  // load images
  for (auto &pair : bl->blocks()) {
    pair.first->image.resize(16, 16);
    if (!option.callback_load_image(pair.first->getImageFilename(),
                                    pair.first->image.data())) {
      errmsg += fmt::format(
          "Failed to load image \"{}\", this error will be ignored.\n",
          pair.first->getImageFilename());
      continue;
    }
    pair.first->image.transposeInPlace();
  }

  return bl;
}

extern "C" {

SCL_EXPORT Kernel *SCL_createKernel() { return new TokiSlopeCraft; }
SCL_EXPORT void SCL_destroyKernel(Kernel *k) {
  delete static_cast<TokiSlopeCraft *>(k);
}

SCL_EXPORT AbstractBlock *SCL_createBlock() { return new simpleBlock; }
SCL_EXPORT void SCL_destroyBlock(AbstractBlock *b) { delete b; }

SCL_EXPORT BlockListInterface *SCL_createBlockList(
    const char *filename, const blockListOption &option) {
  const bool can_write_err =
      (option.errmsg != nullptr) && (option.errmsg_capacity > 0);

  std::string errmsg;

  BlockListInterface *ret = impl_createBlockList(filename, option, errmsg);

  if (can_write_err) {
    memset(option.errmsg, 0, option.errmsg_capacity);
    const size_t copy_len = std::min(option.errmsg_capacity, errmsg.size());

    memcpy(option.errmsg, errmsg.c_str(), copy_len);
    if (option.errmsg_len_dest != nullptr) {
      *option.errmsg_len_dest = copy_len;
    }
  } else {
    if (option.errmsg_len_dest != nullptr) {
      *option.errmsg_len_dest = 0;
    }
  }

  return ret;
}

SCL_EXPORT void SCL_destroyBlockList(BlockListInterface *) {}

SCL_EXPORT AiCvterOpt *SCL_createAiCvterOpt() { return new AiCvterOpt; }
void SCL_EXPORT SCL_destroyAiCvterOpt(AiCvterOpt *a) { delete a; }

void SCL_EXPORT SCL_setPopSize(AiCvterOpt *a, unsigned int p) {
  a->popSize = p;
}
void SCL_EXPORT SCL_setMaxGeneration(AiCvterOpt *a, unsigned int p) {
  a->maxGeneration = p;
}
void SCL_EXPORT SCL_setMaxFailTimes(AiCvterOpt *a, unsigned int p) {
  a->maxFailTimes = p;
}
void SCL_EXPORT SCL_setCrossoverProb(AiCvterOpt *a, double p) {
  a->crossoverProb = p;
}
void SCL_EXPORT SCL_setMutationProb(AiCvterOpt *a, double p) {
  a->mutationProb = p;
}

unsigned int SCL_EXPORT SCL_getPopSize(const AiCvterOpt *a) {
  return a->popSize;
}
unsigned int SCL_EXPORT SCL_getMaxGeneration(const AiCvterOpt *a) {
  return a->maxGeneration;
}
unsigned int SCL_EXPORT SCL_getMaxFailTimes(const AiCvterOpt *a) {
  return a->maxFailTimes;
}
double SCL_EXPORT SCL_getCrossoverProb(const AiCvterOpt *a) {
  return a->crossoverProb;
}
double SCL_EXPORT SCL_getMutationProb(const AiCvterOpt *a) {
  return a->mutationProb;
}

SCL_EXPORT void SCL_getColorMapPtrs(const float **const rdata,
                                    const float **const gdata,
                                    const float **const bdata,
                                    const uint8_t **mapdata, int *num) {
  TokiSlopeCraft::getColorMapPtrs(rdata, gdata, bdata, mapdata, num);
}

SCL_EXPORT const float *SCL_getBasicColorMapPtrs() {
  return TokiSlopeCraft::getBasicColorMapPtrs();
}

SCL_EXPORT const char *SCL_getSCLVersion() {
  return TokiSlopeCraft::getSCLVersion();
}

SCL_EXPORT SCL_gameVersion SCL_basecolor_version(uint8_t basecolor) {
  if (basecolor <= 51) {
    return SCL_gameVersion::ANCIENT;
  }

  if (basecolor <= 58) {
    return SCL_gameVersion::MC16;
  }

  if (basecolor <= 61) {
    return SCL_gameVersion::MC17;
  }
  return SCL_gameVersion::FUTURE;
}

SCL_EXPORT uint8_t SCL_maxBaseColor() { return 61; }

SCL_EXPORT int SCL_getBlockPalette(const AbstractBlock **blkpp,
                                   size_t capacity) {
  return TokiSlopeCraft::getBlockPalette(blkpp, capacity);
}
}
