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


#ifndef Page5_H
#define Page5_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "NBTWriter.h"
#include "WaterItem.h"
#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include "D:/zlib-1.2.11/zlib.h"
#define bufferSize 1024
bool compressFile(const char*sourcePath,const char*destPath)
{
    char buf[bufferSize]={0};
    FILE*in=NULL;
    gzFile out=NULL;
    int len=0;
    fopen_s(&in,sourcePath,"rb");
    out=gzopen(destPath,"wb");
    if(in==NULL||out==NULL)
        return false;
    while(true)
    {
        len=(int)fread(buf,1,sizeof(buf),in);
        if(ferror(in))return false;
        if(len==0)break;
        if(len!=gzwrite(out,buf,(unsigned)len))
            return false;
        memset(buf,0,sizeof(buf));
    }
    fclose(in);
    gzclose(out);
    qDebug("succeed");
    return true;
}

void MainWindow::on_AllowNaturalOpti_stateChanged(int arg1)
{
    Data.allowNaturalOpti=Data.isSurvival()&&ui->AllowNaturalOpti->isChecked();
}

void MainWindow::on_ExportLite_clicked()
{
    if(Data.ExLitestep<2)return;
    if(Data.step<5)return;
    QString FileName=QFileDialog::getSaveFileName(this,tr("导出为投影/结构方块文件"),"",tr("投影文件(*.litematic) ;; 结构方块文件(*.nbt)"));

    if(FileName.isEmpty())return;

    if(!FileName.endsWith(".litematic")&&!FileName.endsWith(".nbt"))
    {
        qDebug("得到的文件路径有错！");
        return;
    }
    QString unCompressed;
    if(FileName.endsWith(".litematic"))
        unCompressed=FileName.left(FileName.length()-strlen(".litematic"))+".TokiNoBug";
    else
        unCompressed=FileName.left(FileName.length()-strlen(".nbt"))+".TokiNoBug";
    ui->ShowProgressExLite->setMaximum(100+Data.size3D[0]*Data.size3D[1]*Data.size3D[2]);
    ui->ShowProgressExLite->setValue(0);

    qDebug("开始导出投影");

    if(FileName.endsWith(".litematic"))
        Data.exportAsLitematica(unCompressed);
    else
        Data.exportAsStructure(unCompressed);

    if(compressFile(unCompressed.toLocal8Bit().data(),FileName.toLocal8Bit().data()))
    {
        ui->ShowProgressExLite->setValue(100+Data.Build.size());
        qDebug("压缩成功");
        Data.ExLitestep=4;
        QFile umComFile(unCompressed);
        umComFile.remove();
        Data.ProductPath=FileName;
    }
    else
    {
        qDebug("压缩失败");
        return;
    }

    Data.step=6;updateEnables();
    qDebug("导出为投影成功");
    ui->FinishExLite->setEnabled(true);    
    ui->seeExported->setEnabled(true);
    return;

}

void MainWindow::on_Build4Lite_clicked()
{
    ui->ShowProgressExLite->setMaximum(8*Data.sizePic[0]*Data.sizePic[1]);
    ui->ShowProgressExLite->setValue(0);

    ui->ExportLite->setEnabled(false);
    ui->FinishExLite->setEnabled(false);



    Data.ExLitestep=0;

    qDebug("开始makeHeight");
    Data.makeHeight();
    ui->ShowLiteXYZ->setText("X:"+QString::number(Data.size3D[0])+"  × Y:"+QString::number(Data.size3D[1])+"  × Z:"+QString::number(Data.size3D[2]));
    Data.ExLitestep=1;    
    Data.step=5;
    updateEnables();
    updateEnables();
    ui->ShowProgressExLite->setValue(5*Data.sizePic[0]*Data.sizePic[1]);

    qDebug("开始BuildHeight");
    long blockCount=Data.BuildHeight();
    ui->ShowLiteBlocks->setText(QString::number(blockCount));
    Data.ExLitestep=2;

    ui->ShowProgressExLite->setValue(8*Data.sizePic[0]*Data.sizePic[1]);
    ui->ExportLite->setEnabled(true);

    showPreview();
    ui->ManualPreview->setEnabled(true);
}

