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


#include "OptiChain.h"

#define NInf -100000
#define MapSize (Base.rows())
ArrayXXi OptiChain::Base=MatrixXi::Zero(0,0);
Array3i OptiChain::Both(-1,2,-1);
Array3i OptiChain::Left(-1,1,0);
Array3i OptiChain::Right(0,1,-1);
//QLabel* OptiChain::SinkIDP=nullptr;

#ifdef showImg
QLabel* OptiChain::SinkAll=nullptr;
bool OptiChain::AllowSinkHang=false;
#else
    #define AllowSinkHang true
#endif
#ifndef removeQt
QRgb isTColor=qRgb(0,0,0);
QRgb isFColor=qRgb(255,255,255);
QRgb WaterColor=qRgb(0,64,255);
QRgb greyColor=qRgb(192,192,192);
#endif
Region::Region(short _Beg,short _End,RegionType _Type)
{
    Beg=_Beg;End=_End;type=_Type;
}

inline bool Region::isHang() const
{
    return(type==Hang);
}

inline bool Region::isIDP() const
{
    return(type==idp);
}

inline bool Region::isValid() const
{
    return(type!=Invalid)&&(size()>=1);
}

inline int Region::size() const
{
    return (End-Beg+1);
}

inline short Region::indexLocal2Global(short indexLocal) const
{
    return indexLocal+Beg;
}

inline short Region::indexGlobal2Local(short indexGlobal) const
{
    return indexGlobal-Beg;
}

string Region::toString() const
{
    if(!isValid())
        return '{'+to_string(Beg)+','+to_string(End)+'}';//无效区间用大括号
    if(isHang())
        return '['+to_string(Beg)+','+to_string(End)+']';//悬空区间用中括号

    return '('+to_string(Beg)+','+to_string(End)+')';//可沉降区间用括号
}

OptiChain::OptiChain(int size)
{
    if(size<0)
        return;

}

OptiChain::OptiChain(ArrayXi High,ArrayXi Low,int _col)
{
    Col=_col;
    HighLine=High;
    LowLine=Low;
    SubChain.clear();
}

OptiChain::~OptiChain()
{
    return;
}

int OptiChain::validHeight(int index)
{
    if(index<0||index>=MapSize)return NInf;
    if(isAir(index))return NInf;
    return HighLine(index);
}

inline bool OptiChain::isAir(int index)
{
    if(index<0||index>=MapSize)return true;
    return (Base(index,Col)==0);
}

inline bool OptiChain::isWater(int index)
{
    if(index<0||index>=MapSize)return false;
    return (Base(index,Col)==12);
}

inline bool OptiChain::isSolidBlock(int index)
{
    if(index<0||index>=MapSize)return false;
    return(Base(index,Col)!=0&&Base(index,Col)!=12);
}

void OptiChain::dispSubChain()
{
    string out="";
    for(auto it=SubChain.cbegin();it!=SubChain.cend();it++)
    {
        out+=it->toString();
    }
#ifdef removeQt
    cout<<out<<endl;
#else
    qDebug()<<QString::fromStdString(out);
#endif
    //cout<<out<<endl;
}

/*
void HeightLine::segment2Brackets(list<Pair>&List,short sBeg,short sEnd)
{
    if(sEnd<sBeg||sBeg<0)return;
    List.clear();

    if(sBeg==sEnd)
    {
        List.push_back(Pair('(',sBeg));
        List.push_back(Pair(')',sEnd));
        return;
    }

    queue<Region>Pure;
    queue<Region> disPure;//极大值区间
    Region Temp;
    bool isReady=false;
    VectorXi VHL=ValidHighLine();
    VectorXi ScanBoth=VHL,ScanLeft=VHL,ScanRight=VHL;
    ScanBoth.setZero();ScanLeft.setZero();ScanRight.setZero();
    for(int i=sBeg+1;i<sEnd-1;i++)//分别用三个算子处理
    {
        ScanBoth(i)=(VHL.segment(i-1,3).array()*Both.array()).sum();
        ScanLeft(i)=(VHL.segment(i-1,3).array()*Left.array()).sum();
        ScanRight(i)=(VHL.segment(i-1,3).array()*Right.array()).sum();
    }
ScanBoth=(ScanBoth.array()>=0).select(ScanBoth,0);
ScanLeft=(ScanLeft.array()>=0).select(ScanLeft,0);
ScanRight=(ScanRight.array()>=0).select(ScanRight,0);
    isReady=false;
    for(int i=sBeg+1;i<sEnd-1;i++)
    {
        if(!isReady&&ScanBoth(i)&&ScanLeft(i))
        {
            isReady=true;
            Temp.Begin=i;
        }
        if(isReady&&ScanBoth(i)&&ScanRight(i))
        {
            Temp.End=i;
            disPure.push(Temp);
            Temp.Begin=-1;
            Temp.End=-1;
            isReady=false;
        }
    }
    Temp.Begin=sBeg;
    Temp.End=sEnd;
    while(!disPure.empty())
    {
        Temp.End=disPure.front().Begin-1;
        Pure.push(Temp);
        Temp.Begin=disPure.front().End+1;
        disPure.pop();
        if(Temp.Begin>=sEnd)Temp.Begin=sEnd;
        Temp.End=sEnd;
    }
    Pure.push(Temp);

    while(!Pure.empty())
    {
        DealRegion(Pure.front(),List);
#ifndef NoOutPut
        cout<<'['<<Pure.front().Begin<<','<<Pure.front().End<<']'<<"->";
#endif
        Pure.pop();
    }
    List.push_front(Pair('(',sBeg));
    List.push_back(Pair(')',sEnd));
#ifndef NoOutPut
    disp(List);
#endif
}

inline void HeightLine::DealRegion(Region PR, list<Pair> &List)
{
    if(PR.Begin<0||PR.End<PR.Begin)return;
    List.push_back(Pair('(',PR.Begin));
    List.push_back(Pair(')',PR.End));
}
*/

