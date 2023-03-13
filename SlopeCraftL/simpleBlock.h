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

typedef unsigned char uchar;

typedef std::vector<std::string> stringList;

class simpleBlock : public AbstractBlock {
public:
  simpleBlock();
  virtual ~simpleBlock(){};
  std::string id;
  uchar version;
  std::string idOld;
  bool needGlass;
  bool doGlow;
  bool endermanPickable;
  bool burnable;

  unsigned long long size() const { return sizeof(simpleBlock); }

  const char *getId() const { return id.data(); };
  unsigned char getVersion() const { return version; };
  const char *getIdOld() const { return idOld.data(); };
  bool getNeedGlass() const { return needGlass; };
  bool getDoGlow() const { return doGlow; };
  bool getEndermanPickable() const { return endermanPickable; };
  bool getBurnable() const { return burnable; };

  void setId(const char *_id) { id = _id; };
  void setVersion(unsigned char _ver) { version = _ver; };
  void setIdOld(const char *_idOld) { idOld = _idOld; };
  void setNeedGlass(bool _needGlass) { needGlass = _needGlass; };
  void setDoGlow(bool _doGlow) { doGlow = _doGlow; };
  void setEndermanPickable(bool _enderman) { endermanPickable = _enderman; };
  void setBurnable(bool _burn) { burnable = _burn; };

  void destroy() { delete this; }

  static bool dealBlockId(const std::string &id, std::string &netBlockId,
                          stringList *proName, stringList *proVal);
  // simpleBlock& operator =(const simpleBlock &);
};

#endif // SIMPLEBLOCK_H
