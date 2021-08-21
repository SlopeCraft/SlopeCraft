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


#ifndef Page3_H
#define Page3_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include  <Eigen/Dense>
#include <unsupported/Eigen/CXX11/Tensor>
#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#pragma once

void MainWindow::versionCheck()
{
    allowUpdateToCustom(false);
    short maxIndex=(Data.is16()?58:52);
    if(Data.is17())maxIndex=60;

    if(!Data.is17())
        for(short r=59;r<64;r++)
        {

            if(Enables[r]!=NULL)
            {
                disconnect(Enables[r],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
                Enables[r]->setChecked(false);
                Enables[r]->setEnabled(false);
                connect(Enables[r],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
            }

        }
    else    if(!Data.is16())
        for(short r=52;r<64;r++)
        {

            if(Enables[r]!=NULL)
            {
                disconnect(Enables[r],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
                Enables[r]->setChecked(false);
                Enables[r]->setEnabled(false);
                connect(Enables[r],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
            }

        }
    else for(short r=52;r<64;r++)
    {

        if(Enables[r]!=NULL)
        {
            disconnect(Enables[r],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
            Enables[r]->setChecked(true);
            Enables[r]->setEnabled(true);
            connect(Enables[r],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
        }

    }



    for(short i=0;i<64;i++)
        for(short j=0;j<12;j++)
        {
            if(Blocks[i][j]==NULL)break;
            if(!Data.canUseBlock(i,j))Blocks[i][j]->setEnabled(false);
            else Blocks[i][j]->setEnabled(true);
        }

    for(short i=0;i<=maxIndex;i++)
    {
        if(Blocks[i][0]!=NULL&&Blocks[i][1]==NULL)
        {
            Blocks[i][0]->setChecked(true);
            Blocks[i][0]->setEnabled(false);
        }
    }
allowUpdateToCustom(true);
return;
}

void MainWindow::IniNeedGlass()
{
    for(short i=0;i<64;i++)
        for(short j=0;j<12;j++)
            NeedGlass[i][j]=false;

    NeedGlass[6][4]=true;//Lantern
    NeedGlass[6][2]=true;//IronPlate
    NeedGlass[30][1]=true;//GoldPlate

    return;
}

void MainWindow::InidoGlow()
{
    for(short i=0;i<64;i++)
        for(short j=0;j<12;j++)
            doGlow[i][j]=false;
    doGlow[2][2]=true;//glowstone
    doGlow[6][4]=true;//lantern
    doGlow[14][2]=true;//sea_lantern
    doGlow[28][6]=true;//shroomlight
    doGlow[34][2]=true;//campfire
    doGlow[35][2]=true;//magma
    return;
}

void MainWindow::IniEnables()
{
    for(short i=0;i<64;i++)
        if(Enables[i]!=NULL)
        Enables[i]->setChecked(true);

    return;
}

void MainWindow::connectEnables()
{
    for(short i=0;i<64;i++)
        if(Enables[i]!=NULL)
        connect(Enables[i],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
}

void MainWindow::allowUpdateToCustom(bool allowAutoUpdate)
{
    qDebug("调用了allowUpdateToCustom");
    if(allowAutoUpdate)
       {
        for(short i=0;i<64;i++)
            for(short j=0;j<12;j++)
            {if(Blocks[i][j]!=NULL)
                    connect(Blocks[i][j],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
              }

        }
       else {
        for(short i=0;i<64;i++)
            for(short j=0;j<12;j++)
            {if(Blocks[i][j]!=NULL)
                    disconnect(Blocks[i][j],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
              }
    }
    return;
}


void MainWindow::IniBL()
{
    ui->isBLSurvivalBetter->setChecked(true);

    //qDebug("IniBL运行到一半");

    applyPre(BLBetter);
}


void MainWindow::ChangeToCustom()
{
    ui->isBLCustom->setChecked(true);
    versionCheck();
    for(short base=0;base<64;base++)
        if(Blocks[base][1]==NULL&&Blocks[base][0]!=NULL)
        {
            disconnect(Blocks[base][0],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
            Blocks[base][0]->setChecked(true);
            //Blocks[base][0]->setEnabled(false);
            connect(Blocks[base][0],SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
        }
    return;
}

void MainWindow::applyPre(short*BL)
{
    //qDebug("调用了applyPre");
    allowUpdateToCustom(false);//prevent radiobuttons from switching BlockList mode to Custom

    //qDebug("成功禁用更改预设");

    grabGameVersion();
    short maxIndex=(Data.is16()?58:51);
    if(Data.is17())maxIndex=60;
    for(short i=0;i<=maxIndex;i++)
        if(Blocks[i][BL[i]]==NULL)continue;
        else
        {
            if(Data.canUseBlock(i,BL[i]))
                Blocks[i][BL[i]]->setChecked(true);
            else
                if(Data.canUseBlock(i,0))
                    Blocks[i][0]->setChecked(true);
            else
                    for(short c=0;c<12;c++)
                        if(Data.canUseBlock(i,c))
                        {Blocks[i][c]->setChecked(true);break;}
            else
                            Blocks[i][c]->setChecked(false);

        }

    //qDebug("修改了方块列表");

    if(Data.isSurvival())ui->Enable12->setChecked(false);

    allowUpdateToCustom(true);

    qDebug("恢复了更改预设");

    return;
}

void MainWindow::applyVanilla()
{
    applyPre(BLCreative);
    //ui->isBLCreative->setChecked(true);
    return;
}

void MainWindow::applyCheap()
{
    applyPre(BLCheaper);
    //ui->isBLSurvivalCheaper->setChecked(true);
    if(!Data.isCreative()&&Data.gameVersion<=12)
    {
    disconnect(ui->Enable03,SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
    ui->Enable03->setChecked(false);
    connect(ui->Enable03,SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
    }
    return;
}

void MainWindow::applyElegant()
{
    applyPre(BLBetter);
    //ui->isBLSurvivalBetter->setChecked(true);
    if(!Data.isCreative()&&Data.gameVersion<=12)
    {
    disconnect(ui->Enable03,SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
    ui->Enable03->setChecked(false);
    connect(ui->Enable03,SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
    }
    return;
}

void MainWindow::applyShiny()
{
    applyPre(BLGlowing);
    //ui->isBLGlowing->setChecked(true);
    if(!Data.isCreative()&&Data.gameVersion<=12)
    {
    disconnect(ui->Enable03,SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
    ui->Enable03->setChecked(false);
    connect(ui->Enable03,SIGNAL(clicked()),this,SLOT(ChangeToCustom()));
    }
    return;
}


void MainWindow::on_confirmBL_clicked()
{
    grabGameVersion();
    //grab MIndex
    getMIndex();

    qDebug("成功从界面读入了MIndex");

    //grab BlockList
    getBlockList();

    qDebug("成功从界面读入了BlockList");

    //以后会在这里加入一些功能，检查版本冲突
    Data.Allowed.ApplyAllowed(&Data.Basic,Data.colorAllowed);
    cout<<Data.Allowed.Map.transpose()<<endl;
    qDebug("成功刷新了颜色列表");

    ui->IntroColorCount->setText(tr("根据你的选项，地图画将可以使用")+QString::number(Data.Allowed._RGB.rows())+tr("种颜色"));
    ui->NextPage3->setEnabled(true);
    Data.step=4;
    updateEnables();
    qDebug("成功从界面读入了自定义方块列表的数据，并刷新了颜色列表，允许翻页调整图像");
}

void MainWindow::getMIndex()
{
    short Base,Depth,index;
    for(short mapColor=0;mapColor<256;mapColor++)
    {
        Base=mapColor/4;
        Depth=mapColor%4;
        index=Data.mapColor2Index(mapColor);
        Data.colorAllowed[index]=true;
        if(!Base)
        {
            Data.colorAllowed[index]=false;
            continue;
        }
        if(Enables[Base]==NULL)
        {
            Data.colorAllowed[index]=false;
            continue;
        }
        if(!Enables[Base]->isChecked())
        {
            Data.colorAllowed[index]=false;
            continue;
        }
        if(!Data.is16()&&Base>=52)
        {
            Data.colorAllowed[index]=false;
            continue;
        }
        if(!Data.is17()&&Base>=59)
        {
            Data.colorAllowed[index]=false;
            continue;
        }

        if(Depth==3&&!Data.isCreative())
        {
            Data.colorAllowed[index]=false;
            continue;
        }
        if(Data.isFlat()) {
            if(Base==12&&Depth!=2) {
                Data.colorAllowed[index]=false;
                continue;
            }
            if(Base!=12&&Depth!=1) {
                Data.colorAllowed[index]=false;
                continue;
            }
        }

        /*if(Enables[Base]==NULL)
        {Data.colorAllowed[Data.mapColor2Index(mapColor)]=false;
            continue;}
        if(Base==0)
        {
            Data.colorAllowed[Data.mapColor2Index(mapColor)]=false;
            continue;
        }
        if(Base==12)
        {
            Data.colorAllowed[Data.mapColor2Index(mapColor)]=(Data.isCreative()||(Depth!=3))&&(!Data.isFlat()||(Depth==0))&&Enables[Base]->isChecked();
            continue;
        }
            switch(Depth)
            {
            case 0:
                Data.colorAllowed[Data.mapColor2Index(mapColor)]=(!Data.isFlat())&&Enables[Base]->isChecked();
                Data.colorAllowed[Data.mapColor2Index(mapColor)]=Data.colorAllowed[Data.mapColor2Index(mapColor)]&&((Base<=51)||(Data.is16()));
                break;
            case 1:
                Data.colorAllowed[Data.mapColor2Index(mapColor)]=Enables[Base]->isChecked();
                Data.colorAllowed[Data.mapColor2Index(mapColor)]=Data.colorAllowed[Data.mapColor2Index(mapColor)]&&((Base<=51)||(Data.is16()));
                break;
            case 2:
                Data.colorAllowed[Data.mapColor2Index(mapColor)]=(!Data.isFlat())&&Enables[Base]->isChecked();
                Data.colorAllowed[Data.mapColor2Index(mapColor)]=Data.colorAllowed[Data.mapColor2Index(mapColor)]&&((Base<=51)||(Data.is16()));
                break;
            case 3:
                Data.colorAllowed[Data.mapColor2Index(mapColor)]=(Data.isCreative())&&Enables[Base]->isChecked();
                Data.colorAllowed[Data.mapColor2Index(mapColor)]=Data.colorAllowed[Data.mapColor2Index(mapColor)]&&((Base<=51)||(Data.is16()));
                break;
            default:
                qDebug("出现了错误的Depth");
                Data.colorAllowed[Data.mapColor2Index(mapColor)]=false;
                break;
            }*/
    }
    //Data.colorAllowed[0]=true;
    return;
}

void MainWindow::getBlockList()
{
    for(short i=0;i<64;i++)
        for(short j=0;j<12;j++)
        {
            if(Blocks[i][j]!=NULL&&Blocks[i][j]->isChecked())
            {
                Data.SelectedBlockList[i]=j;
                Data.BlockListId[i]=Data.BlockId[i][j];
                if(Data.gameVersion<=12&&Data.BlockIdfor12[i][j]!="[]")
                    Data.BlockListId[i]=Data.BlockIdfor12[i][j];
                break;
            }
            if(j>=11&&Blocks[i][j]==NULL)
            {
                Data.SelectedBlockList[i]=0;
                Data.BlockListId[i]="DefaultBlockId";
                /*if(i>=61)
                qDebug("出现被设为DefaultBlockId的方块");*/
            }

        }
    return;
}

void MainWindow::showColorColors()
{
    short R=0,G=0,B=0;
    QPalette pe;
    for(short base=1;base<64;base++)
    {
        if(ShowColors[base]==NULL)continue;

            R=roundf(255.0f*Data.Basic._RGB(128+base,0));
            G=roundf(255.0f*Data.Basic._RGB(128+base,1));
            B=roundf(255.0f*Data.Basic._RGB(128+base,2));
            if(R>255)
            {
                R=255;
                qDebug("出现R>255的情况");
            }
            if(G>255)
            {
                G=255;
                qDebug("出现G>255的情况");
            }
            if(B>255)
            {
                B=255;
                qDebug("出现B>255的情况");
            }
            pe.setColor(QPalette::Window,QColor(R,G,B));
            pe.setColor(QPalette::WindowText,QColor(255-R,255-G,255-B));

            ShowColors[base]->setPalette(pe);
            ShowColors[base]->setAutoFillBackground(true);
    }
}

void MainWindow::on_FirstConcrete_clicked()
{
    short canChange[16]={8,15,16,17,18,19,20,21,22,23,24,25,25,27,28,29};
    for(short i=0;i<16;i++)
        if(Blocks[canChange[i]][0]!=NULL)
        Blocks[canChange[i]][0]->setChecked(true);
    qDebug("调用了FirstConcrete");
}

void MainWindow::on_FirstWool_clicked()
{
    short canChange[16]={8,15,16,17,18,19,20,21,22,23,24,25,25,27,28,29};
    for(short i=0;i<16;i++)
        if(Blocks[canChange[i]][1]!=NULL)
        Blocks[canChange[i]][1]->setChecked(true);
    qDebug("调用了FirstWool");
}

void MainWindow::on_FirstStainedGlass_clicked()
{
    short canChange[16]={8,15,16,17,18,19,20,21,22,23,24,25,25,27,28,29};
    for(short i=0;i<16;i++)
        if(Blocks[canChange[i]][2]!=NULL)
        Blocks[canChange[i]][2]->setChecked(true);
    qDebug("调用了FirstStainedGlass");
}

#endif
