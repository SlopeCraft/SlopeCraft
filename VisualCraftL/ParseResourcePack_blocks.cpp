#include "ParseResourcePack.h"

using namespace block_model;
using Array3f = ::Eigen::Array3f;

ray_t::ray_t(const face_idx f) {
  switch (f) {
  case face_idx::face_down:
  case face_idx::face_up:
    abc = {0, 1, 0};
    break;

  case face_idx::face_north:
  case face_idx::face_south:
    abc = {0, 0, 1};
    break;

  case face_idx::face_east:
  case face_idx::face_west:
    abc = {1, 0, 0};
    break;
  }
}

plane_t element::plane(face_idx fi) const noexcept {
  Array3f norm;
  Array3f point;

  Array3f pos_min = this->_to.min(this->_from);
  Array3f pos_max = this->_to.max(this->_from);

  switch (fi) {
  case face_y_pos:
    point = {0.5, 1, 0.5};
    break;
  case face_y_neg:
    point = {0.5, 0, 0.5};
    break;

  case face_x_pos:
    point = {1, 0.5, 0.5};
    break;
  case face_x_neg:
    point = {0, 0.5, 0.5};
    break;

  case face_z_pos:
    point = {0.5, 0.5, 1};
    break;
  case face_z_neg:
    point = {0.5, 0.5, 0};
    break;
  }

  switch (fi) {
  case face_y_pos:
  case face_y_neg:
    norm = {0, 1, 0};
    break;

  case face_x_pos:
  case face_x_neg:
    norm = {1, 0, 0};
    break;

  case face_z_pos:
  case face_z_neg:
    norm = {0, 0, 1};
    break;
  }

  return plane_t(norm, point);
}

constexpr float near_zero = 1e-10f;

Array3f crossover_point(const plane_t &plane, const ray_t &ray) noexcept {
  const Array3f Aa_Bb_Cc = plane.ABC * ray.abc;
  const float Aa_Bb_Cc_sum = Aa_Bb_Cc.sum();

  if (std::abs(Aa_Bb_Cc_sum) < near_zero) {
    Array3f nan;
    nan.fill(std::nanf(nullptr));
    return nan;
  }

  const float Aa = Aa_Bb_Cc[0], Bb = Aa_Bb_Cc[1], Cc = Aa_Bb_Cc[2];

  const Array3f Ax0_By0_Cz0 = plane.ABC * ray.x0y0z0;

  const float Ax0 = Ax0_By0_Cz0[0], By0 = Ax0_By0_Cz0[1], Cz0 = Ax0_By0_Cz0[2];

  Array3f result;
  result[0] = (Bb + Cc) * ray.x0y0z0[0] - ray.abc[0] * (By0 + Cz0 + plane.D);
  result[1] = (Aa + Cc) * ray.x0y0z0[1] - ray.abc[1] * (Ax0 + Cz0 + plane.D);
  result[2] = (Aa + Bb) * ray.x0y0z0[2] - ray.abc[2] * (Ax0 + By0 + plane.D);

  result /= Aa_Bb_Cc_sum;

  return result;
}

void element::intersect_points(
    const face_idx f, const ray_t &ray,
    std::vector<intersect_point> *const dest) const noexcept {
  if (dest == nullptr) {
    return;
  }

  const Array3f &start_point = ray.x0y0z0;
  /*  switch (f) {
    case face_x_pos:
      start_point = {128, 0, 0};
      break;
    case face_x_neg:
      start_point = {-128, 0, 0};
      break;

    case face_y_pos:
      start_point = {0, 128, 0};
      break;
    case face_y_neg:
      start_point = {0, -128, 0};
      break;

    case face_z_pos:
      start_point = {0, 0, 128};
      break;
    case face_z_neg:
      start_point = {0, 0, -128};
      break;
    }
  */

  intersect_point intersect;
  if (!this->face(f).is_hidden) {
    intersect.coordinate = crossover_point(this->plane(f), ray);
    intersect.face_ptr = &this->face(f);
#warning compute uv here
  }
}