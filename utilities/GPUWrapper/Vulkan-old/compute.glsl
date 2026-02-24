#version 450
//#extension GL_EXT_shader_8bit_storage: enable
#extension GL_EXT_shader_16bit_storage: enable

#extension GL_EXT_debug_printf: enable

#define NAN float(0.0f / 0.0f)
#define pi_fp32 float(radians(180))

//struct task_info {
//    uint task_num;
//    uint colorset_size;
//    //uint16_t algo;
//};

const uint ALGO_RGB = 114;
const uint ALGO_RGB_BETTER = 82;
const uint ALGO_HSV = 72;
const uint ALGO_LAB94 = 108;
const uint ALGO_LAB00 = 76;
const uint ALGO_XYZ = 88;

const bool SC_OCL_SPOT_NAN = true;

layout (local_size_x = 64, local_size_y = 1, local_size_z = 1) in;

layout (std140, binding = 0) readonly buffer bufOpt {
    uint task_num;
    uint colorset_size;
    uint algo;
} option;
layout (binding = 1) readonly buffer bufCC {
    float colorset_colors[];
};
layout (binding = 2) readonly buffer bufUC {
    float unconverted_colors[];
};
layout (binding = 3) writeonly buffer bufRDD {
    float result_diff_dst[];
};
layout (binding = 4) writeonly buffer bufRID {
    uint16_t result_idx_dst[];
};


bool have_nan(vec3 v) {
    bool ret = false;
    for (uint i = 0; i < 3; i++) {
        ret = ret || isnan(v[i]);
    }
    return ret;
}

float norm2(vec3 v) { return dot(v, v); }
float sum3(vec3 v) { return v[0] + v[1] + v[2]; }
float square(float s) { return s * s; }
vec3 square_vec3(vec3 v) { return v * v; }



float color_diff_RGB_XYZ(vec3 RGB1, vec3 RGB2);
float color_diff_RGB_Better(vec3 rgb1, vec3 rgb2);
float color_diff_HSV(vec3 hsv1_vec3, vec3 hsv2_vec3);
float color_diff_Lab94(vec3 lab1_vec3, vec3 lab2_vec3);
float color_diff_Lab00(vec3 lab1_vec3, vec3 lab2_vec3);

void compute(const uint global_idx);

void main() {

    const uint global_idx = gl_GlobalInvocationID.x;
    if (global_idx >= option.task_num) {
        return;
    }
    //debugPrintfEXT("global_idx = %u, task_num = %u, colorset_size = %u, algo = %u", global_idx, option.colorset_size, option.algo);
    compute(global_idx);
}


void compute(const uint global_idx) {
    const vec3 unconverted = { unconverted_colors[global_idx * 3 + 0],
    unconverted_colors[global_idx * 3 + 1],
    unconverted_colors[global_idx * 3 + 2] };


    uint result_idx = 65535 - 1;
    float result_diff = 1e30 / 2;

    for (uint idx = 0; idx < option.colorset_size; idx++) {
        const vec3 color_ava = { colorset_colors[idx * 3 + 0],
        colorset_colors[idx * 3 + 1],
        colorset_colors[idx * 3 + 2] };
        if (true && have_nan(color_ava)) {
            debugPrintfEXT(
            "Nan spotted at color_ava. color_ava = {%f,%f,%f}, get_global_id = %u.n",
            color_ava[0], color_ava[1], color_ava[2],
            uint(global_idx));
            return;
        }

        float diff_sq = 0;

        /*

    float color_diff_RGB_XYZ(vec3 RGB1, vec3 RGB2);
    float color_diff_RGB_Better(vec3 rgb1, vec3 rgb2);
    float color_diff_HSV(vec3 hsv1_vec3, vec3 hsv2_vec3);
    float color_diff_Lab94(vec3 lab1_vec3, vec3 lab2_vec3);
    float color_diff_Lab00(vec3 lab1_vec3, vec3 lab2_vec3);
    */
        switch (uint(option.algo)) {
            case ALGO_RGB_BETTER:
            diff_sq = color_diff_RGB_Better(color_ava, unconverted);
            break;
            case ALGO_HSV:
            diff_sq = color_diff_HSV(color_ava, unconverted);
            break;
            case ALGO_LAB94:
            diff_sq = color_diff_Lab94(color_ava, unconverted);
            break;
            case ALGO_LAB00:
            diff_sq = color_diff_Lab00(color_ava, unconverted);
            break;
            default :
            diff_sq = color_diff_RGB_XYZ(color_ava, unconverted);

        }
        if (true && isnan(diff_sq)) {
            debugPrintfEXT("Spotted nan at idx = %u.n", (idx));
            return;
        }
        if (result_diff > diff_sq) {
            /* this branch may be optimized */
            result_idx = idx;
            result_diff = diff_sq;
        }
    }

    result_idx_dst[global_idx] = uint16_t(result_idx);
    result_diff_dst[global_idx] = result_diff;

}

