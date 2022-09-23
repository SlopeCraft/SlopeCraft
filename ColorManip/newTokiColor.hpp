#ifndef NEWTOKICOLOR_HPP
#define NEWTOKICOLOR_HPP

#include "../SlopeCraftL/SlopeCraftL.h"
#include "ColorManip.h"
#include <Eigen/Dense>
#include <cmath>

using Eigen::Dynamic;

template <bool isColorFixed, bool has_map_color, int max_color_count>
class colorset_new;

template <bool is_not_optical> class newTokiColorBase {
public:
  using TempVectorXf_t = Eigen::ArrayXf;
  using uintN_t = uint16_t;
  static constexpr uintN_t label_uncomputed = ~uintN_t(0);
  uintN_t result_color_id{label_uncomputed}; // the final color index

  inline bool is_result_computed() const noexcept {
    return (result_color_id != label_uncomputed);
  }
};

template <> class newTokiColorBase<true> {
public:
  using TempVectorXf_t = Eigen::Array<float, Dynamic, 1, Eigen::ColMajor, 256>;
  //记录与result的深度值不同的两个有损优化候选色选择系数（升序排列），Depth=3时无效

  using uintN_t = uint8_t;
  std::array<float, 2> sideSelectivity;
  //记录与result的深度值不同的两个有损优化候选色（升序排列），Depth=3时无效

  std::array<uint8_t, 2> sideResult;

  uint8_t Result; //最终调色结果

public:
  static bool needFindSide;
  static std::array<uint8_t, 4> DepthCount;
  // static ::SCL_convertAlgo convertAlgo;

public:
  inline bool is_result_computed() const noexcept { return (Result != 0); }
};

template <bool is_not_optical, class basic_t, class allowed_t>
class newTokiColor : public newTokiColorBase<is_not_optical> {
private:
  static constexpr float threshold = 1e-10f;

public:
  using Base_t = newTokiColorBase<is_not_optical>;
  using TempVectorXf_t = typename Base_t::TempVectorXf_t;
  using result_t = typename Base_t::uintN_t;

  Eigen::Array3f c3; //   color in some colorspace
  float ResultDiff;  // color diff for the result

  static inline ::SCL_convertAlgo &convertAlgo() noexcept {
    static ::SCL_convertAlgo val = SCL_convertAlgo::RGB_Better;
    return val;
  }
  // These two members must be defined by caller
  static const basic_t *const Basic;
  static const allowed_t *const Allowed;

public:
  explicit newTokiColor(ARGB rawColor) {
    switch (convertAlgo()) {
    case ::SCL_convertAlgo::RGB:
    case ::SCL_convertAlgo::RGB_Better:
    case ::SCL_convertAlgo::gaCvter:
      c3[0] = std::max(getR(rawColor) / 255.0f, threshold);
      c3[1] = std::max(getG(rawColor) / 255.0f, threshold);
      c3[2] = std::max(getB(rawColor) / 255.0f, threshold);
      break;

    case ::SCL_convertAlgo::HSV:
      RGB2HSV(getR(rawColor) / 255.0f, getG(rawColor) / 255.0f,
              getB(rawColor) / 255.0f, c3[0], c3[1], c3[2]);
      break;
    case ::SCL_convertAlgo::Lab94:
    case ::SCL_convertAlgo::Lab00:
      float X, Y, Z;
      RGB2XYZ(getR(rawColor) / 255.0f, getG(rawColor) / 255.0f,
              getB(rawColor) / 255.0f, X, Y, Z);
      XYZ2Lab(X, Y, Z, c3[0], c3[1], c3[2]);
      break;
    default:
      RGB2XYZ(getR(rawColor) / 255.0f, getG(rawColor) / 255.0f,
              getB(rawColor) / 255.0f, c3[0], c3[1], c3[2]);
      break;
    }
  }

  newTokiColor() {
    if constexpr (is_not_optical) {
      this->Result = 0;
    } else {
    }
  }

