/*
 Copyright © 2021-2022  TokiNoBug
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

#include "BatchUi.h"
#include "ui_BatchUi.h"

BatchUi::BatchUi(BatchUi ** _self,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BatchUi)
{
    ui->setupUi(this);
    self=_self;
    this->setAttribute(Qt::WA_QuitOnClose,false);

}

void BatchUi::closeEvent(QCloseEvent * event) {
    QMainWindow::closeEvent(event);
    delete this;
}

BatchUi::~BatchUi()
{
    *self=nullptr;
    delete ui;
}

void BatchUi::setTasks(const QStringList & fileNames,
                                       bool exportLite,bool exportData) {

    on_BtnClearTask_clicked();
    taskBoxes.reserve(fileNames.size());

    for(ushort idx=0;idx<fileNames.size();idx++) {
        TaskBox * box = new TaskBox(this);
        ui->scrollLayout->addWidget(box);
        taskBoxes.emplace_back(box);

        TokiTask & curTask=taskBoxes.back()->task;
        curTask.src_imageName=fileNames[idx];
        curTask.dst_liteFileName=(exportLite?tr("请设置目标文件名"):"");
        curTask.dst_DataFileName=(exportData?tr("请设置目标文件夹名"):"");
        curTask.dst_beginSeqNum=0;

        box->updateTask();

        connect(box,&TaskBox::erase,this,&BatchUi::erased);
        connect(box,&TaskBox::seqNumChanged,this,&BatchUi::onBoxSeqNumChanged);
        connect(this,&BatchUi::taskTypeUpdated,box,&TaskBox::onTaskTypeUpdated);

    }
    qDebug("setTasks完毕");
    taskBoxes.front()->ui->setMapBegSeq->setValue(1);
    taskBoxes.front()->ui->setMapBegSeq->setValue(0);

    dispTasks();

}

auto BatchUi::ptr2It(TaskBox * widgetPtr) const {
    auto target=taskBoxes.begin();
    for(;target!=taskBoxes.end();++target) {
        if(*target==widgetPtr) {
            return target;
        }
    }
    return taskBoxes.end();
}

ushort BatchUi::ptr2Index(TaskBox * widgetPtr) const {
    for(ushort idx=0;idx<taskBoxes.size();idx++) {
        if(taskBoxes[idx]==widgetPtr)
            return idx;
    }
    return 0;
}

void BatchUi::erased(TaskBox* widgetPtr) {
    ui->scrollLayout->removeWidget(widgetPtr);

    auto temp=ptr2It(widgetPtr);
    taskBoxes.erase(temp);
    widgetPtr->deleteLater();

    dispTasks();
}

void BatchUi::on_BtnAddTask_clicked() {
    QStringList newTasks=QFileDialog::getOpenFileNames(this,
                                                                     tr("选择图片"),
                                                                     "./",
                                                                     tr("图片(*.png *.bmp *.jpg *.tif *.GIF )"));

}

void BatchUi::on_BtnClearTask_clicked() {
    for(const auto & it : taskBoxes) {
        ui->scrollLayout->removeWidget(it);
        it->deleteLater();
    }
    taskBoxes.clear();
}

void BatchUi::onBoxSeqNumChanged(TaskBox * widgetPtr) {
    ushort curIdx=ptr2Index(widgetPtr);
    //qDebug()<<"curIdx="<<curIdx;
    if(ui->autoMapSeqNum->isChecked()) {
        allocateMapSeqNum(curIdx);
    }
}

void BatchUi::on_BtnExecute_clicked() {

}

void BatchUi::allocateMapSeqNum(ushort curIdx) {
    TaskBox * cur=taskBoxes[curIdx];
    if(taskBoxes.size()>unsigned(curIdx+1))
    {
        int curEnd=cur->task.dst_beginSeqNum+cur->task.mapCount()-1;

        int allowcatedNextBeg=curEnd+1;
        taskBoxes[curIdx+1]->ui->setMapBegSeq->setValue(allowcatedNextBeg);
    }

    if(curIdx>0) {
        TaskBox * prev=taskBoxes[curIdx-1];
        int prevEnd=prev->task.dst_beginSeqNum+prev->task.mapCount()-1;

        int curBeg=cur->task.dst_beginSeqNum;

        int minCurBeg=prevEnd+1;

        if(curBeg<minCurBeg){
            cur->ui->setMapBegSeq->setValue(minCurBeg);
        }
    }
}

void BatchUi::dispTasks() const {
    qDebug()<<"task count : "<<taskBoxes.size();
    for(const auto & it : taskBoxes) {
        qDebug()<<it->task.src_imageName;
    }
}