float color_diff_RGB_Better(vec3 rgb1, vec3 rgb2) {
    const float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;
    const vec3 w_vec3 = { w_r, w_g, w_b };
    const float thre = 1e-4f;
    const vec3 one_vec3 = { 1, 1, 1 };

    const float SqrModSquare = norm2(rgb1) * norm2(rgb2);

    const vec3 delta_rgb_vec3 = rgb1 - rgb2;

    const float SigmaRGB = (sum3(rgb1) + sum3(rgb2)) / 3.0f;
    const vec3 S_rgb_vec3 = min((rgb1 + rgb2) / (SigmaRGB + thre), one_vec3);

    if (SC_OCL_SPOT_NAN && have_nan(S_rgb_vec3)) {
        debugPrintfEXT("S_rgb_vec3 contains nan.\n");
        return NAN;
    }

    const float sumRGBSquare = dot(rgb1, rgb2);

    const float theta = 2.0f / pi_fp32 *
    acos((sumRGBSquare * inversesqrt(SqrModSquare + thre)) / 1.01f);

    if (SC_OCL_SPOT_NAN && isnan(theta)) {
        debugPrintfEXT("theta is nan. sumRGBSquare = %f, SqrModSquare = %f.\n",
        sumRGBSquare, SqrModSquare);
        return NAN;
    }

    const vec3 OnedDelta_rgb_vec3 = abs(delta_rgb_vec3) / (rgb1 + rgb2 + thre);

    const float sumOnedDelta = sum3(OnedDelta_rgb_vec3) + thre;

    const vec3 S_t_rgb_vec3 =
    OnedDelta_rgb_vec3 / sumOnedDelta * S_rgb_vec3 * S_rgb_vec3;

    if (SC_OCL_SPOT_NAN && have_nan(S_t_rgb_vec3)) {
        debugPrintfEXT("S_t_rgb_vec3 contains nan.\n");
        return NAN;
    }

    const float S_theta = sum3(S_t_rgb_vec3);

    const vec3 rgb_max_vec3 = max(rgb1, rgb2);
    const float S_ratio =
    max(rgb_max_vec3[0], max(rgb_max_vec3[1], rgb_max_vec3[2]));

    const vec3 SS_w_delta_delta_vec3 =
    S_rgb_vec3 * S_rgb_vec3 * delta_rgb_vec3 * delta_rgb_vec3 * w_vec3;

    const float part1 = sum3(SS_w_delta_delta_vec3) / sum3(w_vec3);
    if (SC_OCL_SPOT_NAN && isnan(part1)) {
        debugPrintfEXT("part1 is nan.\n");
        return NAN;
    }

    const float part2 = S_theta * S_ratio * theta * theta;
    if (SC_OCL_SPOT_NAN && isnan(part2)) {
        debugPrintfEXT("part2 is nan.\n");
        return NAN;
    }

    return part1 + part2;
}

float color_diff_RGB_XYZ(vec3 RGB1, vec3 RGB2) {
    return norm2(RGB1 - RGB2);
}

float color_diff_HSV(vec3 hsv1_vec3, vec3 hsv2_vec3) {
    const float h1 = hsv1_vec3[0];
    const float s1 = hsv1_vec3[1];
    const float v1 = hsv1_vec3[2];

    const float h2 = hsv2_vec3[0];
    const float s2 = hsv2_vec3[1];
    const float v2 = hsv2_vec3[2];

    const float sv_1 = s1 * v1;
    const float sv_2 = s2 * v2;

    const float dX = 50.0f * (cos(h1) * sv_1 - cos(h2) * sv_2);
    const float dY = 50.0f * (sin(h1) * sv_1 - sin(h2) * sv_2);
    const float dZ = 50.0f * (v1 - v2);

    return dX * dX + dY * dY + dZ * dZ;
}