  result_t compute(ARGB argb) noexcept {
    if (getA(argb) == 0) {
      if constexpr (is_not_optical) {
        this->Result = 0;
        return this->Result;
      } else {
        static_assert(is_not_optical, "Not implemented here.");

        return 0;
      }
    }

    if (this->is_result_computed()) {
      if constexpr (is_not_optical) {
        return this->Result;
      } else {
        return this->result_color_id;
      }
    }

    switch (convertAlgo()) {
    case ::SCL_convertAlgo::RGB:
      return applyRGB();
    case ::SCL_convertAlgo::RGB_Better:
      return applyRGB_plus();
    case ::SCL_convertAlgo::HSV:
      return applyHSV();
    case ::SCL_convertAlgo::Lab94:
      return applyLab94();
    case ::SCL_convertAlgo::Lab00:
      return applyLab00();
    case ::SCL_convertAlgo::XYZ:
      return applyXYZ();

    default:
      exit(1);
      return 0;
    }
  }

private:
  template <typename = void> void doSide(const TempVectorXf_t &Diff) {
    static_assert(is_not_optical, "");

    int tempIndex = 0;
    // Diff.array()+=10.0;ResultDiff+=10.0;
    this->sideSelectivity[0] = 1e35f;
    this->sideResult[0] = 0;
    this->sideSelectivity[1] = 1e35f;
    this->sideResult[1] = 0;

    // using Base_t::DepthCount;
    // using Base_t::needFindSide;

    if (!Base_t::needFindSide)
      return;
    // qDebug("开始doSide");
    // qDebug()<<"size(Diff)=["<<Diff.rows()<<','<<Diff.cols()<<']';
    // qDebug()<<"DepthCount="<<(short)DepthCount[0]<<;
    // qDebug()<<"DepthCount=["<<(short)DepthCount[0]<<','<<(short)DepthCount[1]<<','<<(short)DepthCount[2]<<','<<(short)DepthCount[3]<<']';
    // qDebug()<<"DepthIndex=["<<DepthIndexEnd[0]<<','<<DepthIndexEnd[1]<<','<<DepthIndexEnd[2]<<','<<DepthIndexEnd[3]<<']';
    switch (this->Result % 4) {
    case 3:
      return;
    case 0: // 1,2
      if (Base_t::DepthCount[1]) {
        this->sideSelectivity[0] =
            Diff.segment(Base_t::DepthCount[0], Base_t::DepthCount[1])
                .minCoeff(&tempIndex);
        this->sideResult[0] = Allowed->Map(Base_t::DepthCount[0] + tempIndex);
      }
      if (Base_t::DepthCount[2]) {
        this->sideSelectivity[1] =
            Diff.segment(Base_t::DepthCount[0] + Base_t::DepthCount[1],
                         Base_t::DepthCount[2])
                .minCoeff(&tempIndex);
        this->sideResult[1] = Allowed->Map(Base_t::DepthCount[0] +
                                           Base_t::DepthCount[1] + tempIndex);
      }
      break;
    case 1: // 0,2
      if (Base_t::DepthCount[0]) {
        this->sideSelectivity[0] =
            Diff.segment(0, Base_t::DepthCount[0]).minCoeff(&tempIndex);
        this->sideResult[0] = Allowed->Map(0 + tempIndex);
      }
      if (Base_t::DepthCount[2]) {
        this->sideSelectivity[1] =
            Diff.segment(Base_t::DepthCount[0] + Base_t::DepthCount[1],
                         Base_t::DepthCount[2])
                .minCoeff(&tempIndex);
        this->sideResult[1] = Allowed->Map(Base_t::DepthCount[0] +
                                           Base_t::DepthCount[1] + tempIndex);
      }
      break;
    case 2: // 0,1
      if (Base_t::DepthCount[0]) {
        this->sideSelectivity[0] =
            Diff.segment(0, Base_t::DepthCount[0]).minCoeff(&tempIndex);
        this->sideResult[0] = Allowed->Map(0 + tempIndex);
      }
      if (Base_t::DepthCount[1]) {
        this->sideSelectivity[1] =
            Diff.segment(Base_t::DepthCount[0], Base_t::DepthCount[1])
                .minCoeff(&tempIndex);
        this->sideResult[1] = Allowed->Map(Base_t::DepthCount[0] + tempIndex);
      }
      break;
    }
    // sideSelectivity[0]-=1.0;sideSelectivity[1]-=1.0;
    // sideSelectivity[0]*=100.0;sideSelectivity[1]*=100.0;
    /*
    qDebug()<<"side[0]=["<<sideResult[0]<<','<<sideSelectivity[0]<<']';
    qDebug()<<"side[1]=["<<sideResult[1]<<','<<sideSelectivity[1]<<']';
    qDebug()<<"ResultDiff="<<ResultDiff;*/
    return;
  }