void OptiChain::divideAndCompress()
{
    divideToChain();
    while(!Chain.empty())
    {
        divideToSubChain();

        for(auto it=SubChain.begin();it!=SubChain.end();it++)
            if(it->isIDP())Sink(*it);

        for(auto it=SubChain.begin();it!=SubChain.end();it++)
            if(AllowSinkHang&&it->isHang())Sink(*it);
    }
#ifdef showImg

    int scaledH=SinkAll->height()-2;
    int scaledW=SinkAll->width()-2;
    SinkAll->setPixmap(QPixmap::fromImage(toQImage(3)).scaled(scaledW,scaledH));
#endif
}

void OptiChain::divideToChain()
{
    while(!Chain.empty())Chain.pop();
    Region Temp(0,MapSize-1,idp);
    for(int i=1;i<MapSize;i++)
    {
        if(isAir(i))
        {
            Temp.End=i-1;
            Chain.push(Temp);
            Temp.Beg=i;
        }
        if(isWater(i))
        {
            Temp.End=i-1;
            Chain.push(Temp);
            Temp.Beg=i;
        }
        if(isSolidBlock(i)&&isAir(i))
        {
            Temp.End=i-1;
            Chain.push(Temp);
            Temp.Beg=i;
        }
    }
    Temp.End=MapSize-1;
    Chain.push(Temp);
#ifdef removeQt
    cout<<"Divided coloum "<<Col<<" into "<<Chain.size()<<"isolated region(s)"<<endl;
#else
    qDebug()<<"将第"<<Col<<"列切分为"<<Chain.size()<<"个孤立区间";
#endif
}

void OptiChain::divideToSubChain()
{
    if(!Chain.front().isValid())
    {
        Chain.pop();
        return;
    }
    SubChain.clear();

    divideToSubChain(Chain.front());
    Chain.pop();
}

