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

#include "mainwindow.h"
#define reportRate 50
AdjT::AdjT(MainWindow*p)
{
    parent=p;
    connect(this,SIGNAL(addProgress(int)),parent,SLOT(AdjPro(int)));
}

void AdjT::run()
{
    if(parent->Data.adjStep<1)return;
    //wait();
    int step=reportRate*parent->Data.sizePic[0]*parent->Data.sizePic[1]/colorAdjuster.count()/2;
    int itered=1;
    //qDebug()<<"step="<<step;
    auto mid=colorAdjuster.begin();
    for(int count=0;count*2>=colorAdjuster.count();)
        if(colorAdjuster.contains(mid.key()))
        {
                mid++;
                count++;
        }
    for(;mid!=colorAdjuster.end();mid++)//后向遍历
    {
        if(colorAdjuster.contains(mid.key()))
        {
            if (mid.value().Result)break;//发现有处理过的颜色则终止
            mid.value().apply(mid.key());
            //parent->AdjPro(step);
            itered++;
            if(itered%reportRate==0)
            emit addProgress(step);
        }
    }
    //qDebug("成功将hash中所有颜色匹配为地图色");
    //qDebug()<<"调色工作量："<<ColorCount;

}
