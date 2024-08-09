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

#ifndef SLOPECRAFT_UTILITIES_MCDATAVERSION_H
#define SLOPECRAFT_UTILITIES_MCDATAVERSION_H

#include <SC_GlobalEnums.h>

#include <stdint.h>

namespace MCDataVersion {

enum class MCDataVersion_t : int {
  Java_1_21 = 3953,
  Java_1_20_5 = 3837,
  Snapshot_23w51b = 3802,
  Snapshot_23w51a = 3801,
  Java_1_20_4 = 3700,
  Java_1_20_4_rc1 = 3699,
  Java_1_20_3 = 3698,
  Java_1_20_3__rc2 = 3577,
  Java_1_20_3__rc1 = 3576,
  Java_1_20_2_pre4 = 3575,
  Java_1_20_2_pre3 = 3574,
  Java_1_20_2_pre2 = 3573,
  Java_1_20_2_pre1 = 3572,
  Snapshot_23w35a = 3571,
  Snapshot_23w33a = 3570,
  Snapshot_23w32a = 3569,
  Snapshot_23w31a = 3567,
  Java_1_20_1 = 3465,
  Java_1_20_1__rc1 = 3464,
  Java_1_20 = 3463,
  Java_1_20__rc1 = 3462,
  Java_1_20__pre7 = 3461,
  Java_1_20__pre6 = 3460,
  Java_1_20__pre5 = 3458,
  Java_1_20__pre4 = 3457,
  Java_1_20__pre3 = 3456,
  Java_1_20__pre2 = 3455,
  Java_1_20__pre1 = 3454,
  Snapshot_23w_18a = 3453,
  Snapshot_23w_17a = 3452,
  Snapshot_23w_16a = 3449,
  Snapshot_23w_14a = 3445,
  Snapshot_23w_13a = 3442,
  Snapshot_23w_12a = 3442,
  Java_1_19_4 = 3337,
  Java_1_19_4__rc3 = 3336,
  Java_1_19_4__rc2 = 3335,
  Java_1_19_4__rc1 = 3334,
  Java_1_19_4__pre4 = 3333,
  Java_1_19_4__pre3 = 3332,
  Java_1_19_4__pre2 = 3331,
  Java_1_19_4__pre1 = 3330,
  Snapshot_23w07a = 3329,
  Snapshot_23w06a = 3326,
  Snapshot_23w05a = 3323,
  Snapshot_23w04a = 3321,
  Snapshot_23w03a = 3320,
  Java_1_19_3 = 3218,
  Java_1_19_3__rc3 = 3217,
  Java_1_19_3__rc2 = 3216,
  Java_1_19_3__rc1 = 3215,
  Java_1_19_3__pre3 = 3213,
  Java_1_19_3__pre2 = 3212,
  Java_1_19_3__pre1 = 3211,
  Snapshot_22w46a = 3210,
  Snapshot_22w45a = 3208,
  Snapshot_22w44a = 3207,
  Snapshot_22w43a = 3206,
  Snapshot_22w42a = 3205,
  Java_1_19_2 = 3120,
  Java_1_19_2__rc2 = 3119,
  Java_1_19_2__rc1 = 3118,
  Java_1_19_1 = 3117,
  Java_1_19_1__rc3 = 3116,
  Java_1_19_1__rc2 = 3115,
  Java_1_19_1__pre6 = 3114,
  Java_1_19_1__pre5 = 3113,
  Java_1_19_1__pre4 = 3112,
  Java_1_19_1__pre3 = 3111,
  Java_1_19_1__pre2 = 3110,
  Java_1_19_1__rc1 = 3109,
  Java_1_19_1__pre1 = 3107,
  Snapshot_22w24a = 3106,
  Java_1_19 = 3105,
  Java_1_19__rc2 = 3104,
  Java_1_19__rc1 = 3103,
  Java_1_19__pre5 = 3102,
  Java_1_19__pre4 = 3101,
  Java_1_19__pre3 = 3100,
  Java_1_19__pre2 = 3099,
  Java_1_19__pre1 = 3098,
  Snapshot_22w19a = 3096,
  Snapshot_22w18a = 3095,
  Snapshot_22w17a = 3093,
  Snapshot_22w16b = 3092,
  Snapshot_22w16a = 3091,
  Snapshot_22w15a = 3089,
  Snapshot_22w14a = 3088,
  Snapshot_22w13a = 3085,
  Snapshot_22w12a = 3082,
  Snapshot_22w11a = 3080,
  Java_1_19__exp1 = 3066,
  Java_1_18_2 = 2975,
  Java_1_18_2__rc1 = 2974,
  Java_1_18_2__pre3 = 2973,
  Java_1_18_2__pre2 = 2972,
  Java_1_18_2__pre1 = 2971,
  Snapshot_22w07a = 2969,
  Snapshot_22w06a = 2968,
  Snapshot_22w05a = 2967,
  Snapshot_22w03a = 2966,
  Java_1_18_1 = 2865,
  Java_1_18_1__rc3 = 2864,
  Java_1_18_1__rc2 = 2863,
  Java_1_18_1__rc1 = 2862,
  Java_1_18_1__pre1 = 2861,
  Java_1_18 = 2860,
  Java_1_18__rc4 = 2859,
  Java_1_18__rc3 = 2858,
  Java_1_18__rc2 = 2857,
  Java_1_18__rc1 = 2856,
  Java_1_18__pre8 = 2855,
  Java_1_18__pre7 = 2854,
  Java_1_18__pre6 = 2853,
  Java_1_18__pre5 = 2851,
  Java_1_18__pre4 = 2850,
  Java_1_18__pre3 = 2849,
  Java_1_18__pre2 = 2848,
  Java_1_18__pre1 = 2847,
  Snapshot_21w44a = 2845,
  Snapshot_21w43a = 2844,
  Snapshot_21w42a = 2840,
  Snapshot_21w41a = 2839,
  Snapshot_21w40a = 2838,
  Snapshot_21w39a = 2836,
  Snapshot_21w38a = 2835,
  Snapshot_21w37a = 2834,
  Java_1_18__exp7 = 2831,
  Java_1_18__exp6 = 2830,
  Java_1_18__exp5 = 2829,
  Java_1_18__exp4 = 2828,
  Java_1_18__exp3 = 2827,
  Java_1_18__exp2 = 2826,
  Java_1_18__exp1 = 2825,
  Java_1_17_1 = 2730,
  Java_1_17_1__rc2 = 2729,
  Java_1_17_1__rc1 = 2728,
  Java_1_17_1__pre3 = 2727,
  Java_1_17_1__pre2 = 2726,
  Java_1_17_1__pre1 = 2725,
  Java_1_17 = 2724,
  Java_1_17__rc2 = 2723,
  Java_1_17__rc1 = 2722,
  Java_1_17__pre5 = 2721,
  Java_1_17__pre4 = 2720,
  Java_1_17__pre3 = 2719,
  Java_1_17__pre2 = 2718,
  Java_1_17__pre1 = 2716,
  Snapshot_21w20a = 2715,
  Snapshot_21w19a = 2714,
  Snapshot_21w18a = 2713,
  Snapshot_21w17a = 2712,
  Snapshot_21w16a = 2711,
  Snapshot_21w15a = 2709,
  Snapshot_21w14a = 2706,
  Snapshot_21w13a = 2705,
  Snapshot_21w11a = 2703,
  Snapshot_21w10a = 2699,
  Snapshot_21w08b = 2698,
  Snapshot_21w08a = 2697,
  Snapshot_21w07a = 2695,
  Snapshot_21w06a = 2694,
  Snapshot_21w05b = 2692,
  Snapshot_21w05a = 2690,
  Snapshot_21w03a = 2689,
  Snapshot_20w51a = 2687,
  Snapshot_20w49a = 2685,
  Snapshot_20w48a = 2683,
  Snapshot_20w46a = 2682,
  Snapshot_20w45a = 2681,
  Combat_Test_8c = 2707,
  Combat_Test_8b = 2706,
  Combat_Test_8 = 2705,
  Combat_Test_7c = 2704,
  Combat_Test_7b = 2703,
  Combat_Test_7 = 2702,
  Combat_Test_6 = 2701,
  Java_1_16_5 = 2586,
  Java_1_16_5__rc1 = 2585,
  Java_1_16_4 = 2584,
  Java_1_16_4__rc1 = 2583,
  Java_1_16_4__pre2 = 2582,
  Java_1_16_4__pre1 = 2581,
  Java_1_16_3 = 2580,
  Java_1_16_3__rc1 = 2579,
  Java_1_16_2 = 2578,
  Java_1_16_2__rc2 = 2577,
  Java_1_16_2__rc1 = 2576,
  Java_1_16_2__pre3 = 2575,
  Java_1_16_2__pre2 = 2574,
  Java_1_16_2__pre1 = 2573,
  Snapshot_20w30a = 2572,
  Snapshot_20w29a = 2571,
  Snapshot_20w28a = 2570,
  Snapshot_20w27a = 2569,
  Java_1_16_1 = 2567,
  Java_1_16 = 2566,
  Java_1_16__rc1 = 2565,
  Java_1_16__pre8 = 2564,
  Java_1_16__pre7 = 2563,
  Java_1_16__pre6 = 2562,
  Java_1_16__pre5 = 2561,
  Java_1_16__pre4 = 2560,
  Java_1_16__pre3 = 2559,
  Java_1_16__pre2 = 2557,
  Java_1_16__pre1 = 2556,
  Snapshot_20w22a = 2555,
  Snapshot_20w21a = 2554,
  Snapshot_20w20b = 2537,
  Snapshot_20w20a = 2536,
  Snapshot_20w19a = 2534,
  Snapshot_20w18a = 2532,
  Snapshot_20w17a = 2529,
  Snapshot_20w16a = 2526,
  Snapshot_20w15a = 2525,
  Snapshot_20w14a = 2524,
  Snapshot_20w13b = 2521,
  Snapshot_20w13a = 2520,
  Snapshot_20w12a = 2515,
  Snapshot_20w11a = 2513,
  Snapshot_20w10a = 2512,
  Snapshot_20w09a = 2510,
  Snapshot_20w08a = 2507,
  Snapshot_20w07a = 2506,
  Snapshot_20w06a = 2504,
  Combat_Test_5 = 2321,
  Combat_Test_4 = 2320,
  Java_1_15_2 = 2230,
  Java_1_15_2__pre2 = 2229,
  Java_1_15_2__pre1 = 2228,
  Java_1_15_1 = 2227,
  Java_1_15_1__pre1 = 2226,
  Java_1_15 = 2225,
  Java_1_15__pre7 = 2224,
  Java_1_15__pre6 = 2223,
  Java_1_15__pre5 = 2222,
  Java_1_15__pre4 = 2221,
  Java_1_15__pre3 = 2220,
  Java_1_15__pre2 = 2219,
  Java_1_15__pre1 = 2218,
  Snapshot_19w46b = 2217,
  Snapshot_19w46a = 2216,
  Snapshot_19w45b = 2215,
  Snapshot_19w45a = 2214,
  Snapshot_19w44a = 2213,
  Snapshot_19w42a = 2212,
  Snapshot_19w41a = 2210,
  Snapshot_19w40a = 2208,
  Snapshot_19w39a = 2207,
  Snapshot_19w38b = 2206,
  Snapshot_19w38a = 2205,
  Snapshot_19w37a = 2204,
  Snapshot_19w36a = 2203,
  Snapshot_19w35a = 2201,
  Snapshot_19w34a = 2200,
  Combat_Test_3 = 2069,
  Combat_Test_2 = 2068,
  Java_1_14_3__Combat_Test_ = 2067,
  Java_1_14_4 = 1976,
  Java_1_14_4__pre7 = 1975,
  Java_1_14_4__pre6 = 1974,
  Java_1_14_4__pre5 = 1973,
  Java_1_14_4__pre4 = 1972,
  Java_1_14_4__pre3 = 1971,
  Java_1_14_4__pre2 = 1970,
  Java_1_14_4__pre1 = 1969,
  Java_1_14_3 = 1968,
  Java_1_14_3__pre4 = 1967,
  Java_1_14_3__pre3 = 1966,
  Java_1_14_3__pre2 = 1965,
  Java_1_14_3__pre1 = 1964,
  Java_1_14_2 = 1963,
  Java_1_14_2__pre4 = 1962,
  Java_1_14_2__pre3 = 1960,
  Java_1_14_2__pre2 = 1959,
  Java_1_14_2__pre1 = 1958,
  Java_1_14_1 = 1957,
  Java_1_14_1__pre2 = 1956,
  Java_1_14_1__pre1 = 1955,
  Java_1_14 = 1952,
  Java_1_14__pre5 = 1951,
  Java_1_14__pre4 = 1950,
  Java_1_14__pre3 = 1949,
  Java_1_14__pre2 = 1948,
  Java_1_14__pre1 = 1947,
  Snapshot_19w14b = 1945,
  Snapshot_19w14a = 1944,
  Snapshot_19w13b = 1943,
  Snapshot_19w13a = 1942,
  Snapshot_19w12b = 1941,
  Snapshot_19w12a = 1940,
  Snapshot_19w11b = 1938,
  Snapshot_19w11a = 1937,
  Snapshot_19w09a = 1935,
  Snapshot_19w08b = 1934,
  Snapshot_19w08a = 1933,
  Snapshot_19w07a = 1932,
  Snapshot_19w06a = 1931,
  Snapshot_19w05a = 1930,
  Snapshot_19w04b = 1927,
  Snapshot_19w04a = 1926,
  Snapshot_19w03c = 1924,
  Snapshot_19w03b = 1923,
  Snapshot_19w03a = 1922,
  Snapshot_19w02a = 1921,
  Snapshot_18w50a = 1919,
  Snapshot_18w49a = 1916,
  Snapshot_18w48b = 1915,
  Snapshot_18w48a = 1914,
  Snapshot_18w47b = 1913,
  Snapshot_18w47a = 1912,
  Snapshot_18w46a = 1910,
  Snapshot_18w45a = 1908,
  Snapshot_18w44a = 1907,
  Snapshot_18w43c = 1903,
  Snapshot_18w43b = 1902,
  Snapshot_18w43a = 1901,
  Java_1_13_2 = 1631,
  Java_1_13_2__pre2 = 1630,
  Java_1_13_2__pre1 = 1629,
  Java_1_13_1 = 1628,
  Java_1_13_1__pre2 = 1627,
  Java_1_13_1__pre1 = 1626,
  Snapshot_18w33a = 1625,
  Snapshot_18w32a = 1623,
  Snapshot_18w31a = 1622,
  Snapshot_18w30b = 1621,
  Snapshot_18w30a = 1620,
  Java_1_13 = 1519,
  Java_1_13__pre10 = 1518,
  Java_1_13__pre9 = 1517,
  Java_1_13__pre8 = 1516,
  Java_1_13__pre7 = 1513,
  Java_1_13__pre6 = 1512,
  Java_1_13__pre5 = 1511,
  Java_1_13__pre4 = 1504,
  Java_1_13__pre3 = 1503,
  Java_1_13__pre2 = 1502,
  Java_1_13__pre1 = 1501,
  Snapshot_18w22c = 1499,
  Snapshot_18w22b = 1498,
  Snapshot_18w22a = 1497,
  Snapshot_18w21b = 1496,
  Snapshot_18w21a = 1495,
  Snapshot_18w20c = 1493,
  Snapshot_18w20b = 1491,
  Snapshot_18w20a = 1489,
  Snapshot_18w19b = 1485,
  Snapshot_18w19a = 1484,
  Snapshot_18w16a = 1483,
  Snapshot_18w15a = 1482,
  Snapshot_18w14b = 1481,
  Snapshot_18w14a = 1479,
  Snapshot_18w11a = 1478,
  Snapshot_18w10d = 1477,
  Snapshot_18w10c = 1476,
  Snapshot_18w10b = 1474,
  Snapshot_18w10a = 1473,
  Snapshot_18w09a = 1472,
  Snapshot_18w08b = 1471,
  Snapshot_18w08a = 1470,
  Snapshot_18w07c = 1469,
  Snapshot_18w07b = 1468,
  Snapshot_18w07a = 1467,
  Snapshot_18w06a = 1466,
  Snapshot_18w05a = 1464,
  Snapshot_18w03b = 1463,
  Snapshot_18w03a = 1462,
  Snapshot_18w02a = 1461,
  Snapshot_18w01a = 1459,
  Snapshot_17w50a = 1457,
  Snapshot_17w49b = 1455,
  Snapshot_17w49a = 1454,
  Snapshot_17w48a = 1453,
  Snapshot_17w47b = 1452,
  Snapshot_17w47a = 1451,
  Snapshot_17w46a = 1449,
  Snapshot_17w45b = 1448,
  Snapshot_17w45a = 1447,
  Snapshot_17w43b = 1445,
  Snapshot_17w43a = 1444,
  Java_1_12_2 = 1343,
  Java_1_12_2__pre2 = 1342,
  Java_1_12_2__pre1 = 1341,
  Java_1_12_1 = 1241,
  Java_1_12_1__pre1 = 1240,
  Snapshot_17w31a = 1239,
  Java_1_12 = 1139,
  Java_1_12__pre7 = 1138,
  Java_1_12__pre6 = 1137,
  Java_1_12__pre5 = 1136,
  Java_1_12__pre4 = 1135,
  Java_1_12__pre3 = 1134,
  Java_1_12__pre2 = 1133,
  Java_1_12__pre1 = 1132,
  Snapshot_17w18b = 1131,
  Snapshot_17w18a = 1130,
  Snapshot_17w17b = 1129,
  Snapshot_17w17a = 1128,
  Snapshot_17w16b = 1127,
  Snapshot_17w16a = 1126,
  Snapshot_17w15a = 1125,
  Snapshot_17w14a = 1124,
  Snapshot_17w13b = 1123,
  Snapshot_17w13a = 1122,
  Snapshot_17w06a = 1022,
  Java_1_11_2 = 922,
  Java_1_11_1 = 921,
  Snapshot_16w50a = 920,
  Java_1_11 = 819,
  Java_1_11__pre1 = 818,
  Snapshot_16w44a = 817,
  Snapshot_16w43a = 816,
  Snapshot_16w42a = 815,
  Snapshot_16w41a = 814,
  Snapshot_16w40a = 813,
  Snapshot_16w39c = 812,
  Snapshot_16w39b = 811,
  Snapshot_16w39a = 809,
  Snapshot_16w38a = 807,
  Snapshot_16w36a = 805,
  Snapshot_16w35a = 803,
  Snapshot_16w33a = 802,
  Snapshot_16w32b = 801,
  Snapshot_16w32a = 800,
  Java_1_10_2 = 512,
  Java_1_10_1 = 511,
  Java_1_10 = 510,
  Java_1_10__pre2 = 507,
  Java_1_10__pre1 = 506,
  Snapshot_16w21b = 504,
  Snapshot_16w21a = 503,
  Snapshot_16w20a = 501,
  Java_1_9_4 = 184,
  Java_1_9_3 = 183,
  Java_1_9_3__pre3 = 182,
  Java_1_9_3__pre2 = 181,
  Java_1_9_3__pre1 = 180,
  Snapshot_16w15b = 179,
  Snapshot_16w15a = 178,
  Snapshot_16w14a = 177,
  Java_1_9_2 = 176,
  Java_1_9_1 = 175,
  Java_1_9_1__pre3 = 172,
  Java_1_9_1__pre2 = 171,
  Java_1_9_1__pre1 = 170,
  Java_1_9 = 169,
  Java_1_9__pre4 = 168,
  Java_1_9__pre3 = 167,
  Java_1_9__pre2 = 165,
  Java_1_9__pre1 = 164,
  Snapshot_16w07b = 163,
  Snapshot_16w07a = 162,
  Snapshot_16w06a = 161,
  Snapshot_16w05b = 160,
  Snapshot_16w05a = 159,
  Snapshot_16w04a = 158,
  Snapshot_16w03a = 157,
  Snapshot_16w02a = 156,
  Snapshot_15w51b = 155,
  Snapshot_15w51a = 154,
  Snapshot_15w50a = 153,
  Snapshot_15w49b = 152,
  Snapshot_15w49a = 151,
  Snapshot_15w47c = 150,
  Snapshot_15w47b = 149,
  Snapshot_15w47a = 148,
  Snapshot_15w46a = 146,
  Snapshot_15w45a = 145,
  Snapshot_15w44b = 143,
  Snapshot_15w44a = 142,
  Snapshot_15w43c = 141,
  Snapshot_15w43b = 140,
  Snapshot_15w43a = 139,
  Snapshot_15w42a = 138,
  Snapshot_15w41b = 137,
  Snapshot_15w41a = 136,
  Snapshot_15w40b = 134,
  Snapshot_15w40a = 133,
  Snapshot_15w39c = 132,
  Snapshot_15w39b = 131,
  Snapshot_15w39a = 130,
  Snapshot_15w38b = 129,
  Snapshot_15w38a = 128,
  Snapshot_15w37a = 127,
  Snapshot_15w36d = 126,
  Snapshot_15w36c = 125,
  Snapshot_15w36b = 124,
  Snapshot_15w36a = 123,
  Snapshot_15w35e = 122,
  Snapshot_15w35d = 121,
  Snapshot_15w35c = 120,
  Snapshot_15w35b = 119,
  Snapshot_15w35a = 118,
  Snapshot_15w34d = 117,
  Snapshot_15w34c = 116,
  Snapshot_15w34b = 115,
  Snapshot_15w34a = 114,
  Snapshot_15w33c = 112,
  Snapshot_15w33b = 111,
  Snapshot_15w33a = 111,
  Snapshot_15w32c = 104,
  Snapshot_15w32b = 103,
  Snapshot_15w32a = 100,

};

MCDataVersion_t string_to_data_version(const char *str,
                                       bool *ok = nullptr) noexcept;
const char *data_version_to_string(MCDataVersion_t v) noexcept;

MCDataVersion_t max_supported_version(SCL_gameVersion v) noexcept;

MCDataVersion_t min_supported_version(SCL_gameVersion v) noexcept;

MCDataVersion_t suggested_version(SCL_gameVersion v) noexcept;

inline bool is_data_version_suitable(SCL_gameVersion v,
                                     MCDataVersion_t dv) noexcept {
  if (dv < min_supported_version(v) || dv > max_supported_version(v)) {
    return false;
  }

  return true;
}

}  // namespace MCDataVersion
#endif  // SLOPECRAFT_UTILITIES_MCDATAVERSION_H