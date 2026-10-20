/*
 Copyright © 2021-2026  TokiNoBug
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

#ifndef SIMPLEBLOCK_H
#define SIMPLEBLOCK_H

#include <iostream>
#include <optional>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <memory>

#include <version_set.hpp>
#include "SCLDefines.h"

#include <mc_schem.hpp>

using namespace SlopeCraft;

typedef std::vector<std::string> stringList;

struct block_detail_info {
  std::string id_namespace;
  std::string pure_id;

  [[nodiscard]] inline bool operator==(
      const block_detail_info& b) const noexcept {
    return (this->id_namespace == b.id_namespace) and
           (this->pure_id == b.pure_id);
  }
};

template <>
struct std::hash<block_detail_info> {
  [[nodiscard]] static inline size_t operator()(
      const block_detail_info& info) noexcept {
    size_t hash = std::hash<std::string_view>{}(info.id_namespace);
    hash ^= std::hash<std::string_view>{}(info.pure_id);
    return hash;
  }
};

class mc_block : public ::SlopeCraft::mc_block_interface {
 public:
  mc_block();
  ~mc_block() override = default;
  std::string id{};
  std::string idOld{};
  std::string nameZH{};
  std::string nameEN{};
  std::string imageFilename{};
  Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> image;
  version_set needStone{0};
  SCL_gameVersion version{0};
  bool needGlass{false};
  bool doGlow{false};
  bool endermanPickable{false};
  bool burnable{false};
  uint8_t stackSize{64};

  [[nodiscard]] std::optional<block_detail_info> detail_info(
      SCL_gameVersion current_version) const;

  const char* getId() const noexcept override { return id.data(); };
  SCL_gameVersion getVersion() const noexcept override { return version; };
  const char* getIdOld() const noexcept override { return idOld.data(); };
  bool getNeedGlass() const noexcept override { return needGlass; };
  bool getDoGlow() const noexcept override { return doGlow; };
  bool getEndermanPickable() const noexcept override {
    return endermanPickable;
  };
  bool getBurnable() const noexcept override { return burnable; };
  uint8_t getStackSize() const noexcept override { return this->stackSize; }
  const char* getNameZH() const noexcept override {
    return this->nameZH.c_str();
  }
  const char* getNameEN() const noexcept override {
    return this->nameEN.c_str();
  }

  const char* getImageFilename() const noexcept override {
    return this->imageFilename.c_str();
  }

  void getImage(uint32_t* dest_row_major) const noexcept override {
    memcpy(dest_row_major, this->image.data(),
           this->image.size() * sizeof(uint32_t));
  }
  bool getNeedStone(SCL_gameVersion v) const noexcept override {
    return this->needStone[v];
  }

  void setId(const char* _id) noexcept override { id = _id; };
  void setVersion(SCL_gameVersion _ver) noexcept override { version = _ver; };
  void setIdOld(const char* _idOld) noexcept override { idOld = _idOld; };
  void setNeedGlass(bool _needGlass) noexcept override {
    needGlass = _needGlass;
  };
  void setDoGlow(bool _doGlow) noexcept override { doGlow = _doGlow; };
  void setEndermanPickable(bool _enderman) noexcept override {
    endermanPickable = _enderman;
  };
  void setBurnable(bool _burn) noexcept override { burnable = _burn; };
  void setStackSize(uint8_t stack_size) noexcept override {
    this->stackSize = std::max<uint8_t>(stack_size, 1);
  }
  void setNeedStone(SCL_gameVersion v, bool ns) noexcept override {
    this->needStone[v] = ns;
  }

  void setNameZH(const char* nzh) noexcept override { this->nameZH = nzh; }
  void setNameEN(const char* nzh) noexcept override { this->nameEN = nzh; }
  void setImage(const uint32_t* src, bool is_row_major) noexcept override {
    if (is_row_major) {
      Eigen::Map<const Eigen::ArrayXX<uint32_t>> map(src, 16, 16);
      this->image = map.transpose();
    } else {
      this->image.resize(16, 16);
      memcpy(this->image.data(), src, 16 * 16 * sizeof(uint32_t));
    }
  }

  void setImageFilename(const char* _ifn) noexcept override {
    this->imageFilename = _ifn;
  }

  void copyTo(mc_block_interface* dst) const noexcept override {
    *static_cast<mc_block*>(dst) = *this;
  }

  const char* idForVersion(SCL_gameVersion ver) const noexcept override {
    if (ver >= SCL_gameVersion::MC13) {
      return this->getId();
    }

    if (this->idOld.empty()) {
      return this->getId();
    }
    return this->getIdOld();
  }

  [[nodiscard]] mc_schem::unique_block to_mc_schem_block(
      SCL_gameVersion ver) const;
};

#endif  // SIMPLEBLOCK_H
