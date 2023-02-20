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

float color_diff_RGB(float3 RGB1, float3 RGB2);

/// Function implementations

float norm2(float3 v) { return dot(v, v); }

float deg2rad(float deg) { return deg * M_PI / 180.0f; }

float color_diff_RGB(float3 RGB1, float3 RGB2) { return norm2(RGB1 - RGB2); }

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