long mcMap::makeHeight()//整张图片将被遍历5遍（向量化的遍历也被视为遍历）
{
    int u=0;
    qDebug()<<u++;
if(step<5)return 0;
if(ExLitestep<0)return 0;

Base.setConstant(sizePic[0]+1,sizePic[1],11);

Base.block(1,0,sizePic[0],sizePic[1])=mapPic/4;

ArrayXXi dealedDepth;
ArrayXXi rawShadow=mapPic-4*(mapPic/4);

if((dealedDepth>=3).any())
{
    qDebug("错误：Depth中存在深度为3的方块");
    return -1;
}
dealedDepth.setZero(sizePic[0]+1,sizePic[1]);
dealedDepth.block(1,0,sizePic[0],sizePic[1])=rawShadow-1;
//Depth的第一行没有意义，只是为了保持行索引一致
WaterList.clear();

for(short r=0;r<Base.rows();r++)
{
    for(short c=0;c<Base.cols();c++)
    {
        if(Base(r,c)==12)
        {
            WaterList[TokiRC(r,c)]=nullWater;
            dealedDepth(r,c)=0;
            continue;
        }
        if(Base(r,c)==0)
        {
            dealedDepth(r,c)=0;
            continue;
        }
    }
    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[0]);
}

HighMap.setZero(sizePic[0]+1,sizePic[1]);
LowMap.setZero(sizePic[0]+1,sizePic[1]);

int waterCount=WaterList.size();
qDebug()<<"共有"<<waterCount<<"个水柱";
for(short r=0;r<sizePic[0];r++)//遍历每一行，根据高度差构建高度图
{
    HighMap.row(r+1)=HighMap.row(r)+dealedDepth.row(r+1);
    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[0]);
}

for(short c=0;c<Base.cols();c++)
{
    if(Base(1,c)==0||Base(1,c)==12||rawShadow(0,c)==2)
    {
        Base(0,c)=0;
        HighMap(0,c)=HighMap(1,c);
    }
    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[1]);
}


LowMap=HighMap;

for(auto it=WaterList.begin();it!=WaterList.end();it++)
{
    LowMap(TokiRow(it.key()),TokiCol(it.key()))=HighMap(TokiRow(it.key()),TokiCol(it.key()))-WaterColumnSize[rawShadow(TokiRow(it.key())-1,TokiCol(it.key()))]+1;
}

for(short c=0;c<sizePic[1];c++)
{
    HighMap.col(c)-=LowMap.col(c).minCoeff();
    LowMap.col(c)-=LowMap.col(c).minCoeff();
    //沉降每一列
    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[1]);
}

if(allowNaturalOpti)
{
    //执行高度压缩
    OptiChain::Base=Base;
    for(int c=0;c<sizePic[1];c++)
    {
        OptiChain Compressor(HighMap.col(c),LowMap.col(c),c);
        Compressor.divideAndCompress();
        HighMap.col(c)=Compressor.HighLine;
        LowMap.col(c)=Compressor.LowLine;
        parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[1]);
    }
}

int maxHeight=HighMap.maxCoeff();

for(auto it=WaterList.begin();it!=WaterList.end();it++)
{
    int r=TokiRow(it.key()),c=TokiCol(it.key());
    it.value()=TokiWater(HighMap(r,c),LowMap(r,c));
    maxHeight=max(maxHeight,HighMap(r,c)+1);
    //遮顶玻璃块
}

#ifdef putMapData
    putMap("D:\\extreme_",HighMap,LowMap);
#endif



    size3D[2]=2+sizePic[0];//z
    size3D[0]=2+sizePic[1];//x
    size3D[1]=1+maxHeight;//y

    ExLitestep=1;

    return (sizePic[0]+1)*sizePic[1];
}

