#include "ParseResourcePack.h"
#include "VisualCraftL.h"

#include <iomanip>
#include <iostream>

using std::cout, std::endl;

int print_rotate_sheet();

int main(int argc, char **argv) { return print_rotate_sheet(); }

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