#ifndef OPTITREE_H
#define OPTITREE_H
#include<stack>
#include <QDebug>
//#include <QList>
#include <QImage>
#include <Eigen/Dense>
#include <QRgb>
#include <iostream>
#include <cmath>
#include <list>
#include <queue>
#include <cmath>
//#include "heightline.cpp"
using namespace std;
using namespace Eigen;
class Pair
{
public:
    Pair(char='(',short=0);
    char type;
    short index;
    bool equalto(char);
    //bool operator==(char);
    ~Pair();
};


struct Region
{
    short Begin;
    short End;
};//Region是两端均为右端点/水/极大值区间的区间，其内不含极大值


class Node;
class HeightLine
{
public:
    HeightLine(int _size=129,char method='Z',int Low=0,int High=0);//Z->zeros,R->Random
    HeightLine(const VectorXi&HighL,const VectorXi&LowL);
    //VectorXi HighLine();
    //VectorXi LowLine();
    VectorXi DepthLine();
    bool isWater(int);
    bool isAir(int);
    bool isNormalBlock(int);
    int validHigh(int);
    int Size;//totalSize
    VectorXi HighLine;//Height refers to LowLine
    VectorXi LowLine;
    static MatrixXi Base;
    static short currentColum;
    static Vector3i Both;
    static Vector3i Left;
    static Vector3i Right;

    QImage toQImage();
    //void toBrackets(list<short>&index,list<char>&brackets);
    //void toBrackets(list<Pair>&);
    void segment2Brackets(list<Pair>&,short,short);
    //void toWaterRegion(queue<Region>&);
    void DealRegion(Region,list<Pair>&);
    void toSubRegion(queue<Region>&);
    VectorXi ValidHighLine();
    //void toBrackets_Near(list<short>&index,list<char>&brackets);
    //bool isContinious();
    void Sink(Node*);
    //void SinkMonotonous();
    void SinkBoundary(short=-1,short=-1);
    //void SinkInner();
    friend class OptiTree;
};

void disp(const list<Pair>&);

/*
bool operator>=(Region,Node*);
bool operator>=(Node*,Region);
bool conflictWith(Region,Node*);
bool conflictWith(Node*,Region);
bool isRightBeside(Region,Node*);
bool isRightBeside(Node*,Region);*/

class Node//二叉链，与二叉树没有本质区别
{
public:
    Node(short beg=-1,short end=-1);
    ~Node();
    void disp();
    friend class OptiTree;
    friend class HeightLine;
    /*
    friend bool operator>=(Region,Node*);
    friend bool conflictWith(Region,Node*);
    friend bool operator>=(Node*,Region);
    friend bool isRightBeside(Region,Node*);
    friend bool isRightBeside(Node*,Region);*/
//private:
    short Begin;
    short End;
    Node *Child;
    Node *Sib;
    bool isAble;
    short Degree;
    bool isComplete();
    short Length();
    void Freeze();
    bool haveChild();//指有无激活的子树
    bool haveSib();//指有无激活的平级树
    Node* creatChild();
    Node*creatChild(short,short);
    Node* creatSib();
    Node*creatSib(short,short);
    Node* insertChild(Node*newChild);
    Node* insertChild(Region);
    Node* insertSib(Node*newSib);
    Node* insertSib(Region);
    static Node* moveChild(Node*oldParent,Node*newParent);
    static Node* moveSib(Node*oldBrother,Node*newBrother);
    short maxSibEnd();
    void refreshDegree();
    Node* SetValue(short,short);
    //Node* findRightBesideBrother(Region);
};

class OptiTree
{
public:
    OptiTree();
    ~OptiTree();//析构函数，删除所有节点
    void ShowTree();
    void NaturalOpti(VectorXi&,VectorXi&);

    //void add(Region);
private:
    void NaturalOpti(HeightLine&,short,short);
    Node *Current();
    void goUp();
    void goDown();
    void goNextSib();
    void goPrevSib();
    bool isRoot();



    void gotoRoot();

    void BuildTree(HeightLine&,short=-1,short=-1);

    void Compress(HeightLine&);

    void FreezeTree();
    Node*Root;
    stack<Node*> Stack;
    void preventEmpty();
};

#endif // OPTITREE_H
