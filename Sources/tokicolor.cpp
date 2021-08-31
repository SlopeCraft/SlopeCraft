/*
 Copyright © 2021  TokiNoBug
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
#include "mainwindow.h"
#define Thre 1e-10f
#include <cmath>
#define deg2rad(deg) ((deg)*M_PI/180.0)
auto atan2(VectorXf y,VectorXf x)
{
    return (x.array()>0.0).select((y.array()/x.array()).atan(),(x.array()<0.0).select((y.array()>=0).select((y.array()/x.array()).atan()+M_PI,(y.array()/x.array()).atan()-M_PI),(y.array()!=0.0).select(y.array()/y.array().abs()*M_PI/2.0,0)));
    /*x=(x.array()!=0.0).select(x,Thre);
    auto adder=(x.array()<=0).select((y.array()<=0).select(y*0.0,2*M_PI),M_PI);

    return adder.array()+(y.array()/x.array()).atan();*/
    /*auto t=(x.array().abs()>Thre).select((y.array()/x.array()).atan(),0.0);
    return (x.array()<0.0).select(t+M_PI,(y.array()>=0).select(t,t+2*M_PI));*/
}

auto sign(VectorXf x)
{
    return x.array()/(x.array().abs()+Thre);
}

auto AngleCvt(VectorXf I)
{
    return (I.array()>=0.0).select(I.array(),I.array()+2*M_PI);
}

void TokiColor::doSide(VectorXf Diff)
{
    int tempIndex=0;
    //Diff.array()+=10.0;ResultDiff+=10.0;
    sideSelectivity[0]=1e35f;sideResult[0]=0;
    sideSelectivity[1]=1e35f;sideResult[1]=0;
    if(!needFindSide) return;
    //qDebug("开始doSide");
    //qDebug()<<"size(Diff)=["<<Diff.rows()<<','<<Diff.cols()<<']';
    //qDebug()<<"DepthCount="<<(short)DepthCount[0]<<;
    //qDebug()<<"DepthCount=["<<(short)DepthCount[0]<<','<<(short)DepthCount[1]<<','<<(short)DepthCount[2]<<','<<(short)DepthCount[3]<<']';
    //qDebug()<<"DepthIndex=["<<DepthIndexEnd[0]<<','<<DepthIndexEnd[1]<<','<<DepthIndexEnd[2]<<','<<DepthIndexEnd[3]<<']';
    switch (Result%4)
    {
    case 3:
        return;
    case 0://1,2
        if(DepthCount[1])
        {
            sideSelectivity[0]=Diff.segment(DepthIndexEnd[0]+1,DepthCount[1]).minCoeff(&tempIndex);
            sideResult[0]=Allowed->Map(DepthIndexEnd[0]+1+tempIndex);
        }
        if(DepthCount[2])
        {
            sideSelectivity[1]=Diff.segment(DepthIndexEnd[1]+1,DepthCount[2]).minCoeff(&tempIndex);
            sideResult[1]=Allowed->Map(DepthIndexEnd[1]+1+tempIndex);
        }
        break;
    case 1://0,2
        if(DepthCount[0])
        {
            sideSelectivity[0]=Diff.segment(0,DepthCount[0]).minCoeff(&tempIndex);
            sideResult[0]=Allowed->Map(tempIndex);
        }
        if(DepthCount[2])
        {
            sideSelectivity[1]=Diff.segment(DepthIndexEnd[1]+1,DepthCount[2]).minCoeff(&tempIndex);
            sideResult[1]=Allowed->Map(DepthIndexEnd[1]+1+tempIndex);
        }
        break;
    case 2://0,1
        if(DepthCount[0])
        {
            sideSelectivity[0]=Diff.segment(0,DepthCount[0]).minCoeff(&tempIndex);
            sideResult[0]=Allowed->Map(tempIndex);
        }
        if(DepthCount[1])
        {
            sideSelectivity[1]=Diff.segment(DepthIndexEnd[0]+1,DepthCount[1]).minCoeff(&tempIndex);
            sideResult[1]=Allowed->Map(DepthIndexEnd[0]+1+tempIndex);
        }
        break;
    }
    //sideSelectivity[0]-=1.0;sideSelectivity[1]-=1.0;
    //sideSelectivity[0]*=100.0;sideSelectivity[1]*=100.0;
    qDebug()<<"side[0]=["<<sideResult[0]<<','<<sideSelectivity[0]<<']';
    qDebug()<<"side[1]=["<<sideResult[1]<<','<<sideSelectivity[1]<<']';
    qDebug()<<"ResultDiff="<<ResultDiff;
    return;
}

