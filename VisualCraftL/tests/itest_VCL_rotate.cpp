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

#include "ParseResourcePack.h"
#include "VisualCraftL.h"

#include <iomanip>
#include <iostream>

#include <CLI11.hpp>

using std::cout, std::endl;

int print_rotate_sheet();

int rotate_arr3f(int argc, const char *const *const argv);

int main(int, char **) {
  return print_rotate_sheet();
  // return rotate_arr3f(argc, argv);
}

int print_rotate_sheet() {
  using namespace block_model;
  const std::array<face_rot, 4> rots{
      face_rot::face_rot_0, face_rot::face_rot_90, face_rot::face_rot_180,
      face_rot::face_rot_270};

  const std::array<face_idx, 6> faces{
      face_idx::face_up,    face_idx::face_down, face_idx::face_north,
      face_idx::face_south, face_idx::face_east, face_idx::face_west};

  constexpr int width = 12;
  cout << std::setw(width) << "";
  cout << " | ";
  for (int face_i = 0; face_i < 6; face_i++) {
    cout << std::setw(width) << face_idx_to_string(faces[face_i]);
    cout << " | ";
  }
  cout << endl;

  for (int i = 0; i < 104; i++) {
    cout << '-';
  }

  cout << endl;
  std::string buf;
  for (int rx = 0; rx < 4; rx++) {
    for (int ry = 0; ry < 4; ry++) {
      buf.clear();
      buf.push_back('(');
      buf += std::to_string(int(rots[rx]) * 10);
      buf.push_back(',');
      buf += std::to_string(int(rots[ry]) * 10);
      buf.push_back(')');
      cout << std::setw(width) << buf;
      cout << " | ";

      for (int face_i = 0; face_i < 6; face_i++) {
        cout << std::setw(width)
             << (int)(rotate(faces[face_i], rots[rx], rots[ry]));
        cout << " | ";
      }
      cout << endl;
    }
  }

  cout << endl;

  return 0;
}

int rotate_arr3f(int argc, const char *const *const argv) {
  std::array<float, 3> __from, __to;
  int __rot_x = 0, __rot_y = 0;
  CLI::App app;

  app.add_option("--from", __from)->expected(1)->required();
  app.add_option("--to", __to)->expected(1)->required();

  app.add_option("--rotx", __rot_x)
      ->required()
      ->check(CLI::IsMember({0, 90, 180, 270}));
  app.add_option("--roty", __rot_y)
      ->required()
      ->check(CLI::IsMember({0, 90, 180, 270}));

  CLI11_PARSE(app, argc, argv);

  Eigen::Array3f from(__from.data()), to(__to.data());

  const block_model::face_rot rot_x = block_model::face_rot(__rot_x / 10);
  const block_model::face_rot rot_y = block_model::face_rot(__rot_y / 10);

  from = block_model::rotate(from, rot_x, rot_y);
  to = block_model::rotate(to, rot_x, rot_y);

  cout << "after rotation : from = [" << from.transpose() << "], to = ["
       << to.transpose() << ']' << endl;

  cout << "min = [" << from.min(to).transpose() << "] , max = ["
       << from.max(to).transpose() << ']' << endl;

  return 0;
}