float color_diff_Lab94(vec3 lab1_vec3, vec3 lab2_vec3) {
    const float L1 = lab1_vec3[0];
    const float a1 = lab1_vec3[1];
    const float b1 = lab1_vec3[2];

    const float L2 = lab2_vec3[0];
    const float a2 = lab2_vec3[1];
    const float b2 = lab2_vec3[2];

    const float deltaL_2 = square(L1 - L2);
    const float C1_2 = a1 * a1 + b1 * b1;
    const float C2_2 = a2 * a2 + b2 * b2;

    const float deltaCab_2 = square(sqrt(C1_2) - sqrt(C2_2));
    const float deltaHab_2 = square(a2 - a1) + square(b2 - b1) - deltaCab_2;

    const float SC_2 = square(sqrt(C1_2) * 0.045f + 1.0f);
    const float SH_2 = square(sqrt(C2_2) * 0.015f + 1.0f);

    const float result = deltaL_2 + deltaCab_2 / SC_2 + deltaHab_2 / SH_2;
    return result;
}

float color_diff_Lab00(vec3 lab1_vec3, vec3 lab2_vec3) {
    const float kL = 1.0f;
    const float kC = 1.0f;
    const float kH = 1.0f;
    const float L1 = lab1_vec3[0];
    const float a1 = lab1_vec3[1];
    const float b1 = lab1_vec3[2];

    const float L2 = lab2_vec3[0];
    const float a2 = lab2_vec3[1];
    const float b2 = lab2_vec3[2];

    float C1sab = sqrt(a1 * a1 + b1 * b1);
    float C2sab = sqrt(a2 * a2 + b2 * b2);
    float mCsab = (C1sab + C2sab) / 2;
    float pow_mCsab_7 = pow(mCsab, 7);
    float G = 0.5 * (1 - sqrt(pow_mCsab_7 / (pow_mCsab_7 + pow(25.0f, 7.0f))));
    float a1p = (1 + G) * a1;
    float a2p = (1 + G) * a2;
    float C1p = sqrt(a1p * a1p + b1 * b1);
    float C2p = sqrt(a2p * a2p + b2 * b2);
    float h1p, h2p;
    if (b1 == 0 && a1p == 0)
    h1p = 0;
    else
    h1p = atan(b1, a1p);

    if (h1p < 0) h1p += 2 * pi_fp32;

    if (b2 == 0 && a2p == 0)
    h2p = 0;
    else
    h2p = atan(b2, a2p);

    if (h2p < 0) h2p += 2 * pi_fp32;

    float dLp = L2 - L1;
    float dCp = C2p - C1p;
    float dhp;
    if (C1p * C2p == 0) {
        dhp = 0;
    } else {
        if (abs(h2p - h1p) <= radians(180.0f)) {
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
    } else if (abs(h2p - h1p) <= radians(180.0f)) {
        mhp = (h1p + h2p) / 2;
    } else if (h1p + h2p < radians(360.0f)) {
        mhp = (h1p + h2p + radians(360.0f)) / 2;
    } else {
        mhp = (h1p + h2p - radians(360.0f)) / 2;
    }

    float T = 1 - 0.17f * cos(mhp - radians(30.0f)) + 0.24f * cos(2 * mhp) +
    0.32f * cos(3 * mhp + radians(6.0f)) -
    0.20f * cos(4 * mhp - radians(63.0f));

    float dTheta =
    radians(30.0f) * exp(-square((mhp - radians(275.0f)) / radians(25.0f)));

    float RC = 2 * sqrt(pow(mCp, 7) / (pow(25.0f, 7.0f) + pow(mCp, 7.0f)));
    float square_mLp_minus_50 = square(mLp - 50);
    float SL = 1 + 0.015f * square_mLp_minus_50 * inversesqrt(20 + square_mLp_minus_50);

    float SC = 1 + 0.045f * mCp;

    float SH = 1 + 0.015f * mCp * T;

    float RT = -RC * sin(2 * dTheta);

    float Diffsquare = square(dLp / SL / kL) + square(dCp / SC / kC) +
    square(dHp / SH / kH) +
    RT * (dCp / SC / kC) * (dHp / SH / kH);

    return Diffsquare;
}
