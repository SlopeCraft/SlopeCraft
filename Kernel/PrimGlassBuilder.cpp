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

#include "PrimGlassBuilder.h"

const ARGB airColor=ARGB32(255,255,255);
const ARGB targetColor=ARGB32(0,0,0);
const ARGB glassColor=ARGB32(192,192,192);

const std::vector<TokiPos> * edge::vertexes=nullptr;


void defaultProgressRangeSet(int,int,int) {

}
void defaultProgressAdd(int) {

}
void defaultKeepAwake() {

}

edge::edge() {
    //beg=TokiRC(0,0);
    //end=TokiRC(0,0);
    begIdx=0;
    endIdx=0;
    lengthSquare=0;
}

edge::edge(uint _begIdx,uint _endIdx) {
    begIdx=_begIdx;
    endIdx=_endIdx;
    int r1=TokiRow(beg()),c1=TokiCol(beg());
    int r2=TokiRow(end()),c2=TokiCol(end());

    int rowSpan=r1-r2;
    int colSpan=c1-c2;
    lengthSquare=(rowSpan*rowSpan+colSpan*colSpan);
}

TokiPos edge::beg() const {
    return vertexes->at(begIdx);
}

TokiPos edge::end() const {
    return vertexes->at(endIdx);
}

pairedEdge::pairedEdge() {
    first=TokiRC(0,0);
    second=TokiRC(0,0);
    lengthSquare=0;
}
pairedEdge::pairedEdge(TokiPos A,TokiPos B) {
    int r1=TokiRow(A),c1=TokiCol(A);
    int r2=TokiRow(B),c2=TokiCol(B);
    first=A;
    second=B;
    int rowSpan=r1-r2;
    int colSpan=c1-c2;
    lengthSquare=(rowSpan*rowSpan+colSpan*colSpan);
}

pairedEdge::pairedEdge(ushort r1,ushort c1,ushort r2,ushort c2) {
    first=TokiRC(r1,c1);
    second=TokiRC(r2,c2);
    int rowSpan=r1-r2;
    int colSpan=c1-c2;
    lengthSquare=(rowSpan*rowSpan+colSpan*colSpan);
}

pairedEdge::pairedEdge(const edge & src) {
    first=src.beg();
    second=src.end();
    lengthSquare=src.lengthSquare;
}
/*
bool edge::connectWith(TokiPos P) const {
    return pairedEdge(*this).connectWith(P);
}

void edge::drawEdge(glassMap & map, bool drawHead) const {
    pairedEdge(*this).drawEdge(map,drawHead);
    return;
}
*/
bool pairedEdge::connectWith(TokiPos P) const {
    return (first==P)||(second==P);
}

void pairedEdge::drawEdge(glassMap & map,bool drawHead) const {
    if(lengthSquare<=2)return;
    float length=sqrt(lengthSquare);
    Eigen::Vector2f startPoint(TokiRow(first),TokiCol(first));
    Eigen::Vector2f endPoint(TokiRow(second),TokiCol(second));
    Eigen::Vector2f step=(endPoint-startPoint)/ceil(2.0*length);
    Eigen::Vector2f cur;
    int stepCount=ceil(2.0*length);
    int r,c;
    for(int i=1;i<stepCount;i++) {
        cur=i*step+startPoint;
        r=floor(cur(0));
        c=floor(cur(1));
        if(r>=0&&r<map.rows()&&c>=0&&c<map.cols()) {
            map(r,c)=PrimGlassBuilder::glass;
            continue;
        }
        r=ceil(cur(0));
        c=ceil(cur(1));
        if(r>=0&&r<map.rows()&&c>=0&&c<map.cols())
            map(r,c)=PrimGlassBuilder::glass;
    }
    map(TokiRow(first),TokiCol(first))=
            (drawHead?PrimGlassBuilder::target:PrimGlassBuilder::air);
    map(TokiRow(second),TokiCol(second))=
            (drawHead?PrimGlassBuilder::target:PrimGlassBuilder::air);
}

