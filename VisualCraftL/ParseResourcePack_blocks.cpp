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
    point = {0.5, pos_max[1], 0.5};
    break;
  case face_y_neg:
    point = {0.5, pos_min[1], 0.5};
    break;

  case face_x_pos:
    point = {pos_max[0], 0.5, 0.5};
    break;
  case face_x_neg:
    point = {pos_min[0], 0.5, 0.5};
    break;

  case face_z_pos:
    point = {0.5, 0.5, pos_max[2]};
    break;
  case face_z_neg:
    point = {0.5, 0.5, pos_min[2]};
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
  if (dest == nullptr)
    return;

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

  if (this->face(f).is_hidden)
    return;

  switch (f) {
  case face_x_neg:
  case face_x_pos:
    if (this->x_range_abs() < 1e-4f)
      return;
  case face_y_neg:
  case face_y_pos:
    if (this->y_range_abs() < 1e-4f)
      return;
  case face_z_neg:
  case face_z_pos:
    if (this->z_range_abs() < 1e-4f)
      return;
  }

  intersect_point intersect;
  Array3f coordinate;
  coordinate = crossover_point(this->plane(f), ray);
  // printf("\nelement::intersect_points : coordinate = [%f, %f,
  // %f]",coordinate[0], coordinate[1], coordinate[2]);
  if (!this->is_not_outside(coordinate))
    return;

  intersect.face_ptr = &this->face(f);

  const Array3f min_pos = this->xyz_minpos();
  const Array3f max_pos = this->xyz_maxpos();
  /*
  printf("\nelement::intersect_points : ");
  printf("min_pos = [%f, %f, %f]; ", min_pos[0], min_pos[1], min_pos[2]);
  printf("max_pos = [%f, %f, %f]; ", max_pos[0], max_pos[1], max_pos[2]);

  */

  // u is col and v is row
  Array3f uv_start;
  // #warning compute uv here

  switch (f) {
  case face_idx::face_up:
    uv_start = {min_pos[0], max_pos[1], min_pos[2]};
    // uv_end=max_pos;

    // here u <-> x+
    intersect.uv[0] = (coordinate[0] - uv_start[0]) / this->x_range_abs();
    // here v<-> z+
    intersect.uv[1] = (coordinate[2] - uv_start[2]) / this->z_range_abs();
    /*
    printf("\nelement::intersect_points : face = up, uv_start = [%f, %f, %f], "
           "uv = [%f, %f] ",
           uv_start[0], uv_start[1], uv_start[2], intersect.uv[0],
           intersect.uv[1]);

           */

    break;

  case face_idx::face_down:
    uv_start = {max_pos[0], min_pos[1], max_pos[2]};
    {
      Array3f uv_end = {min_pos[0], min_pos[1], min_pos[2]};
      //   here u <-> x+
      intersect.uv[0] = (coordinate[0] - uv_end[0]) / this->x_range_abs();
      // here v <-> z-
      intersect.uv[1] = (uv_start[2] - coordinate[2]) / this->z_range_abs();
    }
    break;

  case face_idx::face_east:
    // uv_start=max_pos;
    uv_start = {max_pos[0], max_pos[1], max_pos[2]};
    // uv_end = {max_pos[0], min_pos[1], min_pos[2]};
    //  here u <-> z-
    intersect.uv[0] = (uv_start[2] - coordinate[2]) / this->z_range_abs();
    // here v <-> y-
    intersect.uv[1] = (uv_start[1] - coordinate[1]) / this->y_range_abs();
    break;

  case face_idx::face_west:
    uv_start = {min_pos[0], max_pos[1], min_pos[2]};
    // uv_end = {min_pos[0], min_pos[1], max_pos[2]};

    // here u <-> z+
    intersect.uv[0] = (coordinate[2] - uv_start[2]) / this->z_range_abs();
    // here v <-> y-
    intersect.uv[1] = (uv_start[1] - coordinate[1]) / this->y_range_abs();
    break;

  case face_idx::face_south:
    uv_start = {min_pos[0], max_pos[1], max_pos[2]};
    // uv_end = {max_pos[0], min_pos[1], max_pos[2]};

    // here u <-> x+
    intersect.uv[0] = (coordinate[0] - uv_start[0]) / this->x_range_abs();
    // here v <-> y-
    intersect.uv[1] = (uv_start[1] - coordinate[1]) / this->y_range_abs();
    break;

  case face_idx::face_north:
    uv_start = {max_pos[0], max_pos[0], min_pos[0]};
    // uv_end = min_pos;

    // here u <-> x-
    intersect.uv[0] = (uv_start[0] - coordinate[0]) / this->x_range_abs();
    // here v <-> y-
    intersect.uv[1] = (uv_start[1] - coordinate[1]) / this->y_range_abs();
    break;
  }

  for (auto &uv : intersect.uv) {
    uv = std::max<float>(std::min<float>(uv, 16), 0);
  }

  switch (intersect.face_ptr->rot) {
  case face_rot::face_rot_0:
    break;
  case face_rot::face_rot_90: {
    float temp_u = intersect.uv[0];
    intersect.uv[0] = intersect.uv[1];
    intersect.uv[1] = 1 - temp_u;
  } break;

  case face_rot::face_rot_180:
    intersect.uv[0] = 1 - intersect.uv[0];
    intersect.uv[1] = 1 - intersect.uv[1];
    break;

  case face_rot::face_rot_270: {
    float temp_u = intersect.uv[0];
    intersect.uv[0] = 1 - intersect.uv[1];
    intersect.uv[1] = temp_u;
    break;
  }
  }

  intersect.distance = (coordinate - ray.x0y0z0).square().sum();

  dest->emplace_back(intersect);
}

