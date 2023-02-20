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
float square(float v);
float3 square_vec3(float3 v);

float color_diff_RGB(float3 RGB1, float3 RGB2);
float color_diff_RGB_Better(float3 RGB1, float3 RGB2);
float color_diff_HSV(float3 hsv1_vec3, float3 hsv2_vec3);
float color_diff_Lab94(float3 lab1_vec3,float3 lab2_vec3);


/// Function implementations

float norm2(float3 v) { return dot(v, v); }
float sum3(float3 v) { return v[0] + v[1] + v[2]; }
float square(float s) {return s*s; }
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

float color_diff_HSV(float3 hsv1_vec3, float3 hsv2_vec3) {
  const float h1 = hsv1_vec3[0];
  const float s1 = hsv1_vec3[1];
  const float v1 = hsv1_vec3[2];

  const float h2 = hsv2_vec3[0];
  const float s2 = hsv2_vec3[1];
  const float v2 = hsv2_vec3[2];

  const float sv_1 = s1 * v1;
  const float sv_2 = s2 * v2;

  const float dX = 50.0f * (cos(h1) * sv_1 - cos(h2) - sv_2);
  const float dY = 50.0f * (sin(h1) * sv_1 - sin(h2) - sv_2);
  const float dZ = 50.0f * (v1 - v2);

  return dX * dX + dY * dY + dZ * dZ;
}


float color_diff_Lab94(float3 lab1_vec3,float3 lab2_vec3) {
  const float L1=lab1_vec3[0];
  const float a1=lab1_vec3[1];
  const float b1=lab1_vec3[2];

  const float L2=lab2_vec3[0];
  const float a2=lab2_vec3[1];
  const float b2=lab2_vec3[2];

  const float deltaL_2=square(L1-L2);
  const float C1_2=a1*a1+b1*b1;
  const float C2_2=a2*a2+b2*b2;

  const float deltaCab_2=square(sqrt(C1_2)-sqrt(C2_2));
  const float deltaHab_2=square(a2-a1)+square(b2-b1)-deltaCab_2;


  const float SC_2=square(sqrt(C1_2)*0.045f+1.0f);
  const float SH_2=square(sqrt(C2_2)*0.015f+1.0f);

  const float result=deltaL_2+deltaCab_2/SC_2+deltaHab_2/SH_2;
  return result;
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
SC_MAKE_COLORDIFF_KERNEL_FUN(match_color_HSV, color_diff_HSV)
SC_MAKE_COLORDIFF_KERNEL_FUN(match_color_Lab94, color_diff_Lab94)