void OptiChain::divideToSubChain(const Region &Cur)
{
#ifdef removeQt
    cout<<"ready to analyse"<<Cur.toString()<<endl;
#else
    qDebug()<<"开始分析区间"+QString::fromStdString(Cur.toString());
#endif
    if(Cur.size()<=3)
    {
        SubChain.push_back(Region(Cur.Beg,Cur.End,idp));
#ifdef removeQt
        cout<<"Region"<<Cur.toString()<<" in Chain is too thin, sink directly."<<endl;
#else
        qDebug()<<"Chain中的区间"+QString::fromStdString(Cur.toString())+"过小，直接简单沉降";
#endif
        return;
    }

    ArrayXi HL;
    HL.setZero(Cur.size()+1);
    HL.segment(0,Cur.size())=HighLine.segment(Cur.Beg,Cur.size());
    HL(Cur.size())=NInf;

    ArrayXi ScanBoth,ScanLeft,ScanRight;
    ScanBoth.setZero(Cur.size());
    ScanLeft.setZero(Cur.size());
    ScanRight.setZero(Cur.size());
    //qDebug("开始用三个一维算子扫描HighLine和LowLine");
    for(int i=1;i<Cur.size();i++)//用三个算子扫描一个大孤立区间
    {
        ScanBoth(i)=(HL.segment(i-1,3)*Both).sum()>0;
        ScanLeft(i)=(HL.segment(i-1,3)*Left).sum()>0;
        ScanRight(i)=(HL.segment(i-1,3)*Right).sum()>0;
    }
    ScanLeft*=ScanBoth;
    ScanRight*=ScanBoth;

#ifdef removeQt
    cout<<"scanning finished"<<endl;
#else
    qDebug("扫描完成");
#endif

    bool isReady=false;

    //表示已经检测出极大值区间的入口，找到出口就装入一个极大值区间
    Region Temp(-1,-1,Hang);//均写入绝对index而非相对index
    for(int i=0;i<Cur.size();i++)
    {
        if(!isReady&&ScanLeft(i))
        {
            isReady=true;
            Temp.Beg=Cur.indexLocal2Global(i);
            Temp.End=-1;
        }
        if(isReady&&ScanRight(i))
        {
            isReady=false;
            Temp.End=Cur.indexLocal2Global(i);
            SubChain.push_back(Temp);
            Temp.Beg=-1;Temp.End=-1;
        }
    }
    //qDebug("已将极大值区间串联成链，即将开始填充孤立区间。此时的SubChain为：");
    //dispSubChain();
    auto prev=SubChain.begin();
    for(auto it=SubChain.begin();it!=SubChain.end();prev=it++)
    {
        if(it==SubChain.begin())
        {
            Temp.Beg=Cur.indexLocal2Global(0);
            Temp.End=(it->Beg-1);
            Temp.type=idp;
            if(Temp.isValid())
                SubChain.insert(it,Temp);
        }
        else
        {
            Temp.Beg=(prev->End+1);
            Temp.End=(it->Beg-1);
            Temp.type=idp;
            if(Temp.isValid())
                SubChain.insert(it,Temp);
        }
    }

    if(SubChain.size()<=0)
    {
        SubChain.push_back(Cur);
    }
    else    if(SubChain.back().End<Cur.End)
        SubChain.push_back(Region(SubChain.back().End+1,Cur.End,idp));

#ifdef removeQt
    cout<<"SubChain constructed"<<endl;
#else
    qDebug("SubChain构建完成");
#endif
}

void OptiChain::Sink(const Region &Reg)
{
    if(!Reg.isValid())
    {
#ifdef removeQt
        cout<<"Invalid region: "<<Reg.toString();
#else
        qDebug()<<"无效区间："+QString::fromStdString(Reg.toString());
#endif
        return;
    }
    if(Reg.isIDP())
    {
        HighLine.segment(Reg.Beg,Reg.size())-=LowLine.segment(Reg.Beg,Reg.size()).minCoeff();
        LowLine.segment(Reg.Beg,Reg.size())-=LowLine.segment(Reg.Beg,Reg.size()).minCoeff();
        return;
    }
    if(AllowSinkHang&&Reg.isHang())
    {
        int BegGap;

        BegGap=validHeight(Reg.Beg)-validHeight(Reg.Beg-1);

        int EndGap;
        if(isSolidBlock(Reg.End+1))
            EndGap=validHeight(Reg.End)-validHeight(Reg.End+1);
        else
            EndGap=validHeight(Reg.End)-NInf;
        int offset=min(max(min(BegGap,EndGap)-1,0),LowLine.segment(Reg.Beg,Reg.size()).minCoeff());
        HighLine.segment(Reg.Beg,Reg.size())-=offset;
        LowLine.segment(Reg.Beg,Reg.size())-=offset;
    }
}
#ifndef removeQt
QImage OptiChain::toQImage(int pixelSize)
{
    int maxHeight=HighLine.maxCoeff()+1;
    HighLine-=LowLine.minCoeff();
    LowLine-=LowLine.minCoeff();
    ArrayXXi QRgbMat(maxHeight,MapSize);

    QRgbMat.setConstant(isFColor);

    for(int i=0;i<MapSize;i++)
    {
        if(isAir(i))
            continue;
        if(isWater(i))
        {
            QRgbMat.block(LowLine(i),i,HighLine(i)-LowLine(i)+1,1)=WaterColor;
            continue;
        }
        QRgbMat(HighLine(i),i)=isTColor;
    }
    QRgbMat.colwise().reverseInPlace();

    return Mat2Image(QRgbMat,pixelSize);
}

QImage Mat2Image(const ArrayXXi& mat,int pixelSize)
{
    QImage img(mat.cols()*pixelSize,mat.rows()*pixelSize,QImage::Format_ARGB32);

    QRgb* SL=nullptr;

    for(int r=0;r<mat.rows()*pixelSize;r++)
    {
        SL=(QRgb*)img.scanLine(r);
        for(int c=0;c<mat.cols()*pixelSize;c++)
        {
            SL[c]=mat(r/pixelSize,c/pixelSize);
        }
    }

    return img;
}
#endif
