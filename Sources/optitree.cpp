#include "optitree.h"
//#define HL_Rand_1

#define NoOutPut

#define HighThreshold 1.5/3
#define LowThreshold 0.1/3
#define maxIterTimes 5000

Vector3i HeightLine::Both(-1,2,-1);
Vector3i HeightLine::Left(-1,1,0);
Vector3i HeightLine::Right(0,1,-1);

Pair::Pair(char _type,short _index)
{
    type=_type;index=_index;
}

bool Pair::equalto(char _type)
{
    return type==_type;
}

Pair::~Pair()
{
    return;
}

Node::Node(short beg,short end)
{
    Begin=beg;
    End=end;
    Degree=0;
    Child=NULL;
    Sib=NULL;
    isAble=true;
}

void Node::disp()
{
    cout<<'['<<Begin<<','<<End<<']';
    if(haveChild())
    {
        cout<<'{';
        Child->disp();
        cout<<'}';
    }
    if(haveSib())
    {
        cout<<'-';
        Sib->disp();
    }
}

inline bool Node::haveChild()
{
    return (Child!=NULL)&&(Child->isAble);
}

inline bool Node::haveSib()
{
    return (Sib!=NULL)&&(Sib->isAble);
}

inline bool Node::isComplete()
{
    return (Begin>=0)&&(End>=Begin);
}

Node* Node::creatChild()
{
    if(haveChild())
    {
        qDebug("已存在激活的子树，不能creatChild");
        Child->Degree=Degree+1;
        return Child;
    }
    if(Child==NULL)Child=new Node;

    Child->isAble=true;
    Child->Degree=Degree+1;
    return Child;
}

Node* Node::creatSib()
{
    if(haveSib())
    {
        qDebug("已存在激活的侧链，不能creatSib");
        Sib->Degree=Degree;
        return Sib;
    }
    if(Sib==NULL)Sib=new Node;

    Sib->isAble=true;
    Sib->Degree=Degree;
    return Sib;
}

void Node::Freeze()
{
    Begin=-1;End=-1;
    if(!isAble)return;
    isAble=false;
    if(haveChild())Child->Freeze();
    if(haveSib())Sib->Freeze();
}

void Node::refreshDegree()
{
    if(haveChild())
    {
        Child->Degree=Degree+1;
        Child->refreshDegree();
    }
    if(haveSib())
    {
        Sib->Degree=Degree;
        Sib->refreshDegree();
    }
}

Node* Node::insertChild(Node *newChild)
//在自己与newChild之间插入新的子节点
{
    Node *oldChild=Child;
    Child=newChild;
    Child->Child=oldChild;
    Child->isAble=true;
    refreshDegree();
    return Child;
}

inline Node* Node::insertChild(Region newR)
{
    return insertChild(new Node(newR.Begin,newR.End));
}

Node* Node::insertSib(Node *newSib)
//在自己与Sib之间插入新的侧链，并将旧Sib的侧链嫁接到新的Sib上
{
    Node*oldSib=Sib;
    Sib=newSib;
    Sib->Sib=oldSib;
    Sib->isAble=true;
    refreshDegree();
    return Sib;
}

inline Node* Node::insertSib(Region newS)
{
    return insertSib(new Node(newS.Begin,newS.End));
}

Node* Node::moveChild(Node *oldParent, Node *newParent)
{
    if(newParent->haveChild())
    {
        qDebug()<<"moveChild错误：newParent已有激活的Child";
        return NULL;
    }
    swap(oldParent->Child,newParent->Child);
    newParent->refreshDegree();
    return newParent->Child;
}

Node* Node::moveSib(Node *oldBrother, Node *newBrother)
{
    if(newBrother->haveSib())
    {
        qDebug()<<"moveSib错误：newBrother已有激活的Sib";
        return NULL;
    }
    swap(oldBrother->Sib,newBrother->Sib);
    newBrother->refreshDegree();
    return newBrother->Sib;
}

short Node::maxSibEnd()
{
    if(haveSib())
        return max(End,Sib->maxSibEnd());
    else
        return End;
}