void f(float &I)
{
    if (I>0.008856f)
    I=std::pow(I,1.0f/3.0f);
    else
    I=7.787f*I+16.0f/116.0f;
    return;
}

void invf(float&I)
{
    if(I>std::pow(0.008856,1.0/3.0))
        I=I*I*I;
    else
        I=(I-16.0/116.0)/7.787;
    return;
}


TokiColor::TokiColor()
{
    ColorSpaceType='R';
    c3[0]=0.0f;c3[1]=0.0f;c3[2]=0.0f;
}

void RGB2HSV(float r, float g, float b,  float &h, float &s, float &v)
{
    float K = 0.0f;
    if (g < b)
    {
        std::swap(g, b);
        K = -1.0f;
    }
    if (r < g)
    {
        std::swap(r, g);
        K = -2.0f / 6.0f - K;
    }
    float chroma = r - std::min(g, b);
    h = fabs(K + (g - b) / (6.0f * chroma + Thre));
    s = chroma / (r + Thre);
    v = r;

    return;
}

void HSV2RGB(float H,float S,float V,float&r,float&g,float&b)
{
    float C=V*S;
    float X=C*(1-abs(int(H/deg2rad(60.0))%2-1));
    float m=V-C;
    if(H<deg2rad(60))
    {
        r=C+m;
        g=X+m;
        b=0+m;
        return;
    }
    if(H<deg2rad(120))
    {
        r=X+m;
        g=C+m;
        b=0+m;
        return;
    }
    if(H<deg2rad(180))
    {
        r=0+m;
        g=C+m;
        b=X+m;
        return;
    }
    if(H<deg2rad(240))
    {
        r=0+m;
        g=X+m;
        b=C+m;
        return;
    }
    if(H<deg2rad(300))
    {
        r=X+m;
        g=0+m;
        b=C+m;
        return;
    }

        r=C+m;
        g=0+m;
        b=X+m;
        return;

}

void RGB2XYZ(float R, float G, float B, float &X, float &Y, float &Z)
{
    X = 0.412453f * R + 0.357580f * G + 0.180423f * B;
    Y = 0.212671f * R + 0.715160f * G + 0.072169f * B;
    Z = 0.019334f * R + 0.119193f * G + 0.950227f * B;
    return;
}

void XYZ2Lab(float X, float Y, float Z, float &L, float &a, float &b)
{
    X/=0.9504f;f(X);
    Y/=1.0f;   f(Y);
    Z/=1.0888f;f(Z);
    L=116.0f*X-16.0f;
    a=500.0f*(X-Y);
    b=200.0f*(Y-Z);
    return;
}

void Lab2XYZ(float L,float a,float b,float&X,float&Y,float&Z)
{
    L+=16.0;
    X=0.008620689655172*L;
    Y=X-0.002*a;
    Z=Y-0.005*b;
    /*
[X';Y';Z']=[0.008620689655172,0,0;
0.008620689655172,-0.002,0;
0.008620689655172,-0.002,-0.005]*[L+16;a;b]
*/
    invf(X);invf(Y);invf(Z);
    X*=0.9504f;Y*=1.0f;Z*=1.0888f;
}

inline float squeeze01(float t)
{
    if(t<0.0)return 0.0f;
    if(t>1.0)return 1.0f;
    return t;
}

QRgb RGB2QRGB(float r,float g,float b)
{
    return qRgb(255*squeeze01(r),255*squeeze01(g),255*squeeze01(b));
}

