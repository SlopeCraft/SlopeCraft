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

const ARGB airColor=qRgb(255,255,255);
const ARGB targetColor=qRgb(0,0,0);
const ARGB glassColor=qRgb(192,192,192);


double randD(){
    static std::default_random_engine generator(time(0));
    static std::uniform_real_distribution<double> rander(0,1);
    return rander(generator);
}

void defaultProgressRangeSet(int,int,int) {

}
void defaultProgressAdd(int) {

}
void defaultKeepAwake() {

}

edge::edge() {
    beg=TokiRC(0,0);
    end=TokiRC(0,0);
    lengthSquare=0;
}

edge::edge(TokiPos A,TokiPos B) {
    int r1=TokiRow(A),c1=TokiCol(A);
    int r2=TokiRow(B),c2=TokiCol(B);
    beg=A;
    end=B;
    int rowSpan=r1-r2;
    int colSpan=c1-c2;
    lengthSquare=(rowSpan*rowSpan+colSpan*colSpan);
}

edge::edge(ushort r1,ushort c1,ushort r2,ushort c2) {
    beg=TokiRC(r1,c1);
    end=TokiRC(r2,c2);
    int rowSpan=r1-r2;
    int colSpan=c1-c2;
    lengthSquare=(rowSpan*rowSpan+colSpan*colSpan);
}

bool edge::connectWith(TokiPos P) const {
    return (beg==P)||(end==P);
}

void edge::drawEdge(glassMap & map,bool drawHead) const {
    if(lengthSquare<=2)return;
    float length=sqrt(lengthSquare);
    Vector2f startPoint(TokiRow(beg),TokiCol(beg));
    Vector2f endPoint(TokiRow(end),TokiCol(end));
    Vector2f step=(endPoint-startPoint)/ceil(2.0*length);
    Vector2f cur;
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
    map(TokiRow(beg),TokiCol(beg))=(drawHead?PrimGlassBuilder::target:PrimGlassBuilder::air);
    map(TokiRow(end),TokiCol(end))=(drawHead?PrimGlassBuilder::target:PrimGlassBuilder::air);
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
    clock_t lastTime=std::clock();
    const int rowCount=ceil(double(_targetMap.rows())/130);
    const int colCount=ceil(double(_targetMap.cols())/130);

    std::vector<std::vector<PrimGlassBuilder*>> algos(rowCount);
    std::vector<std::vector<glassMap>> glassMaps(rowCount);
    std::vector<std::vector<walkableMap>>walkableMaps(rowCount);
    std::vector<std::vector<TokiMap>> targetMaps(rowCount);

    qDebug()<<"开始分区分块，共["<<rowCount<<','<<colCount<<"]个分区";
    for(int r=0;r<rowCount;r++) {
        algos[r].resize(colCount);
        glassMaps[r].resize(colCount);
        walkableMaps[r].resize(colCount);
        targetMaps[r].resize(colCount);
        for(int c=0;c<colCount;c++) {
            /*qDebug()<<"targetMaps["<<r<<"]["<<c<<"]=_targetMap.block("
            <<130*r<<','<<130*c<<','
            <<std::min((long long)(130),_targetMap.rows()-r*130)<<','
            <<std::min((long long)(130),_targetMap.cols()-c*130)<<");";*/
            targetMaps[r][c]=_targetMap.block(130*r,130*c,
                                              std::min((long long)(130),_targetMap.rows()-r*130),
                                              std::min((long long)(130),_targetMap.cols()-c*130));

            algos[r][c]=new PrimGlassBuilder;
#ifdef WITH_QT
            connect(algos[r][c],&PrimGlassBuilder::progressRangeSet,
                    this,&PrimGlassBuilder::progressRangeSet);
            connect(algos[r][c],&PrimGlassBuilder::progressAdd,
                    this,&PrimGlassBuilder::progressAdd);
            connect(algos[r][c],&PrimGlassBuilder::keepAwake,
                    this,&PrimGlassBuilder::keepAwake);
#endif
        }
    }
    qDebug("分区分块完毕，开始在每个分区内搭桥");
    for(int r=0;r<rowCount;r++) {
        for(int c=0;c<colCount;c++) {
            //qDebug()<<"开始处理第["<<r<<","<<c<<"]块分区";
            glassMaps[r][c]=
                    algos[r][c]->make4SingleMap(targetMaps[r][c],
                                        (walkable==nullptr)?nullptr:(&walkableMaps[r][c]));
        }
    }
    qDebug("每个分区内的搭桥完毕，开始在分区间搭桥");
    std::stack<edge> interRegionEdges;
    for(int r=0;r<rowCount;r++)
        for(int c=0;c<colCount;c++) {
            if(r+1<rowCount) {
                edge temp=connectSingleMaps(algos[r][c],TokiRC(130*r,130*c),
                                            algos[r+1][c],TokiRC(130*(r+1),130*c));
                if(temp.lengthSquare>2)
                    interRegionEdges.push(temp);
            }
            if(c+1<colCount) {
                edge temp=connectSingleMaps(algos[r][c],TokiRC(130*r,130*c),
                                            algos[r][c+1],TokiRC(130*r,130*(c+1)));
                if(temp.lengthSquare>2)
                    interRegionEdges.push(temp);
            }
        }
    qDebug()<<"分区间搭桥完毕，将搭建"<<interRegionEdges.size()<<"个分区间桥梁";

    qDebug()<<"开始拼合各分区的结果";

    glassMap result;
    result.setZero(_targetMap.rows(),_targetMap.cols());

    if(walkable!=nullptr)
        walkable->setZero(_targetMap.rows(),_targetMap.cols());

    for(int r=0;r<rowCount;r++)
        for(int c=0;c<colCount;c++) {
            /*qDebug()<<"result.block("<<130*r<<','<<130*c<<','
            <<targetMaps[r][c].rows()<<','<<targetMaps[r][c].cols()<<")=glassMaps["
            <<r<<"]["<<c<<"];";*/
            result.block(130*r,130*c,targetMaps[r][c].rows(),targetMaps[r][c].cols())
                    =glassMaps[r][c];
            if(walkable!=nullptr) {
                /*qDebug()<<"size(walkableMap)=["<<walkableMaps[r][c].rows()<<','<<walkableMaps[r][c].cols()<<"]";
                qDebug()<<"walkable->block("<<130*r<<','<<130*c<<','
                <<targetMaps[r][c].rows()<<','<<targetMaps[r][c].cols()<<")=walkableMaps["
                <<r<<"]["<<c<<"];";*/
                walkable->block(130*r,130*c,
                                targetMaps[r][c].rows(),targetMaps[r][c].cols())
                        =walkableMaps[r][c];
            }
        }
    qDebug("开始绘制分区间的桥");
    while(!interRegionEdges.empty()) {
        interRegionEdges.top().drawEdge(result);
        if(walkable!=nullptr)
            interRegionEdges.top().drawEdge(*walkable,true);
        interRegionEdges.pop();
    }
    qDebug("拼合分区完毕，开始delete各个分区的algo");
    for(int r=0;r<rowCount;r++)
        for(int c=0;c<colCount;c++)
            delete algos[r][c];

    qDebug()<<"用时"<<std::clock()-lastTime<<"毫秒";

return result;
}