long mcMap::BuildHeight()//进度条上表现为遍历3遍图像
{

    if(step<5)return 0;
    if(ExLitestep<1)return 0;
    Build.resize(size3D[0],size3D[1],size3D[2]);
    Build.setZero();
    int TotalBlockCount=0;
    //Base(r+1,c)<->High(r+1,c)<->Build(c+1,High(r+1,c),r+1)
    //为了区分玻璃与空气，张量中存储的是Base+1.所以元素为1对应着玻璃，0对应空气
    int x=0,y=0,z=0;
    int yLow=0;
    qDebug()<<"共有"<<WaterList.size()<<"个水柱";
    qDebug()<<2;
    for(auto it=WaterList.begin();it!=WaterList.end();it++)//水柱周围的玻璃
    {
        x=TokiCol(it.key())+1;
        z=TokiRow(it.key());
        y=waterHigh(it.value());
        yLow=waterLow(it.value());
        Build(x,y+1,z)=0+1;//柱顶玻璃
        for(short yDynamic=yLow;yDynamic<=y;yDynamic++)
        {
            Build(x-1,yDynamic,z-0)=1;
            Build(x+1,yDynamic,z+0)=1;
            Build(x+0,yDynamic,z-1)=1;
            Build(x+0,yDynamic,z+1)=1;
        }
        if(yLow>=1)       Build(x,yLow-1,z)=1;//柱底玻璃
    }
    qDebug()<<3;
    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[1]*sizePic[0]);

    for(short r=0;r<sizePic[0];r++)//普通方块
    {
        for(short c=0;c<sizePic[1];c++)
        {
            if(Base(r+1,c)==12||Base(r+1,c)==0)
                continue;
            x=c+1;y=HighMap(r+1,c);z=r+1;
            if(y>=1&&parent->NeedGlass[Base(r+1,c)][SelectedBlockList[Base(r+1,c)]])
                Build(x,y-1,z)=0+1;

            Build(x,y,z)=Base(r+1,c)+1;
        }
        parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[1]);
    }

qDebug()<<4;

parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+sizePic[1]*sizePic[0]);

for(auto it=WaterList.begin();it!=WaterList.end();it++)
{
    x=TokiCol(it.key())+1;
    z=TokiRow(it.key());
    y=waterHigh(it.value());
    yLow=waterLow(it.value());
    for(short yDynamic=yLow;yDynamic<=y;yDynamic++)
    {
        Build(x,yDynamic,z)=13;
    }
}

for(short c=0;c<sizePic[1];c++)//北侧方块
    if(Base(0,c))        Build(c+1,HighMap(0,c),0)=11+1;

for(x=0;x<size3D[0];x++)
    for(y=0;y<size3D[1];y++)
        for(z=0;z<size3D[2];z++)
            if(Build(x,y,z)) TotalBlockCount++;
totalBlocks=TotalBlockCount;

    return TotalBlockCount;
}

#ifdef putMapData
void mcMap::putMap(const QString &Path, const MatrixXi &HighMap, const MatrixXi &LowMap)
{
    fstream out;
    out.open((Path+"Base.Toki").toLocal8Bit().data(),ios::out|ios::binary);
    if(out.eof())
    {
        qDebug("out文件流打开失败(Base)");
        return;
    }
    qDebug("开始输出数据");

        for(int c=0;c<Base.cols();c++)
            for(int r=0;r<Base.rows();r++)
        {
            out.write((const char*)&Base(r,c),sizeof(int));
        }


    out.close();

    out.open((Path+"HighMap.Toki").toLocal8Bit().data(),ios::out|ios::binary);
    if(out.eof())
    {
        qDebug("out文件流打开失败(HighMap)");
        return;
    }

        for(int c=0;c<HighMap.cols();c++)
            for(int r=0;r<HighMap.rows();r++)
        {
            out.write((const char*)&HighMap(r,c),sizeof(int));
        }

    out.close();

    out.open((Path+"LowMap.Toki").toLocal8Bit().data(),ios::out|ios::binary);
    if(out.eof())
    {
        qDebug("out文件流打开失败(LowMap)");
        return;
    }

        for(int c=0;c<LowMap.cols();c++)
            for(int r=0;r<LowMap.rows();r++)
        {
            out.write((const char*)&LowMap(r,c),sizeof(int));
        }

    out.close();
    qDebug("数据输出完毕");
}

#endif