#ifdef WITH_QT
PrimGlassBuilder::PrimGlassBuilder(QObject *parent) : QObject(parent)
#else
PrimGlassBuilder::PrimGlassBuilder()
#endif
{
#ifndef WITH_QT
    progressRangeSet=defaultProgressRangeSet;
    progressAdd=defaultProgressAdd;
    keepAwake=defaultKeepAwake;
#endif
}
glassMap PrimGlassBuilder::makeBridge(const TokiMap & _targetMap,
                                      walkableMap* walkable) {
    //clock_t lastTime=std::clock();
    const int rowCount=ceil(double(_targetMap.rows())/unitL);
    const int colCount=ceil(double(_targetMap.cols())/unitL);

    std::vector<std::vector<PrimGlassBuilder*>> algos(rowCount);
    std::vector<std::vector<glassMap>> glassMaps(rowCount);
    std::vector<std::vector<walkableMap>>walkableMaps(rowCount);
    std::vector<std::vector<TokiMap>> targetMaps(rowCount);

    //std::cerr<<"开始分区分块，共["<<rowCount<<','<<colCount<<"]个分区";
    for(int r=0;r<rowCount;r++) {
        algos[r].resize(colCount);
        glassMaps[r].resize(colCount);
        walkableMaps[r].resize(colCount);
        targetMaps[r].resize(colCount);
        for(int c=0;c<colCount;c++) {

            targetMaps[r][c]=_targetMap.block(unitL*r,unitL*c,
                                              std::min((long long)(unitL),_targetMap.rows()-r*unitL),
                                              std::min((long long)(unitL),_targetMap.cols()-c*unitL));

			algos[r][c] = pgb.animate();
//#ifdef WITH_QT
//            connect(algos[r][c],&PrimGlassBuilder::progressRangeSet,
//                    this,&PrimGlassBuilder::progressRangeSet);
//            connect(algos[r][c],&PrimGlassBuilder::progressAdd,
//                    this,&PrimGlassBuilder::progressAdd);
//            connect(algos[r][c],&PrimGlassBuilder::keepAwake,
//                    this,&PrimGlassBuilder::keepAwake);
//#endif
        }
    }
    //qDebug("分区分块完毕，开始在每个分区内搭桥");
    for(int r=0;r<rowCount;r++) {
        for(int c=0;c<colCount;c++) {
            //qDebug()<<"开始处理第["<<r<<","<<c<<"]块分区";
            glassMaps[r][c]=
                    algos[r][c]->make4SingleMap(targetMaps[r][c],
                                        (walkable==nullptr)?nullptr:(&walkableMaps[r][c]));
        }
         emit progressRangeSet(0, rowCount, r);
    }
    //qDebug("每个分区内的搭桥完毕，开始在分区间搭桥");
    std::stack<pairedEdge> interRegionEdges;
    for(int r=0;r<rowCount;r++)
        for(int c=0;c<colCount;c++) {
            if(r+1<rowCount) {
                pairedEdge temp=connectSingleMaps(algos[r][c],TokiRC(unitL*r,unitL*c),
                                            algos[r+1][c],TokiRC(unitL*(r+1),unitL*c));
                if(temp.lengthSquare>2)
                    interRegionEdges.emplace(temp);
            }
            if(c+1<colCount) {
                pairedEdge temp=connectSingleMaps(algos[r][c],TokiRC(unitL*r,unitL*c),
                                            algos[r][c+1],TokiRC(unitL*r,unitL*(c+1)));
                if(temp.lengthSquare>2)
                    interRegionEdges.emplace(temp);
            }
        }
    //qDebug()<<"分区间搭桥完毕，将搭建"<<interRegionEdges.size()<<"个分区间桥梁";

    //qDebug()<<"开始拼合各分区的结果";

    glassMap result;
    result.setZero(_targetMap.rows(),_targetMap.cols());

    if(walkable!=nullptr)
        walkable->setZero(_targetMap.rows(),_targetMap.cols());

    for(int r=0;r<rowCount;r++)
        for(int c=0;c<colCount;c++) {
            /*qDebug()<<"result.block("<<unitL*r<<','<<unitL*c<<','
            <<targetMaps[r][c].rows()<<','<<targetMaps[r][c].cols()<<")=glassMaps["
            <<r<<"]["<<c<<"];";*/
            result.block(unitL*r,unitL*c,targetMaps[r][c].rows(),targetMaps[r][c].cols())
                    =glassMaps[r][c];
            if(walkable!=nullptr) {
                /*qDebug()<<"size(walkableMap)=["<<walkableMaps[r][c].rows()<<','<<walkableMaps[r][c].cols()<<"]";
                qDebug()<<"walkable->block("<<unitL*r<<','<<unitL*c<<','
                <<targetMaps[r][c].rows()<<','<<targetMaps[r][c].cols()<<")=walkableMaps["
                <<r<<"]["<<c<<"];";*/
                walkable->block(unitL*r,unitL*c,
                                targetMaps[r][c].rows(),targetMaps[r][c].cols())
                        =walkableMaps[r][c];
            }
        }
    //qDebug("开始绘制分区间的桥");
    
    while(!interRegionEdges.empty()) {
        interRegionEdges.top().drawEdge(result);
        if(walkable!=nullptr)
            interRegionEdges.top().drawEdge(*walkable,true);
        interRegionEdges.pop();
    }
    //qDebug("拼合分区完毕，开始delete各个分区的algo");
    for(int r=0;r<rowCount;r++)
        for(int c=0;c<colCount;c++)
			pgb.recycle(algos[r][c]);

    emit progressRangeSet(0, 100, 100);
    //qDebug()<<"用时"<<std::clock()-lastTime<<"毫秒";

return result;
}