OptiTree::OptiTree()
{
    Root=new Node;
    Root->isAble=true;
    Root->Degree=0;
    Stack.push(Root);
}

OptiTree::~OptiTree()
{
    delete Root;
}

Node* OptiTree::Current()
{
    return Stack.top();
}

inline short Node::Length()
{
    return End-Begin+1;
}

Node::~Node()
{
    if(Child!=NULL)delete Child;
    if(Sib!=NULL)delete Sib;
}

void OptiTree::goDown()
{
    if(!Current()->haveChild())
    {
        qDebug("goDown失败");
        return;
    }
    Stack.push(Current()->Child);
    //qDebug("goDown成功");
}

void OptiTree::goNextSib()
{
    if(!Current()->haveSib())
    {
        qDebug("goNextSib失败");
        return;
    }
    Stack.push(Current()->Sib);
    //qDebug("goNextSib成功");
}

void OptiTree::goPrevSib()
{
    preventEmpty();
    Node*Temp=Current();
    Stack.pop();
    if(Current()->Degree!=Temp->Degree||Current()->Sib!=Temp)
    {
        qDebug("goPrevSib失败");
        Stack.push(Temp);
        return;
    }
    //qDebug("goPrevSib成功");
}

void OptiTree::goUp()
{
    preventEmpty();
    Node*Temp=Current();
    //qDebug("开始goUp");
    while(!(Current()->Degree+1==Temp->Degree))Stack.pop();
    //qDebug("goUp成功");
}

void OptiTree::gotoRoot()
{
    while(!Stack.empty())Stack.pop();
    preventEmpty();
    //qDebug("gotoRoot成功");
}

inline void OptiTree::preventEmpty()
{
    if(Stack.empty())Stack.push(Root);
}

inline bool OptiTree::isRoot()
{
    return Current()==Root;
}

HeightLine::HeightLine(int _size,char method,int Low,int High)
{
    if (_size>0)Size=_size;
    else
        return;
    HighLine.setZero(Size);
    LowLine.setZero(Size);
    VectorXi Offset;
    Offset.setZero(Size);
    if(method=='R')
    {
#ifdef HL_Rand_1
        HighLine.setRandom();
        HighLine.array()-=HighLine.minCoeff();
        HighLine-=3*(HighLine/3);
        HighLine.array()-=1;
#else
        static bool isFirst=true;
        if(isFirst)
        {
        srand(time(0));
        isFirst=false;
        }
        float x=(rand()%32768)/32767.0;
        for(int i=0;i<Size;i++)
        {
            x=4.0*x*(1.0-x);
            if(x>HighThreshold)
                HighLine(i)=1;
            if(x<LowThreshold)
                HighLine(i)=-1;
        }


        if(rand()%2)
            HighLine*=-1;
#endif
        /*Height=(Height.array()<=0).select(Height,1);
        Height=(Height.array()>=0).select(Height,-1);*/
        for(int i=1;i<Size;i++)
        {

            if(rand()%5==6){
                switch (rand()%3)
                {
                case 0:
                    Offset(i)=1;
                    break;
                case 1:
                    Offset(i)=6;
                    break;
                case 2:
                    Offset(i)=11;
                    break;
                }
                if(Offset(i-1)>0)
                    HighLine(i)=HighLine(i-1);
                else
                    HighLine(i)=HighLine(i-1)+1;
            }
            else
            {
                if(Offset(i-1)>0)
                HighLine(i)+=HighLine(i-1)-1;
                else
                    HighLine(i)+=HighLine(i-1);
            }
        }
        LowLine=HighLine-Offset;
        HighLine.array()-=LowLine.minCoeff();
        LowLine.array()-=LowLine.minCoeff();

    }

    if(method=='L')
    {
        HighLine.setLinSpaced(Low,High);
        LowLine=HighLine;
        return;
    }

    //if(method=='Z')


    HighLine.array()-=LowLine.minCoeff();
    LowLine.array()-=LowLine.minCoeff();
    //cout<<Height.transpose()<<endl;
}