QRgb XYZ2QRGB(float x,float y,float z)
{
    /*
[3.2404814,-1.5371516,-0.4985363;
-0.9692550,1.8759900,0.0415559;
0.0556466,-0.2040413,1.0573111]
*/
    return RGB2QRGB(3.2404814*x-1.5371516*y-0.4985363*z,-0.9692550*x+1.8759900*y+0.0415559*z,0.0556466*x-0.2040413*y+1.0573111*z);
}

QRgb Lab2QRGB(float L,float a,float b)
{
float x,y,z;
Lab2XYZ(L,a,b,x,y,z);
return XYZ2QRGB(x,y,z);
}

QRgb HSV2QRGB(float H,float S,float V)
{
    float r,g,b;
    HSV2RGB(H,S,V,r,g,b);
    return RGB2QRGB(r,g,b);
}


TokiColor::TokiColor(const QRgb& rawColor,char _ColorSpaceType)
{
ColorSpaceType=_ColorSpaceType;
if (_ColorSpaceType>='a')
    _ColorSpaceType-='a'-'A';
switch (_ColorSpaceType)
{
case 'R':
    c3[0]=max(qRed(rawColor)/255.0f,Thre);
    c3[1]=max(qGreen(rawColor)/255.0f,Thre);
    c3[2]=max(qBlue(rawColor)/255.0f,Thre);
    break;
case 'H':
    RGB2HSV(qRed(rawColor)/255.0f,qGreen(rawColor)/255.0f,qBlue(rawColor)/255.0f,c3[0],c3[1],c3[2]);
    break;
case 'X':
    RGB2XYZ(qRed(rawColor)/255.0f,qGreen(rawColor)/255.0f,qBlue(rawColor)/255.0f,c3[0],c3[1],c3[2]);
    break;
default:
    float X,Y,Z;
    RGB2XYZ(qRed(rawColor)/255.0f,qGreen(rawColor)/255.0f,qBlue(rawColor)/255.0f,X,Y,Z);
    XYZ2Lab(X,Y,Z,c3[0],c3[1],c3[2]);
    break;
}

Result=0;
}


unsigned char TokiColor::apply(QRgb Color)
{
    if(qAlpha(Color)<=0)
    {
        ResultDiff=0.0f;
        return Result=0;
    }
    else
        return apply();
}

unsigned char TokiColor::apply()
{
switch (ColorSpaceType)
{
case 'R':
    return applyRGB_plus();
case 'r':
    return applyRGB();
case 'H':
    return applyHSV();
case 'L':
    return applyLab_new();
case 'l':
    return applyLab_old();
default:
    return applyXYZ();
}
}


unsigned char TokiColor::applyRGB()
{
    if(Result)return Result;
    int tempIndex=0;
    auto Diff0_2=(Allowed->_RGB.col(0)-c3[0]).square();
    auto Diff1_2=(Allowed->_RGB.col(1)-c3[1]).square();
    auto Diff2_2=(Allowed->_RGB.col(2)-c3[2]).square();

    auto Diff=Diff0_2+Diff1_2+Diff2_2;
    //Data.CurrentColor-=allowedColors;

    ResultDiff=Diff.minCoeff(&tempIndex)+Thre;
    //Diff.minCoeff(tempIndex,u);
    Result=Allowed->Map(tempIndex);
    //qDebug("调色完毕");
    if(needFindSide)
    doSide(Diff);

    return Result;
}