inline bool intersect_compare_fun(const intersect_point &a,
                                  const intersect_point &b) noexcept {
  return a.distance < b.distance;
}

void model::projection_image(face_idx fidx,
                             EImgRowMajor_t *const dest) const noexcept {
  dest->resize(16, 16);
  dest->fill(0x00000000);
  dest->fill(0x00000000);

  std::vector<intersect_point> intersects;
  intersects.reserve(this->elements.size() * 2);

  ray_t ray(fidx);
  for (int r = 0; r < 16; r++) {
    for (int c = 0; c < 16; c++) {

      // printf("\nr = %i, c = %i, : ", r, c);

      intersects.clear();

      // set the origin point of a ray
      switch (fidx) {
      case face_idx::face_up:
        ray.x0y0z0 = {c + 0.5f, 128.0f, r + 0.5f};
        break;
      case face_idx::face_down:
        ray.x0y0z0 = {c + 0.5f, -128.0f, 15.5f - r};
        break;

      case face_idx::face_east:
        // r->y-, c->z-,x=128
        ray.x0y0z0 = {128.0f, 15.5f - r, 15.5f - c};
        break;
      case face_idx::face_west:
        // r->y-, c->z+, x=-128
        ray.x0y0z0 = {-128.0f, 15.5f - r, c + 0.5f};
        break;
      case face_idx::face_south:
        // r->y-, c->x+, z=128
        ray.x0y0z0 = {c + 0.5f, 15.5f - r, 128.0f};
        break;
      case face_idx::face_north:
        // r->y-, c->x-, z=-128
        ray.x0y0z0 = {15.5f - c, 15.5f - r, -128.0f};
        break;
      }

      for (const element &ele : this->elements) {

        ele.intersect_points(fidx, ray, &intersects);
        ele.intersect_points(inverse_face(fidx), ray, &intersects);
      }

      // printf("\n");

      std::sort(intersects.begin(), intersects.end(), intersect_compare_fun);

      // printf("\nmodel::projection_image : r = %i, c = %i; ", r, c);
      /*printf("intersects distance = [ ");
      for (auto i : intersects) {
        printf("%f, ", i.distance);
      }
      printf("];");
      */

      ARGB color = 0x00000000;
      // printf("\n color : ");
      for (intersect_point &ip : intersects) {
        // printf("\n0x%08X + 0x%08X -> ", color, ip.color());
        color = ComposeColor_background_half_transparent(color, ip.color());
        // printf("0x%08X; ", color);
        if (getA(color) >= 255)
          break;
      }
      // printf(";");

      dest->operator()(r, c) = color;
    }
  }
}

EImgRowMajor_t model::projection_image(face_idx fidx) const noexcept {
  EImgRowMajor_t result(16, 16);

  this->projection_image(fidx, &result);

  return result;
}