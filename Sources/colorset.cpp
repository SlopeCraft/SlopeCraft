#ifndef colorset_cpp
#define colorset_cpp
#include "ColorSet.h"

ColorSet* TokiColor::Allowed=NULL;
ColorSet*TokiColor::Basic=NULL;
short TokiColor::DepthIndexEnd[4]={63,127,191,255};
unsigned char TokiColor::DepthCount[4]={64,64,64,64};
bool TokiColor::needFindSide=false;

ColorSet::ColorSet()
{
    GetMap(Map);
    _RGB.setZero(256,3);
    HSV.setZero(256,3);
    Lab.setZero(256,3);
    XYZ.setZero(256,3);
}

void ColorSet::ApplyAllowed(ColorSet*standard,bool *MIndex)
{
    TokiColor::DepthCount[0]=0;
    TokiColor::DepthCount[1]=0;
    TokiColor::DepthCount[2]=0;
    TokiColor::DepthCount[3]=0;
    short totalAllowColorCount=0;
        for(short r=0;r<256;r++)
        {
            totalAllowColorCount+=MIndex[r];//if r%64==0, that is air, air isn't used as color(it's transparent)
            TokiColor::DepthCount[(4*(r%64)+(r/64))%4]+=MIndex[r];
            //(4*(r%64)+(r/64))
        }
        TokiColor::DepthIndexEnd[0]=TokiColor::DepthCount[0]-1;
        TokiColor::DepthIndexEnd[1]=TokiColor::DepthIndexEnd[0]+TokiColor::DepthCount[1];
        TokiColor::DepthIndexEnd[2]=TokiColor::DepthIndexEnd[1]+TokiColor::DepthCount[2];
        TokiColor::DepthIndexEnd[3]=TokiColor::DepthIndexEnd[2]+TokiColor::DepthCount[3];

        if(totalAllowColorCount<=1)
        {
            qDebug("你只准许了一两种颜色，巧妇难为无米之炊！");
            _RGB.setZero(1,3);
            HSV.setZero(1,3);
            Lab.setZero(1,3);
            XYZ.setZero(1,3);
            Map.setZero(1);
            return;
        }
        qDebug()<<"共允许使用"<<totalAllowColorCount<<"种颜色";

        _RGB.setZero(totalAllowColorCount,3);
        HSV.setZero(totalAllowColorCount,3);
        Lab.setZero(totalAllowColorCount,3);
        XYZ.setZero(totalAllowColorCount,3);
        Map.setZero(totalAllowColorCount,1);
        short write=0;
        for(short readIndex=0;readIndex<=256;readIndex++)
        {

            if(write>=totalAllowColorCount)
                break;
            if(MIndex[readIndex])
            {
                _RGB.row(write)=standard->_RGB.row(readIndex);
                HSV.row(write)=standard->HSV.row(readIndex);
                Lab.row(write)=standard->Lab.row(readIndex);
                XYZ.row(write)=standard->XYZ.row(readIndex);
                Map(write)=standard->Map(readIndex);
                write++;
            }
        }
        //cout<<Map;
        return;
}

void GetMap(unsigned char *Map)
{
    for(short r=0;r<256;r++)Map[r]=4*(r%64)+r/64;
    return;
}

void GetMap(VectorXi &Map)
{
    Map.setZero(256);
    for(short r=0;r<256;r++)Map(r)=4*(r%64)+r/64;
    return;
}

#endif
