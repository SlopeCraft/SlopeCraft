//#pragma once
#include "mainwindow.h"
#define Thre 1e-8f
#include <cmath>

auto atan2(VectorXf y,VectorXf x)
{
    x=(x.array()!=0.0).select(x,1e-10f);
    auto adder=(x.array()<=0).select((y.array()<=0).select(y*0.0,2*M_PI),M_PI);

    return adder.array()+(y.array()/x.array()).atan();
    /*auto t=(x.array().abs()>1e-4f).select((y.array()/x.array()).atan(),0.0);
    return (x.array()<0.0).select(t+M_PI,(y.array()>=0).select(t,t+2*M_PI));*/
}

void TokiColor::doSide(VectorXf Diff,float ResultDiff)
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
    h = fabs(K + (g - b) / (6.0f * chroma + 1e-20f));
    s = chroma / (r + 1e-20f);
    v = r;

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

TokiColor::TokiColor(const QRgb& rawColor,char _ColorSpaceType)
{
ColorSpaceType=_ColorSpaceType;
if (_ColorSpaceType>='a')
    _ColorSpaceType-='a'-'A';
switch (_ColorSpaceType)
{
case 'R':
    c3[0]=max(qRed(rawColor)/255.0f,1e-10f);
    c3[1]=max(qGreen(rawColor)/255.0f,1e-10f);
    c3[2]=max(qBlue(rawColor)/255.0f,1e-10f);

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
        return 0;
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
    return applyLab_old();
default:
    return applyXYZ();
}
}


unsigned char TokiColor::applyRGB()
{
    if(Result)return Result;
    int tempIndex=0;
    auto Diff0_2=(Allowed->_RGB.col(0).array()-c3[0]).square();
    auto Diff1_2=(Allowed->_RGB.col(1).array()-c3[1]).square();
    auto Diff2_2=(Allowed->_RGB.col(2).array()-c3[2]).square();

    auto Diff=Diff0_2+Diff1_2+Diff2_2;
    //Data.CurrentColor-=allowedColors;

    float ResultDiff=Diff.minCoeff(&tempIndex)+1e-8f;
    //Diff.minCoeff(tempIndex,u);
    Result=Allowed->Map(tempIndex);
    //qDebug("调色完毕");
    if(needFindSide)
    doSide(Diff,ResultDiff);

    return Result;
}

unsigned char TokiColor::applyRGB_plus()
{
    if(Result)return Result;
    int tempIndex=0;
    MatrixXf &allowedColors=Allowed->_RGB;
    float R=c3[0];
    float g=c3[1];
    float b=c3[2];
    float w_r=1.0f,w_g=2.0f,w_b=1.0f;
    auto SqrModSquare=((R*R+g*g+b*b)*(allowedColors.col(0).array().square()+allowedColors.col(1).array().square()+allowedColors.col(2).array().square())).sqrt();
    auto deltaR=(R-allowedColors.col(0).array());
    auto deltaG=(g-allowedColors.col(1).array());
    auto deltaB=(b-allowedColors.col(2).array());
    auto SigmaRGB=(R+g+b+allowedColors.col(0).array()+allowedColors.col(1).array()+allowedColors.col(2).array())/3.0f;
    auto S_r=((allowedColors.col(0).array()+R)<SigmaRGB).select((allowedColors.col(0).array()+R)/SigmaRGB,1.0f);
    auto S_g=((allowedColors.col(1).array()+g)<SigmaRGB).select((allowedColors.col(1).array()+g)/SigmaRGB,1.0f);
    auto S_b=((allowedColors.col(2).array()+b)<SigmaRGB).select((allowedColors.col(2).array()+b)/SigmaRGB,1.0f);
    auto sumRGBsquare=R*allowedColors.col(0).array()+g*allowedColors.col(1).array()+b*allowedColors.col(2).array();
    auto theta=2.0/M_PI*(sumRGBsquare/SqrModSquare).acos();
    auto OnedDeltaR=deltaR.abs()/(R+allowedColors.col(0).array());
    auto OnedDeltaG=deltaG.abs()/(g+allowedColors.col(1).array());
    auto OnedDeltaB=deltaB.abs()/(b+allowedColors.col(2).array());
    auto sumOnedDelta=OnedDeltaR+OnedDeltaG+OnedDeltaB+1e-10f;
    auto S_tr=OnedDeltaR/sumOnedDelta*S_r.square();
    auto S_tg=OnedDeltaG/sumOnedDelta*S_g.square();
    auto S_tb=OnedDeltaB/sumOnedDelta*S_b.square();
    auto S_theta=S_tr+S_tg+S_tb;
    auto Rmax=allowedColors.rowwise().maxCoeff();
    auto S_ratio=Rmax.array().max(max(R,max(g,b)));


    auto dist=(S_r.square()*w_r*deltaR.square()+S_g.square()*w_g*deltaG.square()+S_b.square()*w_b*deltaB.square())/(w_r+w_g+w_b)+S_theta*S_ratio*theta.square();//+S_theta*S_ratio*theta.square()

    float ResultDiff=dist.minCoeff(&tempIndex);
    //if(dist.isNaN().any())qDebug("出现Nan");
    Result=Allowed->Map(tempIndex);
    if(needFindSide)
    doSide(dist,ResultDiff);

    return Result;
}