  result_t applyRGB() noexcept {

    int tempIndex = 0; // the index of best color

    auto Diff0_2 = (Allowed->rgb(0) - c3[0]).square();
    auto Diff1_2 = (Allowed->rgb(1) - c3[1]).square();
    auto Diff2_2 = (Allowed->rgb(2) - c3[2]).square();

    TempVectorXf_t Diff = Diff0_2 + Diff1_2 + Diff2_2;
    // Data.CurrentColor-=allowedColors;

    ResultDiff = Diff.minCoeff(&tempIndex) + threshold;
    // Diff.minCoeff(tempIndex,u);
    if constexpr (is_not_optical) {
      this->Result = Allowed->Map(tempIndex);
      if (Base_t::needFindSide)
        doSide(Diff);
      return this->Result;
    } else {
      static_assert(is_not_optical, "Not implemented here.");

      return 0;
    }
  }

  result_t applyRGB_plus() noexcept {

    int tempIndex = 0;
    // const ColorList &allowedColors = Allowed->_RGB;
    float R = c3[0];
    float g = c3[1];
    float b = c3[2];
    float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;
    auto SqrModSquare = ((R * R + g * g + b * b) *
                         (Allowed->rgb(0).square() + Allowed->rgb(1).square() +
                          Allowed->rgb(2).square()))
                            .sqrt();
    auto deltaR = (R - Allowed->rgb(0));
    auto deltaG = (g - Allowed->rgb(1));
    auto deltaB = (b - Allowed->rgb(2));
    auto SigmaRGB =
        (R + g + b + Allowed->rgb(0) + Allowed->rgb(1) + Allowed->rgb(2)) /
        3.0f;
    auto S_r =
        ((Allowed->rgb(0) + R) < SigmaRGB)
            .select((Allowed->rgb(0) + R) / (SigmaRGB + threshold), 1.0f);
    auto S_g =
        ((Allowed->rgb(1) + g) < SigmaRGB)
            .select((Allowed->rgb(1) + g) / (SigmaRGB + threshold), 1.0f);
    auto S_b =
        ((Allowed->rgb(2) + b) < SigmaRGB)
            .select((Allowed->rgb(2) + b) / (SigmaRGB + threshold), 1.0f);
    auto sumRGBsquare =
        R * Allowed->rgb(0) + g * Allowed->rgb(1) + b * Allowed->rgb(2);
    auto theta =
        2.0 / M_PI * (sumRGBsquare / (SqrModSquare + threshold) / 1.01f).acos();
    auto OnedDeltaR = deltaR.abs() / (R + Allowed->rgb(0) + threshold);
    auto OnedDeltaG = deltaG.abs() / (g + Allowed->rgb(1) + threshold);
    auto OnedDeltaB = deltaB.abs() / (b + Allowed->rgb(2) + threshold);
    auto sumOnedDelta = OnedDeltaR + OnedDeltaG + OnedDeltaB + threshold;
    auto S_tr = OnedDeltaR / sumOnedDelta * S_r.square();
    auto S_tg = OnedDeltaG / sumOnedDelta * S_g.square();
    auto S_tb = OnedDeltaB / sumOnedDelta * S_b.square();
    auto S_theta = S_tr + S_tg + S_tb;

    auto Rmax = Allowed->rgb(0)
                    .max(Allowed->rgb(1))
                    .max(Allowed->rgb(2))
                    .max(threshold);
    // auto Rmax = allowedColors.rowwise().maxCoeff();
    auto S_ratio = Rmax.max(std::max(R, std::max(g, b)));

    TempVectorXf_t dist =
        (S_r.square() * w_r * deltaR.square() +
         S_g.square() * w_g * deltaG.square() +
         S_b.square() * w_b * deltaB.square()) /
            (w_r + w_g + w_b) +
        S_theta * S_ratio * theta.square(); //+S_theta*S_ratio*theta.square()

    ResultDiff = dist.minCoeff(&tempIndex);
    /*if(dist.isNaN().any()){
        qDebug("出现Nan");
            if(SqrModSquare.isNaN().any())      qDebug("SqrModSquare出现Nan");
            if(theta.isNaN().any())                      qDebug("theta出现Nan");
            if(sumOnedDelta.isNaN().any())      qDebug("sumOnedDelta出现Nan");
            if(S_ratio.isNaN().any())                  qDebug("S_ratio出现Nan");

    }*/

    if constexpr (is_not_optical) {
      this->Result = Allowed->Map(tempIndex);
      if (Base_t::needFindSide)
        doSide(dist);

      return this->Result;
    } else {
      static_assert(is_not_optical, "Not implemented here.");

      return 0;
    }
  }