unsigned char TokiColor::applyRGB_plus()
{
    if(Result)return Result;
    int tempIndex=0;
    ArrayXXf &allowedColors=Allowed->_RGB;
    float R=c3[0];
    float g=c3[1];
    float b=c3[2];
    float w_r=1.0f,w_g=2.0f,w_b=1.0f;
    auto SqrModSquare=((R*R+g*g+b*b)*(allowedColors.col(0).array().square()+allowedColors.col(1).array().square()+allowedColors.col(2).array().square())).sqrt();
    auto deltaR=(R-allowedColors.col(0).array());
    auto deltaG=(g-allowedColors.col(1).array());
    auto deltaB=(b-allowedColors.col(2).array());
    auto SigmaRGB=(R+g+b+allowedColors.col(0).array()+allowedColors.col(1).array()+allowedColors.col(2).array())/3.0f;
    auto S_r=((allowedColors.col(0).array()+R)<SigmaRGB).select((allowedColors.col(0).array()+R)/(SigmaRGB+Thre),1.0f);
    auto S_g=((allowedColors.col(1).array()+g)<SigmaRGB).select((allowedColors.col(1).array()+g)/(SigmaRGB+Thre),1.0f);
    auto S_b=((allowedColors.col(2).array()+b)<SigmaRGB).select((allowedColors.col(2).array()+b)/(SigmaRGB+Thre),1.0f);
    auto sumRGBsquare=R*allowedColors.col(0).array()+g*allowedColors.col(1).array()+b*allowedColors.col(2).array();
    auto theta=2.0/M_PI*(sumRGBsquare/(SqrModSquare+Thre)/1.01f).acos();
    auto OnedDeltaR=deltaR.abs()/(R+allowedColors.col(0).array()+Thre);
    auto OnedDeltaG=deltaG.abs()/(g+allowedColors.col(1).array()+Thre);
    auto OnedDeltaB=deltaB.abs()/(b+allowedColors.col(2).array()+Thre);
    auto sumOnedDelta=OnedDeltaR+OnedDeltaG+OnedDeltaB+Thre;
    auto S_tr=OnedDeltaR/sumOnedDelta*S_r.square();
    auto S_tg=OnedDeltaG/sumOnedDelta*S_g.square();
    auto S_tb=OnedDeltaB/sumOnedDelta*S_b.square();
    auto S_theta=S_tr+S_tg+S_tb;
    auto Rmax=allowedColors.rowwise().maxCoeff();
    auto S_ratio=Rmax.array().max(max(R,max(g,b)));

    auto dist=(S_r.square()*w_r*deltaR.square()+S_g.square()*w_g*deltaG.square()+S_b.square()*w_b*deltaB.square())/(w_r+w_g+w_b)+S_theta*S_ratio*theta.square();//+S_theta*S_ratio*theta.square()

    ResultDiff=dist.minCoeff(&tempIndex);
    /*if(dist.isNaN().any()){
        qDebug("出现Nan");
            if(SqrModSquare.isNaN().any())      qDebug("SqrModSquare出现Nan");
            if(theta.isNaN().any())                      qDebug("theta出现Nan");
            if(sumOnedDelta.isNaN().any())      qDebug("sumOnedDelta出现Nan");
            if(S_ratio.isNaN().any())                  qDebug("S_ratio出现Nan");

    }*/
    Result=Allowed->Map(tempIndex);
    if(needFindSide)
    doSide(dist);

    return Result;
}

unsigned char TokiColor::applyHSV()
{
    if(Result)return Result;
    int tempIndex=0;
    ArrayXXf &allowedColors=Allowed->HSV;
    //float h=c3[0];
    //float s=c3[1];
    //float v=c3[2];
    auto SV=allowedColors.col(1).array()*allowedColors.col(2).array();
    auto deltaX=50.0f*(SV*(allowedColors.col(0).array()).cos()-c3[2]*c3[1]*cos(c3[0]));
    auto deltaY=50.0f*(SV*(allowedColors.col(0).array()).sin()-c3[2]*c3[1]*sin(c3[0]));
    auto deltaZ=86.60254f*(allowedColors.col(2).array()-c3[2]);
    auto Diff=deltaX.square()+deltaY.square()+deltaZ.square();
    ResultDiff=Diff.minCoeff(&tempIndex);
    Result=Allowed->Map(tempIndex);
    if(needFindSide)
    doSide(Diff);
    return Result;
}