HeightLine::HeightLine(const VectorXi&HighL,const VectorXi&LowL)
{
    HighLine=HighL;
    LowLine=LowL;
    Size=HighLine.size();
}

inline bool HeightLine::isWater(int index)
{
    return HighLine(index)-LowLine(index);//0->普通方块，>=1 -> 水柱
}//水柱罩顶玻璃计入最高，但托底玻璃不计入

QImage HeightLine::toQImage()
{
    int H=HighLine.maxCoeff()-LowLine.minCoeff()+1;
    QImage img(Size,max(H,min(Size/5,100)),QImage::Format_ARGB32);
    QRgb isT=qRgb(0,0,0),isF=qRgb(255,255,255),Water=qRgb(0,64,255),grey=qRgb(192,192,192);
    img.fill(isF);

    for(int i=0;i<Size;i++)
    {
        img.setPixelColor(i,img.height()-1-HighLine(i),isT);
        if(isWater(i))
        {
            img.setPixelColor(i,img.height()-1-HighLine(i),grey);
            for(int y=LowLine(i);y<HighLine(i);y++)
                img.setPixelColor(i,img.height()-1-y,Water);
            if(LowLine(i)>=1)
                img.setPixelColor(i,img.height()-1-(LowLine(i)-1),grey);
        }
    }
    return img;
}

void HeightLine::toBrackets(list<short>&index,list<char>&brackets)
{
    if(Size<=0)return;
    index.clear();brackets.clear();
    brackets.push_back('(');index.push_back(0);
    int LOffset=0,ROffset=0;
    stack<short>S;//最后一步再加最外侧的大括号，将L与R的补偿设为1，自动囊括。

    //考虑更优的补全括号方式：不一定要补全到两端，也许可以补全到最近的一个左/又括号
    for(int i=1;i<Size;i++)
    {
        if(isWater(i)||validHigh(i)>validHigh(i-1))//上升处，i-1为孤立区间的末尾
        {
            if(S.empty())LOffset++;
            else
                S.pop();
            brackets.push_back(')');
            index.push_back(i-1);
        }
        if(isWater(i)||validHigh(i)<validHigh(i-1))//下降处，是孤立区间的起始
        {
            S.push('F');
            brackets.push_back('(');
            index.push_back(i);
        }



    }
    ROffset+=1+S.size();

    //qDebug()<<"LOffset="<<LOffset<<";  ROffset="<<ROffset;
    //cout<<"Raw="<<brackets<<endl;

    for(;LOffset>0;LOffset--)
    {
        index.insert(index.begin(),0);
        brackets.push_front('(');
    }
    for(;ROffset>0;ROffset--)
    {
        index.push_back(Size-1);
        brackets.push_back(')');
    }

}