glassMap PrimGlassBuilder::make4SingleMap(const TokiMap &_targetMap,
                                          walkableMap *walkable) {
    if(_targetMap.rows()>unitL||_targetMap.cols()>unitL) {
        //qDebug("错误！make4SingleMap不应当收到超过unitL*unitL的图");
        return glassMap(0,0);
    }
    targetPoints.clear();
    for(short r=0;r<_targetMap.rows();r++)
        for(short c=0;c<_targetMap.cols();c++) {
            if(_targetMap(r,c)) {
                if(r>1&&c>1&&r+1<_targetMap.rows()&&c+1<_targetMap.cols()
                        &&_targetMap(r+1,c)&&_targetMap(r-1,c)
                        &&_targetMap(r,c+1)&&_targetMap(r,c-1))
                    continue;
                else
                    targetPoints.emplace_back(TokiRC(r,c));
            }
        }
    targetPoints.shrink_to_fit();

    //std::cerr<<"targetPoints.size="<<targetPoints.size()<<std::endl;

    edges.clear();
    tree.clear();


    if(targetPoints.size()>1) {
        addEdgesToGraph();
        //std::cerr<<"edges.size="<<edges.size()<<std::endl;
        runPrim();
        //std::cerr<<"tree.size="<<tree.size()<<std::endl;
    }


    glassMap result(_targetMap.rows(),_targetMap.cols());
    result.setZero();

    for(auto it=tree.cbegin();it!=tree.cend();it++)
        it->drawEdge(result);

    for(auto it=targetPoints.cbegin();it!=targetPoints.cend();it++)
        result(TokiRow(*it),TokiCol(*it))=0;

    if(walkable!=nullptr)
        *walkable=result;

    for(auto it=targetPoints.cbegin();it!=targetPoints.cend();it++) {
        result(TokiRow(*it),TokiCol(*it))=0;
        if(walkable!=nullptr)
            walkable->operator()(TokiRow(*it),TokiCol(*it))=blockType::target;
    }
    return result;
}

pairedEdge PrimGlassBuilder::connectSingleMaps(
                       const PrimGlassBuilder * map1,TokiPos offset1,
                       const PrimGlassBuilder * map2, TokiPos offset2) {

    if(map1->targetPoints.size()<=0||map2->targetPoints.size()<=0)
        return edge();

    ushort offsetR1=TokiRow(offset1),offsetC1=TokiCol(offset1);
    ushort offsetR2=TokiRow(offset2),offsetC2=TokiCol(offset2);

    ushort r1,r2,c1,c2;

    pairedEdge current;

    pairedEdge min;
    min.lengthSquare=0x7FFFFFFF;

    for(auto it=map1->targetPoints.cbegin();it!=map1->targetPoints.cend();it++)
        for(auto jt=map2->targetPoints.cbegin();jt!=map2->targetPoints.cend();jt++) {
            r1=offsetR1+TokiRow(*it);
            c1=offsetC1+TokiCol(*it);
            r2=offsetR2+TokiRow(*jt);
            c2=offsetC2+TokiCol(*jt);
            current=pairedEdge(r1,c1,r2,c2);
            if(current.lengthSquare<=2)return current;

            if(min.lengthSquare>current.lengthSquare)min=current;
        }
    return min;
}

