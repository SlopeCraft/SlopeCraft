#include "colorset.h"


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
    short totalAllowColorCount=0;
        for(short r=0;r<256;r++)
            totalAllowColorCount+=MIndex[r];//if r%64==0, that is air, air isn't used as color(it's transparent)
        if(totalAllowColorCount<=1)
        {
            qDebug("你只准许了一两种颜色，巧妇难为无米之炊！");
            _RGB.setZero(1,3);
            HSV.setZero(1,3);
            Lab.setZero(1,3);
            XYZ.setZero(1,3);
            Map.setZero(1,1);
            return;
        }
        qDebug()<<"共允许使用"<<totalAllowColorCount<<"种颜色";

        for(short Index=0;Index<256;Index++)
            if(MIndex[Index])
                qDebug()<<4*(Index%64)+(Index/64);



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
                Map(write,0)=standard->Map(readIndex,0);
                write++;
            }
        }
        //cout<<Map;
        return;
}