long mcMap::exportAsLitematica(QString FilePathAndName)
{
    NBT::NBTWriter Lite;

    Lite.open(FilePathAndName.toLocal8Bit().data());
    QString LiteName=parent->ui->InputLiteName->toPlainText();
    QString author=parent->ui->InputAuthor->toPlainText();
    QString RegionName=parent->ui->InputRegionName->toPlainText();
    RegionName+=QObject::tr("(xz坐标=-65±128×整数)");
    Lite.writeCompound("Metadata");
        Lite.writeCompound("EnclosingSize");
            Lite.writeInt("x",size3D[0]);
            Lite.writeInt("y",size3D[1]);
            Lite.writeInt("z",size3D[2]);
        Lite.endCompound();
        Lite.writeString("Author",author.toUtf8().data());
        Lite.writeString("Description","This litematic is generated by SlopeCraft, developer TokiNoBug");
        Lite.writeString("Name",LiteName.toUtf8().data());
        Lite.writeInt("RegionCount",1);
        Lite.writeLong("TimeCreated",114514);
        Lite.writeLong("TimeModified",1919810);
        Lite.writeInt("TotalBlocks",totalBlocks);
        Lite.writeInt("TotalVolume",size3D[0]*size3D[1]*size3D[2]);
    Lite.endCompound();
parent->ui->ShowProgressExLite->setValue(50);
    Lite.writeCompound("Regions");
        Lite.writeCompound(RegionName.toUtf8().data());
            Lite.writeCompound("Position");
                Lite.writeInt("x",0);
                Lite.writeInt("y",0);
                Lite.writeInt("z",0);
            Lite.endCompound();
            Lite.writeCompound("Size");
                Lite.writeInt("x",size3D[0]);
                Lite.writeInt("y",size3D[1]);
                Lite.writeInt("z",size3D[2]);
            Lite.endCompound();
            parent->ui->ShowProgressExLite->setValue(100);
            Lite.writeListHead("BlockStatePalette",NBT::idCompound,131);
                {
                    short written=(is16()?59:52);
                    if(is17())written=61;
                    vector<QString> ProName,ProVal;
                    //bool isNetBlockId;
                    QString netBlockId;

                    dealBlockId("air",netBlockId,ProName,ProVal);
                    writeBlock(netBlockId,ProName,ProVal,Lite);
                    for(short r=0;r<written;r++)
                    {
                        dealBlockId(BlockListId[r],netBlockId,ProName,ProVal);
                        writeBlock(netBlockId,ProName,ProVal,Lite);
                    }//到此写入了written+1个方块，还需要写入130-written个

                    writeTrash(130-written,Lite);
                }
            Lite.writeListHead("Entities",NBT::idCompound,0);
            Lite.writeListHead("PendingBlockTicks",NBT::idCompound,0);
            Lite.writeListHead("PendingFluidTiccks",NBT::idCompound,0);
            Lite.writeListHead("TileEntities",NBT::idCompound,0);
            {
                int ArraySize;
                //Lite.writeLong("aLong",1145141919810);
                int Volume=size3D[0]*size3D[1]*size3D[2];
                ArraySize=((Volume%8)?(Volume/8+1):Volume/8);
                long long HackyVal=sizeof(long long);
                char *inverser=(char*)&HackyVal;
                short inverserIndex=7;
            Lite.writeLongArrayHead("BlockStates",ArraySize);
            for(int y=0;y<size3D[1];y++)
                for(int z=0;z<size3D[2];z++)
                {
                    for(int x=0;x<size3D[0];x++)
                    {
                        inverser[inverserIndex--]=Build(x,y,z);

                        if(inverserIndex<0)
                        {

                            inverserIndex=7;
                            Lite.writeLongDirectly("id",HackyVal);
                        }
                    }
                    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+size3D[0]);
                }

            if(!Lite.isListFinished())
                Lite.writeLongDirectly("id",HackyVal);
            }
    Lite.endCompound();
