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
float sum3(float3 v);
float square(float v);
float3 square_vec3(float3 v);

float color_diff_RGB(float3 RGB1, float3 RGB2);
float color_diff_RGB_Better(float3 RGB1, float3 RGB2);
float color_diff_HSV(float3 hsv1_vec3, float3 hsv2_vec3);
float color_diff_Lab94(float3 lab1_vec3,float3 lab2_vec3);
float color_diff_Lab00(float3 lab1_vec3,float3 lab2_vec3);


/// Function implementations

float norm2(float3 v) { return dot(v, v); }
float sum3(float3 v) { return v[0] + v[1] + v[2]; }
float square(float s) {return s*s; }
float3 square_vec3(float3 v) { return v * v; }

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


float color_diff_Lab00(float3 lab1_vec3,float3 lab2_vec3) {
  
  const float kL = 1.0;
  const float kC = 1.0;
  const float kH = 1.0;
  const float L1=lab1_vec3[0];
  const float a1=lab1_vec3[1];
  const float b1=lab1_vec3[2];

  const float L2=lab2_vec3[0];
  const float a2=lab2_vec3[1];
  const float b2=lab2_vec3[2];

  
  float C1sab = sqrt(a1 * a1 + b1 * b1);
  float C2sab = sqrt(a2 * a2 + b2 * b2);
  float mCsab = (C1sab + C2sab) / 2;
  float pow_mCsab_7 = pow(mCsab, 7);
  float G =
      0.5 * (1 - sqrt(pow_mCsab_7 / (pow_mCsab_7 + pow(25.0f, 7.0f))));
  float a1p = (1 + G) * a1;
  float a2p = (1 + G) * a2;
  float C1p = sqrt(a1p * a1p + b1 * b1);
  float C2p = sqrt(a2p * a2p + b2 * b2);
  float h1p, h2p;
  if (b1 == 0 && a1p == 0)
    h1p = 0;
  else
    h1p = atan2(b1, a1p);
  if (h1p < 0)
    h1p += 2 * M_PI;

  if (b2 == 0 && a2p == 0)
    h2p = 0;
  else
    h2p = atan2(b2, a2p);
  if (h2p < 0)
    h2p += 2 * M_PI;

  float dLp = L2 - L1;
  float dCp = C2p - C1p;
  float dhp;
  if (C1p * C2p == 0) {
    dhp = 0;
  } else {
    if (fabs(h2p - h1p) <= radians(180.0f)) {
      dhp = h2p - h1p;
    } else if (h2p - h1p > radians(180.0f)) {
      dhp = h2p - h1p - radians(360.0f);
    } else {
      dhp = h2p - h1p + radians(360.0f);
    }
  }

  float dHp = 2 * sqrt(C1p * C2p) * sin(dhp / 2.0);

  float mLp = (L1 + L2) / 2;
  float mCp = (C1p + C2p) / 2;
  float mhp;
  if (C1p * C2p == 0) {
    mhp = (h1p + h2p);
  } else if (fabs(h2p - h1p) <= radians(180.0f)) {
    mhp = (h1p + h2p) / 2;
  } else if (h1p + h2p < radians(360.0f)) {
    mhp = (h1p + h2p + radians(360.0f)) / 2;
  } else {
    mhp = (h1p + h2p - radians(360.0f)) / 2;
  }

  float T = 1 - 0.17 * cos(mhp - radians(30.0f)) + 0.24 * cos(2 * mhp) +
            0.32 * cos(3 * mhp + radians(6.0f)) -
            0.20 * cos(4 * mhp - radians(63.0f));

  float dTheta = radians(30.0f) * exp(-square((mhp - radians(275.0f)) / radians(25.0f)));

  float RC = 2 * sqrt(pow(mCp, 7) / (pow(25.0f, 7.0f) + pow(mCp, 7.0f)));
  float square_mLp_minus_50 = square(mLp - 50);
  float SL = 1 + 0.015 * square_mLp_minus_50 / sqrt(20 + square_mLp_minus_50);

  float SC = 1 + 0.045 * mCp;

  float SH = 1 + 0.015 * mCp * T;

  float RT = -RC * sin(2 * dTheta);

  float Diffsquare = square(dLp / SL / kL) + square(dCp / SC / kC) +
                     square(dHp / SH / kH) +
                     RT * (dCp / SC / kC) * (dHp / SH / kH);

                     
  return Diffsquare;
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
SC_MAKE_COLORDIFF_KERNEL_FUN(match_color_Lab00, color_diff_Lab00)