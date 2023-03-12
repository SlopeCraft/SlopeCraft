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

#ifndef SCL_UIPACK_UIPACK_H
#define SCL_UIPACK_UIPACK_H

struct uiPack {
public:
  void *_uiPtr{nullptr};
  void (*progressRangeSet)(void *, int, int, int){nullptr};
  void (*progressAdd)(void *, int){nullptr};

public:
  inline void rangeSet(int a, int b, int c) const noexcept {
    if (progressRangeSet != nullptr)
      progressRangeSet(_uiPtr, a, b, c);
  }
  inline void add(int d) const noexcept {
    if (progressAdd != nullptr)
      progressAdd(_uiPtr, d);
  }
};

#endif // SCL_UIPACK_UIPACK_H