unsigned char TokiColor::applyXYZ()
{
    if(Result)return Result;
    int tempIndex=0;
    auto Diff0_2=(Allowed->XYZ.col(0).array()-c3[0]).square();
    auto Diff1_2=(Allowed->XYZ.col(1).array()-c3[1]).square();
    auto Diff2_2=(Allowed->XYZ.col(2).array()-c3[2]).square();

    auto Diff=Diff0_2+Diff1_2+Diff2_2;
    //Data.CurrentColor-=allowedColors;

    ResultDiff=Diff.minCoeff(&tempIndex);
    //Diff.minCoeff(tempIndex,u);
    Result=Allowed->Map(tempIndex);
    if(needFindSide)
    doSide(Diff);
    return Result;
}

unsigned char TokiColor::applyLab_old()
{
    if(Result)return Result;
    int tempIndex=0;
    float L=c3[0];
    float a=c3[1];
    float b=c3[2];
    ArrayXXf &allowedColors=Allowed->Lab;
    auto deltaL_2=(allowedColors.col(0).array()-L).square();
    auto C1_2=a*a+b*b;
    auto C2_2=allowedColors.col(1).array().square()+allowedColors.col(2).array().square();
    auto deltaCab_2=(sqrt(C1_2)-C2_2.array().sqrt()).square();
    auto deltaHab_2=(allowedColors.col(1).array()-a).square()+(allowedColors.col(2).array()-b).square()-deltaCab_2;
    //SL=1,kL=1
    //K1=0.045f
    //K2=0.015f
    float SC_2=(sqrt(C1_2)*0.045f+1.0f)*(sqrt(C1_2)*0.045f+1.0f);
    auto SH_2=(C2_2.sqrt()*0.015f+1.0f).square();
    auto Diff=deltaL_2+deltaCab_2/SC_2+deltaHab_2/SH_2;
    ResultDiff=Diff.minCoeff(&tempIndex);
    Result=Allowed->Map(tempIndex);
    if(needFindSide)
    doSide(Diff);
    return Result;
}

unsigned char TokiColor::applyLab_new()
{
    if(Result)return Result;
    int tempIndex=0;
    float L1s=c3[0];
    float a1s=c3[1];
    float b1s=c3[2];
    ArrayXXf &allow=Allowed->Lab;
    ArrayXf Diff(allow.rows());

    for(short i=0;i<allow.rows();i++)
    {
        Diff(i)=Lab00(L1s,a1s,b1s,allow(i,0),allow(i,1),allow(i,2));
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

    auto addon4mhp=(h2p_h1p.abs()>M_PI).select((h1p_add_h2p>=2.0*M_PI).select(-2*M_PI+L2s*0.0,2.0*M_PI),0.0);
    auto mhp=(C1pC2p!=0).select((h1p_add_h2p+addon4mhp)/2.0,h1p_add_h2p);
    auto T=1-0.17*(mhp-deg2rad(30.0)).cos()+0.24*(2.0*mhp).cos()+0.32*(3.0*mhp+deg2rad(6.0)).cos()-0.2*(4.0*mhp-deg2rad(63.0)).cos();
    auto dTheta=deg2rad(30)*(-(mhp-deg2rad(275.0)/25.0).square()).exp();
    auto Rc=2*(mCp.pow(7)/(mCp.pow(7)+pow(25.0,7))).sqrt();

    auto SL=1+0.015*(mLp-50).square()/(20.0+(mLp-50).square()).sqrt();
    auto SC=1+0.045*mCp;
    auto SH=1+0.015*mCp*T;
    auto RT=-Rc*(2.0*dTheta).sin();

    auto Diff=(dLp/SL/1.0).square()+(dCp/SC/1.0).square()+(dHp/SH/1.0).square()+RT*(dCp/SC/1.0)*(dHp/SH/1.0);
*/
    Diff.abs().minCoeff(&tempIndex);
    if(Diff.isNaN().any())
    {
        qDebug("存在NaN");
        cout<<Diff.transpose()<<endl;
    }
    ResultDiff=Diff.minCoeff(&tempIndex);
    //Diff.minCoeff(tempIndex,u);
    Result=Allowed->Map(tempIndex);
    if(needFindSide)
    doSide(Diff);
    return Result;
}

#endif