Lite.endCompound();
switch (gameVersion)
{
case 12:
    Lite.writeInt("MinecraftDataVersion",1343);
    Lite.writeInt("Version",4);
    break;
case 13:
    Lite.writeInt("MinecraftDataVersion",1631);
    Lite.writeInt("Version",5);
    break;
case 14:
    Lite.writeInt("MinecraftDataVersion",1976);
    Lite.writeInt("Version",5);
    break;
case 15:
    Lite.writeInt("MinecraftDataVersion",2230);
    Lite.writeInt("Version",5);
    break;
case 16:
    Lite.writeInt("MinecraftDataVersion",2586);
    Lite.writeInt("Version",5);
    break;
case 17:
    Lite.writeInt("MinecraftDataVersion",2724);
    Lite.writeInt("Version",5);
    break;
default:
    qDebug("错误的游戏版本！");break;
}

Lite.close();
    return Lite.getByteCount();
}

long mcMap::exportAsStructure(QString FilePathAndName) {
    NBT::NBTWriter file;
    file.open(FilePathAndName.toLocal8Bit().data());
    file.writeListHead("entities",NBT::idByte,0);
    file.writeListHead("size",NBT::idInt,3);
        file.writeInt("This should never be shown",size3D[0]);
        file.writeInt("This should never be shown",size3D[1]);
        file.writeInt("This should never be shown",size3D[2]);
        file.writeListHead("palette",NBT::idCompound,70);
            {
                short written=(is16()?59:52);
                if(is17())written=61;
                vector<QString> ProName,ProVal;
                //bool isNetBlockId;
                QString netBlockId;

                dealBlockId("air",netBlockId,ProName,ProVal);
                writeBlock(netBlockId,ProName,ProVal,file);
                for(short r=0;r<written;r++)
                {
                    dealBlockId(BlockListId[r],netBlockId,ProName,ProVal);
                    writeBlock(netBlockId,ProName,ProVal,file);
                }//到此写入了written+1个方块，还需要写入69-written个

                writeTrash(69-written,file);
            }

        file.writeListHead("blocks",NBT::idCompound,Build.size());
            for(int x=0;x<size3D[0];x++)
                for(int y=0;y<size3D[1];y++) {
                    for(int z=0;z<size3D[2];z++) {
                        file.writeCompound("This should never be shown");
                            file.writeListHead("pos",NBT::idInt,3);
                                file.writeInt("This should never be shown",x);
                                file.writeInt("This should never be shown",y);
                                file.writeInt("This should never be shown",z);
                            file.writeInt("state",Build(x,y,z));
                        file.endCompound();
                    }
                    parent->ui->ShowProgressExLite->setValue(parent->ui->ShowProgressExLite->value()+size3D[2]);
                }
            switch (gameVersion)
            {
            case 12:
                file.writeInt("DataVersion",1343);
                break;
            case 13:
                file.writeInt("DataVersion",1631);
                break;
            case 14:
                file.writeInt("DataVersion",1976);
                break;
            case 15:
                file.writeInt("DataVersion",2230);
                break;
            case 16:
                file.writeInt("DataVersion",2586);
                break;
            case 17:
                file.writeInt("DataVersion",2724);
                break;
            default:
                qDebug("错误的游戏版本！");break;
            }
    return file.close();
}

QString MainWindow::Noder(const short *src,int size)
{
    QString dst;
    char*u=nullptr;
    for(int i=0;i<size;i++)
    {
        u=(char*)(&Data.Basic.XYZ(src[i]/4))+src[i]%4;
        dst.push_back(*u);
    }
    return dst;
}

void MainWindow::on_AllowNaturalOpti_clicked(bool checked)
{
    if(!checked)
    {
        ui->AllowForcedOpti->setDisabled(true);
        ui->AllowForcedOpti->setChecked(false);
        ui->maxHeight->setDisabled(true);
    }
    else
    {
        ui->AllowForcedOpti->setDisabled(true);
        //暂不支持，暂时锁死
    }
}

void MainWindow::on_ManualPreview_clicked()
{
    this->showPreview();
}

void MainWindow::on_AllowForcedOpti_stateChanged(int arg1)
{
    ui->maxHeight->setDisabled(false&&!ui->AllowForcedOpti->isChecked());

}

#endif
