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

#ifndef SIMPLEBLOCK_H
#define SIMPLEBLOCK_H

#include "SCLDefines.h"
using namespace SlopeCraft;
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <memory>

typedef std::vector<std::string> stringList;

class mc_block : public ::SlopeCraft::mc_block_interface {
 public:
  mc_block();
  ~mc_block(){};
  std::string id{};
  uint8_t version{0};
  std::string idOld{};
  std::string nameZH{};
  std::string nameEN{};
  std::string imageFilename{};
  Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> image;
  bool needGlass{false};
  bool doGlow{false};
  bool endermanPickable{false};
  bool burnable{false};
  uint8_t stackSize{64};

  const char *getId() const noexcept override { return id.data(); };
  uint8_t getVersion() const noexcept override { return version; };
  const char *getIdOld() const noexcept override { return idOld.data(); };
  bool getNeedGlass() const noexcept override { return needGlass; };
  bool getDoGlow() const noexcept override { return doGlow; };
  bool getEndermanPickable() const noexcept override {
    return endermanPickable;
  };
  bool getBurnable() const noexcept override { return burnable; };
  uint8_t getStackSize() const noexcept override { return this->stackSize; }
  const char *getNameZH() const noexcept override {
    return this->nameZH.c_str();
  }
  const char *getNameEN() const noexcept override {
    return this->nameEN.c_str();
  }

  const char *getImageFilename() const noexcept override {
    return this->imageFilename.c_str();
  }

  void getImage(uint32_t *dest_row_major) const noexcept override {
    memcpy(dest_row_major, this->image.data(),
           this->image.size() * sizeof(uint32_t));
  }

  void setId(const char *_id) noexcept override { id = _id; };
  void setVersion(unsigned char _ver) noexcept override { version = _ver; };
  void setIdOld(const char *_idOld) noexcept override { idOld = _idOld; };
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

  void setNameZH(const char *__nzh) noexcept override { this->nameZH = __nzh; }
  void setNameEN(const char *__nen) noexcept override { this->nameEN = __nen; }
  void setImage(const uint32_t *src, bool is_row_major) noexcept override {
    if (is_row_major) {
      Eigen::Map<const Eigen::ArrayXX<uint32_t>> map(src, 16, 16);
      this->image = map.transpose();
    } else {
      this->image.resize(16, 16);
      memcpy(this->image.data(), src, 16 * 16 * sizeof(uint32_t));
    }
  }

  void setImageFilename(const char *_ifn) noexcept override {
    this->imageFilename = _ifn;
  }

  void copyTo(mc_block_interface *dst) const noexcept override {
    *static_cast<mc_block *>(dst) = *this;
  }

  //  static bool processBlockId(std::string_view id, std::string &netBlockId,
  //                             std::vector<std::string_view> &proName,
  //                             std::vector<std::string_view> &proVal);

  const char *idForVersion(SCL_gameVersion ver) const noexcept override {
    if (ver >= SCL_gameVersion::MC13) {
      return this->getId();
    }

    if (this->idOld.empty()) {
      return this->getId();
    }
    return this->getIdOld();
  };
  // mc_block& operator =(const mc_block &);
};

class block_list : public ::SlopeCraft::block_list_interface {
 private:
  std::map<std::unique_ptr<mc_block>, uint8_t> m_blocks;

 public:
  block_list() = default;
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

#endif  // SIMPLEBLOCK_H