void PrimGlassBuilder::addEdgesToGraph() {
    edges.clear();
	edge::vertexes = std::addressof(targetPoints);
    int taskCount=(targetPoints.size()*(targetPoints.size()-1))/2;
    //emit progressRangeSet(0,taskCount,0);
    for(uint i=0;i<targetPoints.size();i++) {
        for(uint j=i+1;j<targetPoints.size();j++) {
			edges.emplace_back(edge(i, j));
        }        
        //emit keepAwake();
        //emit progressAdd(targetPoints.size()-i);
    }
    //qDebug("插入了所有的边");
}
void PrimGlassBuilder::runPrim() {
    tree.clear();
    tree.reserve(targetPoints.size()-1);

    std::vector<bool> isFound(targetPoints.size(),false);
    isFound[0]=true;

    uint foundCount=1;

    //emit progressRangeSet(0,targetPoints.size(),0);

    std::stack<std::list<edge>::iterator> eraseTask;

    while(!eraseTask.empty())
        eraseTask.pop();

    while(foundCount<targetPoints.size()) {

        while(!eraseTask.empty()) {
            edges.erase(eraseTask.top());
            eraseTask.pop();
        }

        auto selectedEdge=edges.begin();

        //从列表中第一个元素开始搜索第一个可行边
       for(;;) {
            if(selectedEdge==edges.end()) {
                std::cerr<<"Error: failed to find valid edge!\n";
                break;
            }
            //TokiPos z=selectedEdge->beg();
            //TokiPos w=selectedEdge->end();
            bool fz = isFound[(selectedEdge)->begIdx];
			bool fw = isFound[(selectedEdge)->endIdx];

            if(fz&&fw) {
                eraseTask.emplace(selectedEdge);
                selectedEdge++;
                //如果一条边的首尾都是已经被连接到的点，那么移除这条边
                continue;
            }
            bool uz=!fz;
            bool uw=!fw;
            if((fz&&uw)||(fw&&uz)) {
                //找到了第一条可行的边
                break;
            }
            selectedEdge++;
        }

        //从找到的第一条边开始，寻找长度最小的可行边
        for(auto it=selectedEdge;it!=edges.end();) {
            //if(selectedEdge->lengthSquare<=2)break;
            //TokiPos x=it->beg(),y=it->end();
            bool fx = isFound[(it)->begIdx];
			bool fy = isFound[(it)->endIdx];
            if(fx&&fy) {
                eraseTask.emplace(it);
                it++;//如果一条边的首尾都是已经被连接到的点，那么移除这条边
                continue;
            }
            bool ux=!fx;
            bool uy=!fy;

            if((fx&&uy)||(fy&&ux)) {
                if ((it)->lengthSquare < (selectedEdge)->lengthSquare)
                    selectedEdge=it;
            }
            it++;
        }

        //将选中边装入树中，
        //并从集合unsearched中删除选中边的两个端点，
        //向集合found中加入选中边的两个端点
        {
            //TokiPos x=selectedEdge->beg();
            //TokiPos y=selectedEdge->end();
            isFound[(selectedEdge)->begIdx]=true;
            isFound[(selectedEdge)->endIdx]=true;
            foundCount++;
            //found.emplace(x);
            //found.emplace(y);
            //unsearched.erase(x);
            //unsearched.erase(y);
            tree.emplace_back(*selectedEdge);
        }
        //if(foundCount%reportRate==0) {
        //    //emit progressRangeSet(0,targetPoints.size(),foundCount);
        //    emit keepAwake();
        //}
    }
    //qDebug("prim算法完毕");
}

EImage TokiMap2EImage(const TokiMap& tm) {
    EImage result(tm.rows(),tm.cols());
    result.setConstant(airColor);
    for(ushort r=0;r<tm.rows();r++)
        for(ushort c=0;c<tm.cols();c++) {
            if(tm(r,c)==1)
                result(r,c)=glassColor;
            if(tm(r,c)>1)
                result(r,c)=targetColor;
        }
    return result;
}

glassMap connectBetweenLayers(const TokiMap & map1,const TokiMap & map2,
                              walkableMap* walkable) {
    std::list<TokiPos> target1,target2;
    target1.clear();
    target2.clear();
    for(int r=0;r<map1.rows();r++)
        for(int c=0;c<map1.cols();c++) {
            if(map1(r,c)>=PrimGlassBuilder::target)
                target1.emplace_back(TokiRC(r,c));
            if(map2(r,c)>=PrimGlassBuilder::target)
                target2.emplace_back(TokiRC(r,c));
        }
    std::list<pairedEdge> linkEdges;
    linkEdges.clear();
    pairedEdge min,temp;
    for(auto t1=target1.cbegin();t1!=target1.cend();t1++) {
        min.lengthSquare=0x7FFFFFFF;
        for(auto t2=target2.cbegin();t2!=target2.cend();t2++) {
            temp=pairedEdge(*t1,*t2);
            if(min.lengthSquare>temp.lengthSquare)
                min=temp;
            if(min.lengthSquare<=2)
                break;
        }
        linkEdges.emplace_back(min);
    }

    glassMap result;
    result.setZero(map1.rows(),map1.cols());

    for(auto it=linkEdges.cbegin();it!=linkEdges.cend();it++)
        it->drawEdge(result);

    if(walkable!=nullptr)
        *walkable=result;

    for(auto t=target1.cbegin();t!=target1.cend();t++) {
        result(TokiRow(*t),TokiCol(*t))=PrimGlassBuilder::air;
        if(walkable!=nullptr)
            walkable->operator()(TokiRow(*t),TokiCol(*t))=PrimGlassBuilder::target;
    }
    for(auto t=target2.cbegin();t!=target2.cend();t++) {
        result(TokiRow(*t),TokiCol(*t))=PrimGlassBuilder::air;
        if(walkable!=nullptr)
            walkable->operator()(TokiRow(*t),TokiCol(*t))=PrimGlassBuilder::target;
    }
    return result;
}

TokiMap ySlice2TokiMap(const Eigen::Tensor<uchar,3>& raw) {
    TokiMap result(raw.dimension(0),raw.dimension(2));
    result.setZero();
    for(int i=0;i<raw.size();i++)
        if(raw(i)>1)
            result(i)=PrimGlassBuilder::target;
    return result;
}
