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
#include "TokiColor.h"
#include <cmath>

#define deg2rad(deg) ((deg)*M_PI/180.0)
#define rad2deg(rad) ((rad)*180.0/M_PI)
#define kL 1.0
#define kC 1.0
#define kH 1.0


inline double square(double x)
{
    return x*x;
}

float Lab00( float L1, float a1, float b1, float L2, float a2, float b2)
{
    double C1sab=std::sqrt(a1*a1+b1*b1);
    double C2sab=std::sqrt(a2*a2+b2*b2);
    double mCsab=(C1sab+C2sab)/2;
    double G=0.5*(1-std::sqrt(std::pow(mCsab,7)/(std::pow(mCsab,7)+std::pow(25,7))));
    double a1p=(1+G)*a1;
    double a2p=(1+G)*a2;
    double C1p=std::sqrt(a1p*a1p+b1*b1);
    double C2p=std::sqrt(a2p*a2p+b2*b2);
    double h1p,h2p;
    if(b1==0&&a1p==0)h1p=0;
    else h1p=std::atan2(b1,a1p);
    if(h1p<0)h1p+=2*M_PI;

    if(b2==0&&a2p==0)h2p=0;
    else h2p=std::atan2(b2,a2p);
    if(h2p<0)h2p+=2*M_PI;

    double dLp=L2-L1;
    double dCp=C2p-C1p;
    double dhp;
    if(C1p*C2p==0)
    {
        dhp=0;
    }
    else{
        if(std::abs(h2p-h1p)<=deg2rad(180.0))
        {
            dhp=h2p-h1p;
        }
        else if(h2p-h1p>deg2rad(180.0))
        {
            dhp=h2p-h1p-deg2rad(360.0);
        }
        else{
            dhp=h2p-h1p+deg2rad(360.0);
        }
    }

    double dHp=2*sqrt(C1p*C2p)*std::sin(dhp/2.0);



    double mLp=(L1+L2)/2;
    double mCp=(C1p+C2p)/2;
    double mhp;
    if(C1p*C2p==0)
    {
        mhp=(h1p+h2p);
    }
    else if(std::abs(h2p-h1p)<=deg2rad(180))
    {
        mhp=(h1p+h2p)/2;
    }
    else if(h1p+h2p<deg2rad(360))
    {
        mhp=(h1p+h2p+deg2rad(360))/2;
    }
    else
    {
        mhp=(h1p+h2p-deg2rad(360))/2;
    }

    double T=1-0.17*std::cos(mhp-deg2rad(30))+0.24*cos(2*mhp)+0.32*cos(3*mhp+deg2rad(6))-0.20*cos(4*mhp-deg2rad(63));

    double dTheta=deg2rad(30)*exp(-square((mhp-deg2rad(275))/deg2rad(25)));

    double RC=2*sqrt(pow(mCp,7)/(pow(25,7)+pow(mCp,7)));

    double SL=1+0.015*square(mLp-50)/sqrt(20+square(mLp-50));

    double SC=1+0.045*mCp;

    double SH=1+0.015*mCp*T;

    double RT=-RC*sin(2*dTheta);

    double Diffsquare=square(dLp/SL/kL)+square(dCp/SC/kC)+square(dHp/SH/kH)+RT*(dCp/SC/kC)*(dHp/SH/kH);

#ifdef ShowIntermedium
    printf("%s%lf\n","a1p=",a1p);
    printf("%s%lf\n","a2p=",a2p);

    printf("%s%lf\n","C1p=",C1p);
    printf("%s%lf\n","C2p=",C2p);

    printf("%s%lf\n","h1p=",rad2deg(h1p));
    printf("%s%lf\n","h2p=",rad2deg(h2p));

    printf("%s%lf\n","mhp=",rad2deg(mhp));
    printf("%s%lf\n","G=",G);
    printf("%s%lf\n","T=",T);
    printf("%s%lf\n","SL=",SL);
    printf("%s%lf\n","SC=",SC);
    printf("%s%lf\n","SH=",SH);
    printf("%s%lf\n","RT=",RT);

#endif

    return sqrt(Diffsquare);

}
