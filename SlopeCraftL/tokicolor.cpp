/*
 Copyright © 2021-2022  TokiNoBug
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
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef TOKICOLOR_CPP
#define TOKICOLOR_CPP
#include "ColorSet.h"
//#define threshold 1e-10f

static constexpr float threshold = 1e-10f;

#include <cmath>
#define deg2rad(deg) ((deg)*M_PI / 180.0)
/*
auto atan2(Eigen::VectorXf y,Eigen::VectorXf x)
{
    return
(x.array()>0.0).select((y.array()/x.array()).atan(),(x.array()<0.0).select((y.array()>=0).select((y.array()/x.array()).atan()+M_PI,(y.array()/x.array()).atan()-M_PI),(y.array()!=0.0).select(y.array()/y.array().abs()*M_PI/2.0,0)));
}

auto sign(Eigen::VectorXf x)
{
    return x.array()/(x.array().abs()+threshold);
}

auto AngleCvt(Eigen::VectorXf I)
{
    return (I.array()>=0.0).select(I.array(),I.array()+2*M_PI);
}
*/

void TokiColor::doSide(const TempVectorXf &Diff) {
  int tempIndex = 0;
  // Diff.array()+=10.0;ResultDiff+=10.0;
  sideSelectivity[0] = 1e35f;
  sideResult[0] = 0;
  sideSelectivity[1] = 1e35f;
  sideResult[1] = 0;
  if (!needFindSide)
    return;
  // qDebug("开始doSide");
  // qDebug()<<"size(Diff)=["<<Diff.rows()<<','<<Diff.cols()<<']';
  // qDebug()<<"DepthCount="<<(short)DepthCount[0]<<;
  // qDebug()<<"DepthCount=["<<(short)DepthCount[0]<<','<<(short)DepthCount[1]<<','<<(short)DepthCount[2]<<','<<(short)DepthCount[3]<<']';
  // qDebug()<<"DepthIndex=["<<DepthIndexEnd[0]<<','<<DepthIndexEnd[1]<<','<<DepthIndexEnd[2]<<','<<DepthIndexEnd[3]<<']';
  switch (Result % 4) {
  case 3:
    return;
  case 0: // 1,2
    if (DepthCount[1]) {
      sideSelectivity[0] =
          Diff.segment(DepthCount[0], DepthCount[1]).minCoeff(&tempIndex);
      sideResult[0] = Allowed->Map(DepthCount[0] + tempIndex);
    }
    if (DepthCount[2]) {
      sideSelectivity[1] =
          Diff.segment(DepthCount[0] + DepthCount[1], DepthCount[2])
              .minCoeff(&tempIndex);
      sideResult[1] = Allowed->Map(DepthCount[0] + DepthCount[1] + tempIndex);
    }
    break;
  case 1: // 0,2
    if (DepthCount[0]) {
      sideSelectivity[0] = Diff.segment(0, DepthCount[0]).minCoeff(&tempIndex);
      sideResult[0] = Allowed->Map(0 + tempIndex);
    }
    if (DepthCount[2]) {
      sideSelectivity[1] =
          Diff.segment(DepthCount[0] + DepthCount[1], DepthCount[2])
              .minCoeff(&tempIndex);
      sideResult[1] = Allowed->Map(DepthCount[0] + DepthCount[1] + tempIndex);
    }
    break;
  case 2: // 0,1
    if (DepthCount[0]) {
      sideSelectivity[0] = Diff.segment(0, DepthCount[0]).minCoeff(&tempIndex);
      sideResult[0] = Allowed->Map(0 + tempIndex);
    }
    if (DepthCount[1]) {
      sideSelectivity[1] =
          Diff.segment(DepthCount[0], DepthCount[1]).minCoeff(&tempIndex);
      sideResult[1] = Allowed->Map(DepthCount[0] + tempIndex);
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

TokiColor::TokiColor() {
  // c3[0]=0.0f;c3[1]=0.0f;c3[2]=0.0f;
  Result = 0;
}

TokiColor::TokiColor(ARGB rawColor) {
  // convertAlgo=_convertAlgo;

  switch (convertAlgo) {
  case 'r':
  case 'R':
    c3[0] = std::max(getR(rawColor) / 255.0f, threshold);
    c3[1] = std::max(getG(rawColor) / 255.0f, threshold);
    c3[2] = std::max(getB(rawColor) / 255.0f, threshold);
    break;
  case 'H':
    RGB2HSV(getR(rawColor) / 255.0f, getG(rawColor) / 255.0f,
            getB(rawColor) / 255.0f, c3[0], c3[1], c3[2]);
    break;
  case 'X':
    RGB2XYZ(getR(rawColor) / 255.0f, getG(rawColor) / 255.0f,
            getB(rawColor) / 255.0f, c3[0], c3[1], c3[2]);
    break;
  default:
    float X, Y, Z;
    RGB2XYZ(getR(rawColor) / 255.0f, getG(rawColor) / 255.0f,
            getB(rawColor) / 255.0f, X, Y, Z);
    XYZ2Lab(X, Y, Z, c3[0], c3[1], c3[2]);
    break;
  }

  Result = 0;
}

unsigned char TokiColor::apply(ARGB Color) {
  if (getA(Color) <= 0) {
    /*
    ResultDiff=0.0f;
    sideResult[0]=0;sideResult[1]=0;
    sideSelectivity[0]=0.0;sideSelectivity[1]=0;
    */
    return Result = 0;
  } else
    return apply();
}

unsigned char TokiColor::apply() {
  switch (convertAlgo) {
  case 'R':
#ifdef dispFunCalled
    std::cerr << "RGB+\n";
#endif
    return applyRGB_plus();
  case 'r':
#ifdef dispFunCalled
    std::cerr << "RGB\n";
#endif
    return applyRGB();
  case 'H':
#ifdef dispFunCalled
    std::cerr << "HSV\n";
#endif
    return applyHSV();
  case 'L':
#ifdef dispFunCalled
    std::cerr << "Lab00\n";
#endif
    return applyLab_new();
  case 'l':
#ifdef dispFunCalled
    std::cerr << "Lab94\n";
#endif
    return applyLab_old();
  default:
#ifdef dispFunCalled
    std::cerr << "XYZ\n";
#endif
    return applyXYZ();
  }
}

unsigned char TokiColor::applyRGB() {
  if (Result)
    return Result;
  int tempIndex = 0;
  auto Diff0_2 = (Allowed->_RGB.col(0) - c3[0]).square();
  auto Diff1_2 = (Allowed->_RGB.col(1) - c3[1]).square();
  auto Diff2_2 = (Allowed->_RGB.col(2) - c3[2]).square();

  TempVectorXf Diff = Diff0_2 + Diff1_2 + Diff2_2;
  // Data.CurrentColor-=allowedColors;

  ResultDiff = Diff.minCoeff(&tempIndex) + threshold;
  // Diff.minCoeff(tempIndex,u);
  Result = Allowed->Map(tempIndex);
  // qDebug("调色完毕");
  if (needFindSide)
    doSide(Diff);

  return Result;
}

unsigned char TokiColor::applyRGB_plus() {
  if (Result)
    return Result;
  int tempIndex = 0;
  const ColorList &allowedColors = Allowed->_RGB;
  float R = c3[0];
  float g = c3[1];
  float b = c3[2];
  float w_r = 1.0f, w_g = 2.0f, w_b = 1.0f;
  auto SqrModSquare =
      ((R * R + g * g + b * b) *
       (allowedColors.col(0).square() + allowedColors.col(1).square() +
        allowedColors.col(2).square()))
          .sqrt();
  auto deltaR = (R - allowedColors.col(0));
  auto deltaG = (g - allowedColors.col(1));
  auto deltaB = (b - allowedColors.col(2));
  auto SigmaRGB = (R + g + b + allowedColors.col(0) + allowedColors.col(1) +
                   allowedColors.col(2)) /
                  3.0f;
  auto S_r =
      ((allowedColors.col(0) + R) < SigmaRGB)
          .select((allowedColors.col(0) + R) / (SigmaRGB + threshold), 1.0f);
  auto S_g =
      ((allowedColors.col(1) + g) < SigmaRGB)
          .select((allowedColors.col(1) + g) / (SigmaRGB + threshold), 1.0f);
  auto S_b =
      ((allowedColors.col(2) + b) < SigmaRGB)
          .select((allowedColors.col(2) + b) / (SigmaRGB + threshold), 1.0f);
  auto sumRGBsquare = R * allowedColors.col(0) + g * allowedColors.col(1) +
                      b * allowedColors.col(2);
  auto theta =
      2.0 / M_PI * (sumRGBsquare / (SqrModSquare + threshold) / 1.01f).acos();
  auto OnedDeltaR = deltaR.abs() / (R + allowedColors.col(0) + threshold);
  auto OnedDeltaG = deltaG.abs() / (g + allowedColors.col(1) + threshold);
  auto OnedDeltaB = deltaB.abs() / (b + allowedColors.col(2) + threshold);
  auto sumOnedDelta = OnedDeltaR + OnedDeltaG + OnedDeltaB + threshold;
  auto S_tr = OnedDeltaR / sumOnedDelta * S_r.square();
  auto S_tg = OnedDeltaG / sumOnedDelta * S_g.square();
  auto S_tb = OnedDeltaB / sumOnedDelta * S_b.square();
  auto S_theta = S_tr + S_tg + S_tb;
  auto Rmax = allowedColors.rowwise().maxCoeff();
  auto S_ratio = Rmax.max(std::max(R, std::max(g, b)));

  TempVectorXf dist =
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
  Result = Allowed->Map(tempIndex);
  if (needFindSide)
    doSide(dist);

  return Result;
}

unsigned char TokiColor::applyHSV() {
  if (Result)
    return Result;
  int tempIndex = 0;
  const ColorList &allowedColors = Allowed->HSV;

  auto S_times_V = allowedColors.col(1) * allowedColors.col(2);
  const float s_times_v = c3[1] * c3[2];
  auto deltaX = 50.0f * (allowedColors.col(0).cos() * S_times_V -
                         s_times_v * std::cos(c3[0]));
  auto deltaY = 50.0f * (allowedColors.col(0).sin() * S_times_V -
                         s_times_v * std::sin(c3[0]));
  auto deltaZ = 50.0f * (allowedColors.col(2) - c3[2]);
  TempVectorXf Diff = deltaX.square() + deltaY.square() + deltaZ.square();

  // std::cerr<<"Diff.isNaN().count()="<<Diff.isNaN().count()<<"\n";

  ResultDiff = Diff.minCoeff(&tempIndex);
  Result = Allowed->Map(tempIndex);
  if (needFindSide)
    doSide(Diff);
  return Result;
}

unsigned char TokiColor::applyXYZ() {
  if (Result)
    return Result;
  int tempIndex = 0;
  auto Diff0_2 = (Allowed->XYZ.col(0) - c3[0]).square();
  auto Diff1_2 = (Allowed->XYZ.col(1) - c3[1]).square();
  auto Diff2_2 = (Allowed->XYZ.col(2) - c3[2]).square();

  TempVectorXf Diff = Diff0_2 + Diff1_2 + Diff2_2;
  // Data.CurrentColor-=allowedColors;
  ResultDiff = Diff.minCoeff(&tempIndex);
  // Diff.minCoeff(tempIndex,u);
  Result = Allowed->Map(tempIndex);
  if (needFindSide)
    doSide(Diff);
  return Result;
}

unsigned char TokiColor::applyLab_old() {
  if (Result)
    return Result;
  int tempIndex = 0;
  float L = c3[0];
  float a = c3[1];
  float b = c3[2];
  const ColorList &allowedColors = Allowed->Lab;
  auto deltaL_2 = (allowedColors.col(0) - L).square();
  float C1_2 = a * a + b * b;
  TempVectorXf C2_2 =
      allowedColors.col(1).square() + allowedColors.col(2).square();
  auto deltaCab_2 = (sqrt(C1_2) - C2_2.sqrt()).square();
  auto deltaHab_2 = (allowedColors.col(1) - a).square() +
                    (allowedColors.col(2) - b).square() - deltaCab_2;
  // SL=1,kL=1
  // K1=0.045f
  // K2=0.015f
  float SC_2 = (sqrt(C1_2) * 0.045f + 1.0f) * (sqrt(C1_2) * 0.045f + 1.0f);
  auto SH_2 = (C2_2.sqrt() * 0.015f + 1.0f).square();
  TempVectorXf Diff = deltaL_2 + deltaCab_2 / SC_2 + deltaHab_2 / SH_2;
  ResultDiff = Diff.minCoeff(&tempIndex);
  Result = Allowed->Map(tempIndex);
  if (needFindSide)
    doSide(Diff);
  return Result;
}

unsigned char TokiColor::applyLab_new() {
  if (Result)
    return Result;
  int tempIndex = 0;
  float L1s = c3[0];
  float a1s = c3[1];
  float b1s = c3[2];
  const ColorList &allow = Allowed->Lab;
  TempVectorXf Diff(allow.rows());

  for (short i = 0; i < allow.rows(); i++) {
    Diff(i) = Lab00_diff(L1s, a1s, b1s, allow(i, 0), allow(i, 1), allow(i, 2));
  }

  /*
  auto L2s=allow.col(0).array();
  auto a2s=allow.col(1).array();
  auto b2s=allow.col(2).array();

  auto C1abs=sqrt(a1s*a1s+b1s*b1s);
  auto C2abs=(a2s.square()+b2s.square()).sqrt();
  auto mCabs=(C1abs+C2abs)/2.0;
  auto G=0.5*(1.0-(mCabs.pow(7)/(mCabs.pow(7)+pow(25,7))));
  auto a1p=(1.0+G)*a1s;
  auto a2p=(1.0+G)*a2s;
  auto C1p=(a1p.square()+b1s*b1s).sqrt();
  auto C2p=(a2p.square()+b2s.square()).sqrt();
  VectorXf h1p=AngleCvt(atan2(L2s*0.0+b1s,a1p));
  VectorXf h2p=AngleCvt(atan2(b2s,a2p));

  auto dLp=L2s-L1s;
  auto dCp=C2p-C1p;
  auto h2p_h1p=(h2p-h1p).array();
  auto C1pC2p=C1p*C2p;
  auto addon4dhp=(h2p_h1p.abs()>M_PI).select(-2.0*M_PI*sign(h2p_h1p),0.0);
  auto dhp=(C1pC2p!=0.0).select(h2p_h1p+addon4dhp,0.0);
  auto dHp=2*C1pC2p.sqrt()*(dhp*0.5).sin();

  auto mLp=(L1s+L2s)/2.0;

  auto mCp=(C1p+C2p)/2.0;

  auto h1p_add_h2p=(h1p+h2p).array();

  auto
  addon4mhp=(h2p_h1p.abs()>M_PI).select((h1p_add_h2p>=2.0*M_PI).select(-2*M_PI+L2s*0.0,2.0*M_PI),0.0);
  auto mhp=(C1pC2p!=0).select((h1p_add_h2p+addon4mhp)/2.0,h1p_add_h2p);
  auto
  T=1-0.17*(mhp-deg2rad(30.0)).cos()+0.24*(2.0*mhp).cos()+0.32*(3.0*mhp+deg2rad(6.0)).cos()-0.2*(4.0*mhp-deg2rad(63.0)).cos();
  auto dTheta=deg2rad(30)*(-(mhp-deg2rad(275.0)/25.0).square()).exp();
  auto Rc=2*(mCp.pow(7)/(mCp.pow(7)+pow(25.0,7))).sqrt();

  auto SL=1+0.015*(mLp-50).square()/(20.0+(mLp-50).square()).sqrt();
  auto SC=1+0.045*mCp;
  auto SH=1+0.015*mCp*T;
  auto RT=-Rc*(2.0*dTheta).sin();

  auto
  Diff=(dLp/SL/1.0).square()+(dCp/SC/1.0).square()+(dHp/SH/1.0).square()+RT*(dCp/SC/1.0)*(dHp/SH/1.0);
*/
  Diff.abs().minCoeff(&tempIndex);
  /*
  if(Diff.isNaN().any())
  {
      std::cerr<<"Found NaN"<<std::endl;
      std::cout<<Diff.transpose()<<std::endl;
  }*/
  ResultDiff = Diff.minCoeff(&tempIndex);
  // Diff.minCoeff(tempIndex,u);
  Result = Allowed->Map(tempIndex);
  if (needFindSide)
    doSide(Diff);
  return Result;
}

#endif