unsigned char TokiColor::applyHSV()
{
    if(Result)return Result;
    int tempIndex=0;
    MatrixXf &allowedColors=Allowed->HSV;
    //float h=c3[0];
    //float s=c3[1];
    //float v=c3[2];
    auto SV=allowedColors.col(1).array()*allowedColors.col(2).array();
    auto deltaX=50.0f*(SV*(M_2_PI*allowedColors.col(0).array()).cos()-c3[2]*c3[1]*cos(M_2_PI*c3[0]));
    auto deltaY=50.0f*(SV*(M_2_PI*allowedColors.col(0).array()).sin()-c3[2]*c3[1]*sin(M_2_PI*c3[0]));
    auto deltaZ=86.60254f*(allowedColors.col(2).array()-c3[2]);
    auto Diff=deltaX.square()+deltaY.square()+deltaZ.square();
    float ResultDiff=Diff.minCoeff(&tempIndex);
    Result=Allowed->Map(tempIndex);
    if(needFindSide)
    doSide(Diff,ResultDiff);
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

    float ResultDiff=Diff.minCoeff(&tempIndex);
    //Diff.minCoeff(tempIndex,u);
    Result=Allowed->Map(tempIndex);
    if(needFindSide)
    doSide(Diff,ResultDiff);
    return Result;
}

unsigned char TokiColor::applyLab_old()
{
    if(Result)return Result;
    int tempIndex=0;
    float L=c3[0];
    float a=c3[1];
    float b=c3[2];
    MatrixXf &allowedColors=Allowed->Lab;
     auto deltaL_2=(allowedColors.col(0).array()-L).square();
    float C1_2=a*a+b*b;
    auto C2_2=allowedColors.col(1).array().square()+allowedColors.col(2).array().square();
    auto deltaCab_2=(sqrt(C1_2)-C2_2.array().sqrt()).square();
    auto deltaHab_2=(allowedColors.col(1).array()-a).square()+(allowedColors.col(2).array()-b).square()-deltaCab_2;
    //SL=1,kL=1
    //K1=0.045f
    //K2=0.015f
    float SC_2=(sqrt(C1_2)*0.045f+1.0f)*(sqrt(C1_2)*0.045f+1.0f);
    auto SH_2=(C2_2.sqrt()*0.015f+1.0f).square();

    auto Diff=deltaL_2+deltaCab_2/SC_2+deltaHab_2/SH_2;


    float ResultDiff=Diff.minCoeff(&tempIndex);
    Result=Allowed->Map(tempIndex);
    if(needFindSide)
    doSide(Diff,ResultDiff);
    return Result;
}

unsigned char TokiColor::applyLab_new()
{
    if(Result)return Result;
    int tempIndex=0;
    //float L_1=c3[0];
    //float a_1=c3[1];
    //float b_1=c3[2];
    MatrixXf &allow=Allowed->Lab;

    auto Cs1ab=sqrt(c3[1]*c3[1]+c3[2]*c3[2]);
    auto Cs2ab=(allow.col(1).array().square()+allow.col(2).array().square()).sqrt();
    auto mCsab=(Cs1ab+Cs2ab)/2.0f;
    auto mCsab7=mCsab.pow(7);
    auto G=0.5f*(1.0f-(1.0f-pow(25,7)/(mCsab7+pow(25,7))).sqrt());
    auto ap1=(1.0f+G)*c3[1];
    auto ap2=(1.0f+G)*allow.col(1).array();
    auto Cp1=(ap1.square()+c3[2]*c3[2]).sqrt();
    auto Cp2=(ap2.square()+allow.col(2).array().square()).sqrt();

    auto hp1=(ap1.abs().min(abs(c3[2])>1e-4f)).select(atan2(ap1*0+c3[2],ap1).array(),0.0f);
    //qDebug()<<"a";
    auto hp2=(ap2.abs().min(allow.col(2).array().abs())>1e-4f).select(atan2(allow.col(2).array(),ap2).array(),0.0f);
    //qDebug()<<"b";
    auto dLp=allow.col(0).array()-c3[0];
    auto dCp=Cp2-Cp1;
    auto Cp1Cp2=(Cp1*Cp2).abs();
    auto hp2_1=hp2-hp1;
    //qDebug()<<"c";
    auto addon=(hp2_1.abs()<=M_PI).select(hp2_1*0.0f,(hp2_1>0).select(hp2_1*0.0f+M_PI,-M_PI));
    auto dhp=(Cp1Cp2>1e-4f).select(hp2_1+addon,0.0f);
    //qDebug()<<"d";
    auto dHp=2.0f*(Cp1Cp2).sqrt()*(dhp/2.0f).sin();
    //qDebug()<<"e";
    auto mLp=(c3[0]+allow.col(0).array())/2.0f;
    auto mCp=(Cp1+Cp2)/2.0f;
    //qDebug()<<"f";
    auto hp21=hp2+hp1;
    auto addon2=(hp2_1.abs()<=M_PI).select(hp2_1*0.0f,(hp21<2*M_PI).select(hp21*0.0f+M_PI,-M_PI));
    auto mhp=(Cp1Cp2>1e-4f).select(hp21/2.0f+addon2,hp21);
    //qDebug()<<"g";
    auto T=1.0f-0.17f*(mhp-M_PI/6.0).cos()+0.24f*(2*mhp).cos()+0.32*(3*mhp+M_PI/30).cos()-0.20*(4*mhp-M_PI*63.0/180.0).cos();
    auto dTheta=M_PI/6.0*((M_PI*275.0/180.0-mhp)/(M_PI*25.0/180.0)).exp();
    auto Rc=2*(1-(pow(25,7))/(pow(25,7)+mCp.pow(7))).sqrt();
    //qDebug()<<"h";
    auto mLp_50_2=(mLp-50.0).square();
    auto SL=1.0+0.015*mLp_50_2/(20.0+mLp_50_2).sqrt();
    auto SC=1.0+0.045*mCp;
    auto SH=1.0+0.015*mCp*T;
    auto RT=-Rc*(2.0*dTheta).sin();
    //qDebug()<<"i";
    auto Diff=(dLp/SL).square()+(dCp/SC).square()+(dHp/SH).square()+RT*(dCp/SC)*(dHp/SH);
    //代码运行效果有问题
    cout<<Diff.transpose()<<endl;
    qDebug()<<"j";
    //qDebug()<<"size(Diff)=["<<Diff.rows()<<','<<Diff.cols()<<']';
    Diff.abs().minCoeff(&tempIndex);
        qDebug()<<"k";
    Result=Allowed->Map(tempIndex);
    qDebug()<<"l";
    return Result;
}