glassMap PrimGlassBuilder::make4SingleMap(const TokiMap &_targetMap,
                                          walkableMap *walkable) {
    if(_targetMap.rows()>130||_targetMap.cols()>130) {
        qDebug("错误！make4SingleMap不应当收到超过130*130的图");
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
                    targetPoints.push_back(TokiRC(r,c));
            }
        }
    targetPoints.shrink_to_fit();
    edges.clear();
    tree.clear();

    emit progressRangeSet(0,0,0);

    if(targetPoints.size()>1) {
        addEdgesToGraph();
        runPrim();
    }
    emit progressRangeSet(0,100,100);

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

edge PrimGlassBuilder::connectSingleMaps(
                       const PrimGlassBuilder * map1,TokiPos offset1,
                       const PrimGlassBuilder * map2, TokiPos offset2) {

    if(map1->targetPoints.size()<=0||map2->targetPoints.size()<=0)
        return edge();

    ushort offsetR1=TokiRow(offset1),offsetC1=TokiCol(offset1);
    ushort offsetR2=TokiRow(offset2),offsetC2=TokiCol(offset2);

    ushort r1,r2,c1,c2;

    edge current;

    edge min(0,0,65535,65535);
    min.lengthSquare=0x7FFFFFFF;

    for(auto it=map1->targetPoints.cbegin();it!=map1->targetPoints.cend();it++)
        for(auto jt=map2->targetPoints.cbegin();jt!=map2->targetPoints.cend();jt++) {
            r1=offsetR1+TokiRow(*it);
            c1=offsetC1+TokiCol(*it);
            r2=offsetR2+TokiRow(*jt);
            c2=offsetC2+TokiCol(*jt);
            current=edge(r1,c1,r2,c2);
            if(current.lengthSquare<=2)return current;

            if(min.lengthSquare>current.lengthSquare)min=current;
        }
    return min;
}

