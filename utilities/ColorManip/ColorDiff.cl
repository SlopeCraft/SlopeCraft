/*
__kernel void match_color_test(__global const float3 *color_avaliable_ptr,
                               const ushort color_avaliable_num_ptr,
                               __global const uint *colors_unconverted,
                               __global ushort *result_idx_dst,
                               __global float *result_diff_dst) {

  // const float p=color_avaliable_num_ptr[0];
  return;
}
*/

/// Function definations

// compute sum(v*v)
float norm2(float3 v);
float deg2rad(float deg);
float sum3(float3 v);

float color_diff_RGB(float3 RGB1, float3 RGB2);
float color_diff_RGB_Better(float3 RGB1, float3 RGB2);

/// Function implementations

float norm2(float3 v) { return dot(v, v); }
float sum3(float3 v) { return v[0] + v[1] + v[2]; }
float3 square_vec3(float3 v) { return v * v; }

float deg2rad(float deg) { return deg * M_PI / 180.0f; }

float color_diff_RGB(float3 RGB1, float3 RGB2) { return norm2(RGB1 - RGB2); }

float color_diff_RGB_Better(float3 rgb1, float3 rgb2) {

  const float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;
  const float3 w_vec3 = {w_r, w_g, w_b};
  const float thre = 1e-10f;
  const float3 one_vec3 = {1, 1, 1};

  const float SqrModSquare = norm2(rgb1) * norm2(rgb2);

  const float3 delta_rgb_vec3 = rgb1 - rgb2;

  const float SigmaRGB = (sum3(rgb1) + sum3(rgb2)) / 3.0f;
  const float3 S_rgb_vec3 = fmin((rgb1 + rgb2) / (SigmaRGB + thre), one_vec3);

  const float sumRGBSquare = dot(rgb1, rgb2);

  const float theta =
      2.0f / M_PI * acos(sumRGBSquare / (SqrModSquare + thre) / 1.01f);

  const float3 OnedDelta_rgb_vec3 = fabs(delta_rgb_vec3) / (rgb1 + rgb2 + thre);

  const float sumOnedDelta = sum3(OnedDelta_rgb_vec3);

  const float3 S_t_rgb_vec3 =
      OnedDelta_rgb_vec3 / sumOnedDelta * S_rgb_vec3 * S_rgb_vec3;

  const float S_theta = sum3(S_t_rgb_vec3);

  const float3 rgb_max_vec3 = fmax(rgb1, rgb2);
  const float S_ratio =
      fmax(rgb_max_vec3[0], fmax(rgb_max_vec3[1], rgb_max_vec3[2]));

  const float3 SS_w_delta_delta_vec3 =
      S_rgb_vec3 * S_rgb_vec3 * delta_rgb_vec3 * delta_rgb_vec3 * w_vec3;

  const float part1 = sum3(SS_w_delta_delta_vec3) / sum3(w_vec3);

  const float part2 = S_theta * S_ratio * theta * theta;

  return part1 + part2;
}

#define SC_MAKE_COLORDIFF_KERNEL_FUN(kfun_name, diff_fun)                      \
  __kernel void kfun_name(                                                     \
      __global const float3 *colorset_colors, const ushort colorset_size,      \
      __global const float3 *unconverted_colors,                               \
      __global ushort *result_idx_dst, __global float *result_diff_dst) {      \
    const size_t global_idx = get_global_id(0);                                \
    const float3 unconverted = unconverted_colors[global_idx];                 \
                                                                               \
    ushort result_idx = 0;                                                     \
    float result_diff = FLT_MAX / 2;                                           \
                                                                               \
    for (ushort idx = 0; idx < colorset_size; idx++) {                         \
      const float3 color_ava = colorset_colors[idx];                           \
                                                                               \
      const float diff_sq = diff_fun(color_ava, unconverted);                  \
      if (result_diff < diff_sq) {                                             \
        /* this branch may be optimized */                                     \
        result_idx = idx;                                                      \
        result_diff = diff_sq;                                                 \
      }                                                                        \
    }                                                                          \
                                                                               \
    result_idx_dst[global_idx] = result_idx;                                   \
    result_diff_dst[global_idx] = result_diff;                                 \
  }

SC_MAKE_COLORDIFF_KERNEL_FUN(match_color_RGB, color_diff_RGB)
SC_MAKE_COLORDIFF_KERNEL_FUN(match_color_RGB_Better, color_diff_RGB_Better)