  result_t applyHSV() noexcept {

    int tempIndex = 0;
    // const ColorList &allowedColors = Allowed->HSV;

    auto S_times_V = Allowed->hsv(1) * Allowed->hsv(2);
    const float s_times_v = c3[1] * c3[2];
    auto deltaX = 50.0f * (Allowed->hsv(0).cos() * S_times_V -
                           s_times_v * std::cos(c3[0]));
    auto deltaY = 50.0f * (Allowed->hsv(0).sin() * S_times_V -
                           s_times_v * std::sin(c3[0]));
    auto deltaZ = 50.0f * (Allowed->hsv(2) - c3[2]);
    TempVectorXf_t Diff = deltaX.square() + deltaY.square() + deltaZ.square();

    ResultDiff = Diff.minCoeff(&tempIndex);

    if constexpr (is_not_optical) {
      this->Result = Allowed->Map(tempIndex);
      if (Base_t::needFindSide)
        doSide(Diff);
      return this->Result;
    } else {
      static_assert(is_not_optical, "Not implemented here.");

      return 0;
    }
  }

  result_t applyXYZ() noexcept {
    int tempIndex = 0;
    auto Diff0_2 = (Allowed->xyz(0) - c3[0]).square();
    auto Diff1_2 = (Allowed->xyz(1) - c3[1]).square();
    auto Diff2_2 = (Allowed->xyz(2) - c3[2]).square();

    TempVectorXf_t Diff = Diff0_2 + Diff1_2 + Diff2_2;
    // Data.CurrentColor-=allowedColors;
    ResultDiff = Diff.minCoeff(&tempIndex);

    if constexpr (is_not_optical) {
      this->Result = Allowed->Map(tempIndex);
      if (Base_t::needFindSide)
        doSide(Diff);
      return this->Result;
    } else {
      static_assert(is_not_optical, "Not implemented here.");

      return 0;
    }
  }

  result_t applyLab94() noexcept {
    int tempIndex = 0;
    float L = c3[0];
    float a = c3[1];
    float b = c3[2];
    // const ColorList &allowedColors = Allowed->Lab;
    auto deltaL_2 = (Allowed->lab(0) - L).square();
    float C1_2 = a * a + b * b;
    TempVectorXf_t C2_2 = Allowed->lab(1).square() + Allowed->lab(2).square();
    auto deltaCab_2 = (sqrt(C1_2) - C2_2.sqrt()).square();
    auto deltaHab_2 = (Allowed->lab(1) - a).square() +
                      (Allowed->lab(2) - b).square() - deltaCab_2;
    // SL=1,kL=1
    // K1=0.045f
    // K2=0.015f
    float SC_2 = (sqrt(C1_2) * 0.045f + 1.0f) * (sqrt(C1_2) * 0.045f + 1.0f);
    auto SH_2 = (C2_2.sqrt() * 0.015f + 1.0f).square();
    TempVectorXf_t Diff = deltaL_2 + deltaCab_2 / SC_2 + deltaHab_2 / SH_2;
    ResultDiff = Diff.minCoeff(&tempIndex);

    if constexpr (is_not_optical) {
      this->Result = Allowed->Map(tempIndex);
      if (Base_t::needFindSide)
        doSide(Diff);
      return this->Result;
    } else {
      static_assert(is_not_optical, "Not implemented here.");

      return 0;
    }
  }

  result_t applyLab00() noexcept {
    int tempIndex = 0;
    float L1s = c3[0];
    float a1s = c3[1];
    float b1s = c3[2];
    // const ColorList &allow = Allowed->Lab;
    TempVectorXf_t Diff(Allowed->colorCount());

    for (short i = 0; i < Allowed->colorCount(); i++) {
      Diff(i) = Lab00_diff(L1s, a1s, b1s, Allowed->Lab(i, 0),
                           Allowed->Lab(i, 1), Allowed->Lab(i, 2));
    }

    if constexpr (is_not_optical) {
      this->Result = Allowed->Map(tempIndex);
      if (Base_t::needFindSide)
        doSide(Diff);
      return this->Result;
    } else {
      static_assert(is_not_optical, "Not implemented here.");

      return 0;
    }
  }
};

#endif // NEWTOKICOLOR_HPP