void PrimGlassBuilder::addEdgesToGraph() {
    edges.clear();
    int taskCount=(targetPoints.size()*(targetPoints.size()-1))/2;
    emit progressRangeSet(0,taskCount,0);
    for(uint i=0;i<targetPoints.size();i++) {
        for(uint j=i+1;j<targetPoints.size();j++) {
            edges.push_back(edge(targetPoints[i],targetPoints[j]));
        }        
        emit keepAwake();
        emit progressAdd(targetPoints.size()-i);
    }
    //qDebug("插入了所有的边");
}

void PrimGlassBuilder::runPrim() {
    tree.clear();
    //TokiPos x,y;
    std::unordered_set<TokiPos> found,unsearched;
    found.clear();
    unsearched.clear();
    found.emplace(targetPoints[0]);

    emit progressRangeSet(0,targetPoints.size(),0);

    for(auto i=targetPoints.cbegin();;) {
        i++;
        if(i==targetPoints.cend())
            break;

        unsearched.emplace(*i);
    }

    while(!unsearched.empty()) {
        auto selectedEdge=edges.begin();

        //从列表中第一个元素开始搜索第一个可行边
        for(;;) {
            if(selectedEdge==edges.end()) {
                qDebug("错误！找不到可行边");
                break;
            }
            TokiPos z=selectedEdge->beg;
            TokiPos w=selectedEdge->end;
            bool fz=found.find(z)!=found.end();
            bool fw=found.find(w)!=found.end();

            if(fz&&fw) {
                selectedEdge=edges.erase(selectedEdge);//如果一条边的首尾都是已经被连接到的点，那么移除这条边
                continue;
            }
            bool uz=unsearched.find(z)!=unsearched.end();
            bool uw=unsearched.find(w)!=unsearched.end();
            if((fz&&uw)||(fw&&uz)) {
                //找到了第一条可行的边
                break;
            }
            selectedEdge++;
        }

        //从找到的第一条边开始，寻找长度最小的可行边
        for(auto it=selectedEdge;it!=edges.end();) {
            //if(selectedEdge->lengthSquare<=2)break;
            TokiPos x=it->beg,y=it->end;
            bool fx=found.find(x)!=found.end();
            bool fy=found.find(y)!=found.end();
            if(fx&&fy) {
                it=edges.erase(it);//如果一条边的首尾都是已经被连接到的点，那么移除这条边
                continue;
            }
            bool ux=unsearched.find(x)!=unsearched.end();
            bool uy=unsearched.find(y)!=unsearched.end();

            if((fx&&uy)||(fy&&ux)) {
                if(it->lengthSquare<selectedEdge->lengthSquare)
                    selectedEdge=it;
            }
            it++;
        }

        //将选中边装入树中，
        //并从集合unsearched中删除选中边的两个端点，
        //向集合found中加入选中边的两个端点
        {
            TokiPos x=selectedEdge->beg;
            TokiPos y=selectedEdge->end;
            found.emplace(x);
            found.emplace(y);
            unsearched.erase(x);
            unsearched.erase(y);
            tree.push_back(*selectedEdge);
        }
        emit progressRangeSet(0,targetPoints.size(),found.size());
        emit keepAwake();
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
                target1.push_back(TokiRC(r,c));
            if(map2(r,c)>=PrimGlassBuilder::target)
                target2.push_back(TokiRC(r,c));
        }
    std::list<edge> linkEdges;
    linkEdges.clear();
    edge min,temp;
    for(auto t1=target1.cbegin();t1!=target1.cend();t1++) {
        min.lengthSquare=0x7FFFFFFF;
        for(auto t2=target2.cbegin();t2!=target2.cend();t2++) {
            temp=edge(*t1,*t2);
            if(min.lengthSquare>temp.lengthSquare)
                min=temp;
            if(min.lengthSquare<=2)
                break;
        }
        linkEdges.push_back(min);
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

TokiMap ySlice2TokiMap(const Tensor<uchar,3>& raw) {
    TokiMap result(raw.dimension(0),raw.dimension(2));
    result.setZero();
    for(int i=0;i<raw.size();i++)
        if(raw(i)>1)
            result(i)=PrimGlassBuilder::target;
    return result;
}