/*
float L_1=Data.rawPicRHLXc3[0](r,c);
float a_1=Data.rawPicRHLXc3[1](r,c);
float b_1=Data.rawPicRHLXc3[2](r,c);
auto L_2=allowedColors.col(0).array();
auto a_2=allowedColors.col(1).array();
auto b_2=allowedColors.col(2).array();
float C_ab_s_1=sqrt(a_1*a_1+b_1*b_1);
auto C_ab_s_2=(a_2.square()+b_2.square()).sqrt();

auto meanC_ab_s=(C_ab_s_1+C_ab_s_2)/2.0f;
auto G=0.5f*(1.0f-(meanC_ab_s.pow(7)/(meanC_ab_s.pow(7)+pow(25.0f,7))).sqrt());
auto a_p_1=(1.0f+G)*a_1;
auto a_p_2=(1.0f+G)*a_2;
auto C_ab_p_1=(a_p_1.square()+b_1*b_1).sqrt();
auto C_ab_p_2=(a_p_2.square()+b_2.square()).sqrt();
auto h_ab_p_1=(b_1/a_p_1).atan();
auto h_ab_p_2=(b_2/a_p_2).atan();

auto DL_p=L_1-L_2;
auto DC_ab=C_ab_p_1-C_ab_p_2;
auto Dh_ab=h_ab_p_1-h_ab_p_2;
auto DH_ab=2.0f*(C_ab_p_1*C_ab_p_2).sqrt()*((Dh_ab/2.0f).sin());

auto meanL=(L_1+L_2)/2.0f;
auto meanC_ab_p=(C_ab_p_1+C_ab_p_2)/2.0f;
auto meanh_ab_p=(h_ab_p_1+h_ab_p_2)/2.0f;
auto T=1.0f-0.17f*(meanh_ab_p-M_PI/6.0f).cos()+0.24f*(2.0f*meanh_ab_p).cos()+0.32f*(3.0f*meanh_ab_p+M_PI/30.0f).cos()-0.2f*(4.0f*meanh_ab_p-0.35f*M_PI).cos();
auto S_L=1.0f+0.015f*(meanL-50.0f).square()/(20.0f+(meanL-50.0f).square()).sqrt();
auto S_C=1.0f+0.045f*meanC_ab_p;
auto S_H=1.0f+0.015f*meanC_ab_p*T;

auto DTheta=M_PI/6.0f*(-((meanh_ab_p-275.0f/180.0f*M_PI)/25).square()).exp();
auto R_C=2.0f*(meanC_ab_s.pow(7)/(meanC_ab_s.pow(7)+pow(25,7))).sqrt();
auto R_T=-R_C*(2.0f*DTheta).sin();

auto Diff=(DL_p/(1.0f*S_L)).square()+(DC_ab/(1.0f*S_C)).square()+(DH_ab/(1.0f*S_H)).square()+R_T*(DC_ab/(1.0f*S_C))*(DH_ab/(1.0f*S_H));
*/
