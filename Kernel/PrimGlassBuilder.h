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

#ifndef PRIMGLASSBUILDER_H
#define PRIMGLASSBUILDER_H


#include <QObject>

//using namespace Eigen;

#include <iostream>
#include <random>
#include <cmath>
#include <ctime>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unsupported/Eigen/CXX11/Tensor>
#include "defines.h"
#include "TokiColor.h"
#include "WaterItem.h"

#ifdef WITH_QT
    #include <QObject>
#endif


//using namespace Eigen;

extern const ARGB airColor;
extern const ARGB targetColor;
extern const ARGB glassColor;

typedef Eigen::Array<uchar,Eigen::Dynamic,Eigen::Dynamic> TokiMap;
typedef TokiMap glassMap;
typedef TokiMap walkableMap;


class edge
{
public:
    edge();
    edge(TokiPos,TokiPos);
    edge(ushort r1,ushort c1,ushort r2,ushort c2);
    TokiPos beg;
    TokiPos end;
    int lengthSquare;
    bool connectWith(TokiPos) const;
    void drawEdge(glassMap &,bool drawHead=false) const;
};

TokiMap ySlice2TokiMap(const Eigen::Tensor<uchar,3>&);

glassMap connectBetweenLayers(const TokiMap & ,const TokiMap & ,
                          walkableMap* walkable);
//返回值是架构在相对较高的一层上的，walkable是各层俯视图叠加

#ifdef WITH_QT
class PrimGlassBuilder : public QObject
{
    Q_OBJECT
public:
    explicit PrimGlassBuilder(QObject *parent = nullptr);
#else
class PrimGlassBuilder
{
public:
    PrimGlassBuilder();
#endif
    enum blockType{
            air=0,
            glass=1,
            target=127
        };
    glassMap makeBridge(const TokiMap & _targetMap,
                        walkableMap* walkable=nullptr);
#ifdef WITH_QT
signals:
    void progressRangeSet(int min,int max,int val);
    void progressAdd(int);
    void keepAwake();
#else
    void (*progressRangeSet)(int,int,int);
    void (*progressAdd)(int);
    void (*keepAwake)();
#endif
private:
    std::vector<TokiPos> targetPoints;
    std::list<edge> edges;
    std::vector<edge> tree;
    void addEdgesToGraph();
    void runPrim();
    glassMap make4SingleMap(const TokiMap & _targetMap,
                            walkableMap* walkable);
    static edge connectSingleMaps(const PrimGlassBuilder * map1,TokiPos offset1,
                                  const PrimGlassBuilder * map2, TokiPos offset2);
};

EImage TokiMap2EImage(const TokiMap&);


void defaultProgressRangeSet(int,int,int);
void defaultProgressAdd(int);
void defaultKeepAwake();

#endif // PRIMGLASSBUILDER_H