void HeightLine::toBrackets(list<Pair> &List)
{
    if(Size<=0)return;
    List.clear();
    queue<Region>Pure;
    queue<Region> disPure;//极大值区间
    Region Temp;
    bool isReady=false;
    VectorXi VHL=ValidHighLine();
    VectorXi ScanBoth=VHL,ScanLeft=VHL,ScanRight=VHL;
    ScanBoth.setZero();ScanLeft.setZero();ScanRight.setZero();
    for(int i=1;i<Size-1;i++)//分别用三个算子处理
    {
        if(isWater(i))
        {
            /*ScanBoth(i)=2;
            ScanLeft(i)=1;
            ScanRight(i)=1;
            continue;*/
        }
        ScanBoth(i)=(VHL.segment(i-1,3).array()*Both.array()).sum();
        ScanLeft(i)=(VHL.segment(i-1,3).array()*Left.array()).sum();
        ScanRight(i)=(VHL.segment(i-1,3).array()*Right.array()).sum();
    }
ScanBoth=(ScanBoth.array()>=0).select(ScanBoth,0);
ScanLeft=(ScanLeft.array()>=0).select(ScanLeft,0);
ScanRight=(ScanRight.array()>=0).select(ScanRight,0);
/*
cout<<"ScanBoth="<<endl<<ScanBoth.transpose()<<endl;
cout<<"ScanLeft="<<endl<<ScanLeft.transpose()<<endl;
cout<<"ScanRight="<<endl<<ScanRight.transpose()<<endl;*/
    isReady=false;
    for(int i=1;i<Size-1;i++)
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
    Temp.Begin=0;
    Temp.End=Size-1;
    while(!disPure.empty())
    {
        Temp.End=disPure.front().Begin-1;
        Pure.push(Temp);
        Temp.Begin=disPure.front().End+1;
        disPure.pop();
        if(Temp.Begin>=Size-1)Temp.Begin=Size-1;
        Temp.End=Size-1;
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
    List.push_front(Pair('(',0));
    List.push_back(Pair(')',Size-1));
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

void HeightLine::toWaterRegion(queue<Region> &RList)
{

    Region Temp;
    Temp.Begin=0;Temp.End=Size-1;
    for(int i=1;i<Size;i++)
    {
        //qDebug("rue~");
        if(isWater(i))
        {
            Temp.End=i-1;
            RList.push(Temp);
            Temp.End=Size-1;
            Temp.Begin=i;
        }
    }
    RList.push(Temp);
}

inline bool HeightLine::isContinious()
{
    return (HighLine.segment(1,Size-2).array()-HighLine.segment(0,Size-2).array()==0).all();
}
inline int HeightLine::validHigh(int index)
{
    if(isWater(index))return HighLine(index)-1;
    else return HighLine(index);
}
VectorXi HeightLine::DepthLine()
{
    VectorXi Depth=HighLine.segment(1,Size-1).array()*0;
    for(int i=0;i<Size-1;i++)
    {
        if(isWater(i+1))
        {
            switch (HighLine(i+1)-LowLine(i+1)) {
            case 0+1:
                Depth(i)=0;
                break;
            case 5+1:
                Depth(i)=1;
                break;
            case 10+1:
                Depth(i)=2;
                break;
            }
        }
        else
        {

                if(validHigh(i+1)<validHigh(i))
                {
                    Depth(i)=0;
                    continue;
                }
                if(validHigh(i+1)==validHigh(i))
                {
                    Depth(i)=1;
                    continue;
                }
                if(validHigh(i+1)>validHigh(i))
                {
                    Depth(i)=2;
                    continue;
                }
        }
    }
    return Depth;
}

inline VectorXi HeightLine::ValidHighLine()
{
    return ((HighLine-LowLine).array()==0).select(HighLine,HighLine.array()-1);
}

/*
void HeightLine::toBrackets_Near(list<short> &index, string &brackets)
{

}*/

/*void HeightLine::SinkMonotonous()
{
    if(isContinious())return;
    int IndexB=0,IndexE=0;
    int gapB=0,gapE=0;//表示不连续段的落差绝对值
    bool isReady=false,isIncrease=false;
    for(int i=1;i<Size;i++)
    {
        isReady=isReady&&(isIncrease==(Height(i)>Height(i-1)));//如果区间不单调，不予沉降
        if(abs(Height(i)-Height(i-1))>=2)//出现不连续
        {
            if(!isReady)//进入了不连续区间
            {
                isIncrease=Height(i)>Height(i-1);
                gapB=abs(Height(i)-Height(i-1))-1;
                isReady=true;
                IndexB=i;
                continue;
            }
            if(isReady)//将要结束一个不连续单调区间
            {
                gapE=abs(Height(i)-Height(i-1))-1;
                IndexE=i-1;
                Height.segment(IndexB,IndexE-IndexB+1).array()-=min(isIncrease?gapB:gapE,Height.segment(IndexB,IndexE-IndexB+1).minCoeff());
                isReady=false;
                IndexB=-1;IndexE=-1;
                qDebug()<<"沉降了一个漂浮单调递"<<(isIncrease?"增":"减")<<"区间";
            }
        }
    }
}*/

inline void HeightLine::Sink(Node*rg)
{
    if(rg->isComplete())
    {
        HighLine.segment(rg->Begin,rg->Length()).array()-=LowLine.segment(rg->Begin,rg->Length()).minCoeff();
        LowLine.segment(rg->Begin,rg->Length()).array()-=LowLine.segment(rg->Begin,rg->Length()).minCoeff();
        //qDebug()<<"沉降了["<<rg->Begin<<','<<rg->End<<']';
    }
}

void HeightLine::SinkBoundary()
{
    if(isContinious())return;
    int gapB=0,gapE=0;
    for(int i=0;i<Size-1;i++)//正向遍历，去除前端浮空
    {
        if(validHigh(i)-validHigh(i+1)>=2)//右浮空
        {
            gapE=validHigh(i)-validHigh(i+1);//表示不连续段的落差
            HighLine.segment(0,i+1).array()-=min(gapE-1,LowLine.segment(0,i+1).minCoeff());
            LowLine.segment(0,i+1).array()-=min(gapE-1,LowLine.segment(0,i+1).minCoeff());
            break;
        }
    }

    for(int i=Size-1;i>0;i--)
    {
        if(validHigh(i)-validHigh(i-1)>=2)//左浮空
        {
            gapB=validHigh(i)-validHigh(i-1);
            HighLine.segment(i,Size-i).array()-=min(gapB-1,LowLine.segment(i,Size-i).minCoeff());
            LowLine.segment(i,Size-i).array()-=min(gapB-1,LowLine.segment(i,Size-i).minCoeff());
            break;
        }
    }

    int FBegin=1,FEnd=Size-1;
    bool isReady=false;
    //bool isBSinkable=false,isESinkable=false;
    /*
    for(int i=1;i<Size;i++)
    {
        if(abs(Height(i)-Height(i-1))>=2)
        {
            if(!isReady)
            {
                FBegin=i;
                gapB=Height(i)-Height(i-1);
                isBSinkable=gapB>0;
                gapB=abs(gapB)-1;
                isReady=true;
                continue;
            }
            if(isReady)
            {
                FEnd=i-1;
                gapE=Height(i)-Height(i-1);
                isESinkable=gapE<0;
                gapE=abs(gapE)-1;
                Height.segment(FBegin,FEnd-FBegin+1).array()-=min(min(isBSinkable*gapB,isESinkable*gapE),Height.segment(FBegin,FEnd-FBegin+1).minCoeff());
                isReady=false;

            }
        }
    }*/


    for(int i=1;i<Size-1;i++)//从i=1遍历至i=Size-2
    {
        if(validHigh(i)-validHigh(i-1)>=2)//左浮空
        {FBegin=i;
            gapB=validHigh(i)-validHigh(i-1);
            isReady=true;
        }
        if(validHigh(i)-validHigh(i+1)>=2&&isReady)//右浮空
        {
            FEnd=i;
            gapE=validHigh(i)-validHigh(i+1);
            HighLine.segment(FBegin,FEnd-FBegin+1).array()-=min(min(gapB,gapE)-1,LowLine.segment(FBegin,FEnd-FBegin+1).minCoeff());
            LowLine.segment(FBegin,FEnd-FBegin+1).array()-=min(min(gapB,gapE)-1,LowLine.segment(FBegin,FEnd-FBegin+1).minCoeff());
            //qDebug("沉降了中间的漂浮段");
            isReady=false;
        }
    }

}

/*void HeightLine::SinkInner()
{
    queue<short>Gap,Index;//取断崖的前值为index
    int GapVal=0,IndexVal=0;
    bool isBSinkable=false,isESinkable=false;
    int Offset=0;
    for(int i=1;i<Size;i++)
    {
        if(abs(GapVal=Height(i)-Height(i-1))>=2)
        {
            Gap.push(GapVal);//Gap值为后减前
            Index.push(i-1);
            qDebug()<<i-1;
        }
    }
    if(Gap.size()<=1)return;
    do
    {
        GapVal=Gap.front();IndexVal=Index.front();
        Gap.pop();Index.pop();
        isBSinkable=GapVal>0;//对于一段内部连续的区间，前沟大于0才有意义
        isESinkable=Gap.front()<0;
        if(!isBSinkable&&!isESinkable)continue;
        if(isBSinkable&&isESinkable)
            Offset=min(GapVal,abs(Gap.front()))-1;
        if(isBSinkable&&!isESinkable)
            Offset=abs(Gap.front())-1;
        if(!isBSinkable&&isESinkable)
            Offset=GapVal-1;
        Height.segment(IndexVal,Index.front()-IndexVal+1).array()-=min(Offset,Height.segment(IndexVal,Index.front()-IndexVal+1).minCoeff());
    }
    while(!Gap.empty());

}*/


void OptiTree::NaturalOpti(VectorXi &HighL,VectorXi&LowL)
{
    HeightLine HL(HighL,LowL);
    NaturalOpti(HL);
    LowL=HL.LowLine;
    HighL=HL.HighLine;
}


void OptiTree::BuildTree(HeightLine &HL)
{
    FreezeTree();
    gotoRoot();
    preventEmpty();

    list<Pair> Index;

    HL.toBrackets(Index);

    auto iter=Index.begin();

#ifndef NoOutPut
    qDebug("开始BuildTree");
#endif

    for(;iter!=Index.end();)
    {
        if(iter->equalto('('))
        {//左括号
            //cout<<'(';
            if(Current()->isComplete())
            {//如果当前节点已完成，创建侧链并写入Begin
                Current()->creatSib()->Begin=iter->index;
                goNextSib();
                //iter++;iter++;continue;
            }
            else
            {//如果当前节点Begin完成但End未完成，则创建子树，写入Begin
                if(Current()->Begin>=0)
                {
                    Current()->creatChild()->Begin=iter->index;
                    goDown();
                }
                else
                {//否则写入Begin
                    Current()->Begin=iter->index;
                }
            }
        }

        if(iter->equalto(')'))
        {//右括号
            //cout<<')';
            //qDebug("rue");
            if(Current()->isComplete())
            {//如果当前节点已完成，则向上，写入End
                //qDebug()<<"当前度数"<<Current()->Degree<<"，即将goUp";
                goUp();
                Current()->End=iter->index;
            }
            else
            {//如果当前节点未完成，则Begin必然已经完成（否则报错），写入End
                if(Current()->Begin<0)
                {
                    qDebug("出现错误：不成对的括号：过多的右括号");
                    return;
                }
                Current()->End=iter->index;
                //qDebug("rua!");
            }
        }
        iter++;
    }
#ifndef NoOutPut
    cout<<endl;
    qDebug("开始插入水区间分段");
#endif
    queue<Region> waterRegion;
    HL.toWaterRegion(waterRegion);
    while(!waterRegion.empty())
    {
        add(waterRegion.front());
        waterRegion.pop();
#ifndef NoOutPut
        qDebug("rua!");
#endif
    }
#ifndef NoOutPut
    qDebug("优化树构建完毕");
    ShowTree();
#endif
}

void OptiTree::Compress(HeightLine &HL)
{
    //DFS中序遍历
    HL.Sink(Current());
    if(Current()->haveChild())
    {
        goDown();
        Compress(HL);
        goUp();
    }
    if(Current()->haveSib())
    {
        goNextSib();
        Compress(HL);
        goPrevSib();
    }
}

void OptiTree::FreezeTree()
{
    Root->Freeze();
    Root->isAble=true;
}

void OptiTree::ShowTree()
{
    cout<<endl;
    Root->disp();
    cout<<endl;
}

void OptiTree::NaturalOpti(HeightLine& HL)
{
    BuildTree(HL);
    gotoRoot();
    Compress(HL);
    HL.SinkBoundary();
    HL.SinkBoundary();
}

void disp(const list<Pair>&L)
{
    if(L.empty())return;
    cout<<endl;
    for(auto i=L.cbegin();i!=L.cend();i++)
        cout<<" "<<i->type;
    cout<<endl;
    for(auto i=L.cbegin();i!=L.cend();i++)
        cout<<" "<<i->index;
    cout<<endl;
    cout<<endl;
}

bool operator>=(Region a,Node*b)
{
    return (a.Begin<=b->Begin)&&(a.End>=b->End);
}

bool operator>=(Node*a,Region b)
{
    return (a->Begin<=b.Begin)&&(a->End>=b.End);
}

bool conflictWith(Region a,Node*b)
{
    if(a>=b||b>=a)return false;
    return ((a.End-b->Begin)*(a.Begin-b->End)<0);
}

inline bool conflictWith(Node *a,Region b)
{
    return conflictWith(b,a);
}

inline bool isRightBeside(Region a,Node*b)//指示a在b的左侧成立
{
    return (a.End<=b->Begin);
}

inline bool isRightBeside(Node*a,Region b)
{
    return (a->End<=b.Begin);
}

Node* Node::SetValue(short beg, short end)
{
    Begin=beg;End=end;
    return this;
}

Node* Node::creatChild(short beg, short end)
{
    return creatChild()->SetValue(beg,end);
}

Node* Node::creatSib(short beg, short end)
{
    return creatSib()->SetValue(beg,end);
}

Node* Node::findRightBesideBrother(Region Reg)
//返回最近的与Reg右邻的节点的brother
{
    //如果haveSib且Sib不在Reg的右侧，则向右继续寻找
    //如果haveSib且Sib在Reg的右侧，返回this
    //如果!haveSib，则return this
    if(!haveSib())return this;
    if(haveSib()&&isRightBeside(Reg,Sib))return this;

    return Sib->findRightBesideBrother(Reg);
}

void OptiTree::add(Region newR)
{
    gotoRoot();
    int i=0;
#ifndef NoOutPut
    qDebug()<<"尝试插入区间["<<newR.Begin<<','<<newR.End<<']';
#endif
    while(++i)
    {
        if(i>=maxIterTimes)
        {
            qDebug()<<"'区间["<<newR.Begin<<','<<newR.End<<"]连续尝试超过"<<maxIterTimes<<"次，强制停止";
            return;
        }
        if(conflictWith(Current(),newR))
        {
#ifndef NoOutPut
            qDebug()<<"区间["<<newR.Begin<<','<<newR.End<<"]与当前节点冲突，不可插入";
#endif
            return;
        }
        if(Current()>=newR)
        {
            if(!Current()->haveChild())
            {
                Current()->creatChild()->SetValue(newR.Begin,newR.End);
#ifndef NoOutPut
                qDebug()<<"成功插入区间["<<newR.Begin<<','<<newR.End<<']';
#endif
                return;
            }
            else if(newR>=Current()->Child)
            {
                Node::moveSib(Current()->Child->findRightBesideBrother(newR),Current()->insertChild(newR));
#ifndef NoOutPut
                qDebug()<<"成功插入区间["<<newR.Begin<<','<<newR.End<<']';
#endif
                return;
            }
            else
            {
                goDown();continue;
            }
        }

        if(Current()->haveSib()&&isRightBeside(Current(),newR)&&newR>=Current()->Sib)
        {

            Current()->insertSib(newR);
            swap(Current()->Sib->Sib,Current()->Sib->Child);
            Node* temp=Current()->Sib->Child->findRightBesideBrother(newR);
            temp->disp();
            Node::moveSib(temp,Current()->Sib);
#ifndef NoOutPut
            qDebug()<<"成功插入区间["<<newR.Begin<<','<<newR.End<<']';
#endif
            return;
        }


        if(isRightBeside(newR,Current()))
        {
            goPrevSib();
            continue;
        }

        if(isRightBeside(Current(),newR))
        {
            if(!Current()->haveSib()||isRightBeside(newR,Current()->Sib))
            {
                Current()->insertSib(newR);
#ifndef NoOutPut
                qDebug()<<"成功插入区间["<<newR.Begin<<','<<newR.End<<']';
#endif
                return;
            }
            else
            {
                goNextSib();
                continue;
            }
        }
    }

}
