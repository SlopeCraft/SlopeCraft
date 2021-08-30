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


#ifndef Page7_H
#define Page7_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
//#include <NBTWriter.h>

void MainWindow::on_InputDataIndex_textChanged()
{
    bool isIndexValid=false;
    const int indexStart=ui->InputDataIndex->toPlainText().toInt(&isIndexValid);
    isIndexValid=isIndexValid&&(indexStart>=0);
    if(isIndexValid)
    {
        if(ceil(Data.mapPic.rows()/128.0f)==1&&ceil(Data.mapPic.cols()/128.0f)==1)
        ui->ShowDataFileName->setText("map_"+QString::number(indexStart)+".dat");
        else
            ui->ShowDataFileName->setText("map_"+QString::number(indexStart)+".dat"+"~"+"map_"+QString::number(indexStart+ceil(Data.mapPic.cols()/128.0f)*ceil(Data.mapPic.rows()/128.0f)-1)+".dat");
        ui->ExportData->setEnabled(true);
        return;
    }

    ui->ShowDataFileName->setText(tr("你输入的起始序号不可用，请输入大于等于0的整数！"));
    ui->ExportData->setEnabled(false);
    return;
}


void MainWindow::on_ExportData_clicked()
{
    if(Data.step<5)return;
    bool isIndexValid=false;
    const int indexStart=ui->InputDataIndex->toPlainText().toInt(&isIndexValid);
    isIndexValid=isIndexValid&&(indexStart>=0);
    if(!isIndexValid)
    {
        QMessageBox::information(this,tr("你输入的起始序号不可用"),tr("请输入大于等于0的整数！"));
                    return;
    }
    string FolderPath=(QFileDialog::getExistingDirectory(this,tr("请选择导出的文件夹"))).toStdString();
    if(FolderPath.empty())
    {
        QMessageBox::information(this,tr("你选择的文件夹不存在！"),tr("你可以选择存档中的data文件夹"));
        return;
    }

    ui->InputDataIndex->setEnabled(false);
    ui->ExportData->setEnabled(false);
    ui->FinshExData->setEnabled(false);
    ui->ExportData->setText(tr("请稍等"));


    //FolderPath.replace("\\","/");
    for(auto it=FolderPath.begin();it!=FolderPath.end();it++)
        if(*it=='\\')*it='/';

    Data.exportAsData(FolderPath,indexStart);
    qDebug("导出地图文件成功");
    Data.step=6;updateEnables();

    ui->InputDataIndex->setEnabled(true);
    ui->ExportData->setEnabled(true);
    ui->FinshExData->setEnabled(true);
    ui->ExportData->setText(tr("导出"));
}

long mcMap::exportAsData(const string &FolderPath,const int indexStart)
{
const int rows=ceil(mapPic.rows()/128.0f);
const int cols=ceil(mapPic.cols()/128.0f);
//const int maxrr=rows*128;
//const int maxcc=cols*128;
parent->ui->ShowProgressExData->setValue(0);
parent->ui->ShowProgressExData->setMaximum(128*rows*cols);

int offset[2]={0,0};//r,c
int currentIndex=indexStart;
for(int c=0;c<cols;c++)
{
    for(int r=0;r<rows;r++)
    {
        offset[0]=r*128;
        offset[1]=c*128;

        string currentUn=FolderPath+"/map_"+std::to_string(currentIndex)+".TokiNoBug";
        string currentFile=FolderPath+"/map_"+std::to_string(currentIndex)+".dat";

        qDebug()<<"开始导出("<<r<<","<<c<<")的地图"<<QString::fromStdString(currentUn);

        NBT::NBTWriter MapFile(currentUn.data());

        switch (gameVersion)
        {
        case 13:
            break;
        case 14:
            MapFile.writeInt("DataVersion",1631);
            break;
        case 15:
            MapFile.writeInt("DataVersion",2230);
            break;
        case 16:
            MapFile.writeInt("DataVersion",2586);
            break;
        case 17:
            MapFile.writeInt("DataVersion",2724);
            break;
        default:
            qDebug("错误的游戏版本！");
            break;
        }
        MapFile.writeString("ExportedBy","Exported by SlopeCraft v3.3, developed by TokiNoBug");
        MapFile.writeCompound("data");
            MapFile.writeByte("scale",0);
            MapFile.writeByte("trackingPosition",0);
            MapFile.writeByte("unlimitedTracking",0);
            MapFile.writeInt("xCenter",0);
            MapFile.writeInt("zCenter",0);
            switch(gameVersion)
            {
            case 12:
                MapFile.writeByte("dimension",114);
                MapFile.writeShort("height",128);
                MapFile.writeShort("width",128);
                break;
            case 13:
                MapFile.writeListHead("banners",NBT::idCompound,0);
                MapFile.writeListHead("frames",NBT::idCompound,0);
                MapFile.writeInt("dimension",889464);
                break;
            case 14:
                MapFile.writeListHead("banners",NBT::idCompound,0);
                MapFile.writeListHead("frames",NBT::idCompound,0);
                MapFile.writeInt("dimension",0);
                MapFile.writeByte("locked",1);
                break;
            case 15:
                MapFile.writeListHead("banners",NBT::idCompound,0);
                MapFile.writeListHead("frames",NBT::idCompound,0);
                MapFile.writeInt("dimension",0);
                MapFile.writeByte("locked",1);
                break;
            case 16:
                MapFile.writeListHead("banners",NBT::idCompound,0);
                MapFile.writeListHead("frames",NBT::idCompound,0);
                MapFile.writeString("dimension","minecraft:overworld");
                MapFile.writeByte("locked",1);
                break;
            case 17:
                MapFile.writeListHead("banners",NBT::idCompound,0);
                MapFile.writeListHead("frames",NBT::idCompound,0);
                MapFile.writeString("dimension","minecraft:overworld");
                MapFile.writeByte("locked",1);
            }

            MapFile.writeByteArrayHead("colors",16384);
            unsigned char ColorCur=0;
                for(short rr=0;rr<128;rr++)
                {
                    for(short cc=0;cc<128;cc++)
                    {
                        if(rr+offset[0]<mapPic.rows()&&cc+offset[1]<mapPic.cols())
                        ColorCur=(unsigned char)mapPic(rr+offset[0],cc+offset[1]);
                        else
                            ColorCur=0;
                        MapFile.writeByte("this should never be seen",ColorCur);
                    }
                    parent->ui->ShowProgressExData->setValue(parent->ui->ShowProgressExData->value()+1);
                }
            MapFile.endCompound();
            MapFile.close();

            if(compressFile(currentUn.data(),currentFile.data()))
            {
                qDebug("压缩成功");
                QFile umComFile(QString::fromStdString(currentUn));
                umComFile.remove();
            }
            currentIndex++;
    }
}
    //scale=0,trackingPosition=0,unlimitedTracking=0(均为byte)在1.12~1.16均不变
    //xCenter=0,zCenter=0(int)均不变
    //colors(16384byte)均不变
    return 0;
}

#endif
