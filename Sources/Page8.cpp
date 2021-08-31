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


#ifndef Page8_CPP
#define Page8_CPP

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include  <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QProcess>
void MainWindow::on_seeExported_clicked()
{
    if(Data.step<6)return;
    if(!ProductPath.empty()&&(Data.ExLitestep>=2||Data.ExMcFstep>=1))
    {
        //char a='\\';
        //char b='\"';
        for(auto it=ProductPath.begin();it!=ProductPath.end();it++)
            if(*it=='/') *it='\\';
        //Data.ProductPath.replace("/","\\");
        QProcess pro;
        //Data.ProductPath=Data.ProductPath.left(Data.ProductPath.lastIndexOf('\\'));
        ProductPath=ProductPath.substr(0,ProductPath.find_last_of('\\'));
        qDebug().noquote()<<QString::fromStdString(ProductPath);
        string cmd="explorer /select,\""+ProductPath+"\"";
        qDebug().noquote()<<QString::fromStdString(cmd);
        pro.startDetached(QString::fromStdString(cmd));
    }
}

void MainWindow::on_contact_clicked()
{
    contactB();
    contactG();
}


#endif
