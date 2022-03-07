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

#include "MainWindow.h"

BatchUi::BatchUi(BatchUi ** _self,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BatchUi)
{
    ui->setupUi(this);
    self=_self;
    this->setAttribute(Qt::WA_QuitOnClose,false);

    connect(ui->setTypeData,&QRadioButton::clicked,
            this,&BatchUi::onTaskTypeChanged);
    connect(ui->setTypeLite,&QRadioButton::clicked,
            this,&BatchUi::onTaskTypeChanged);
    connect(ui->setTypeStructure,&QRadioButton::clicked,
            this,&BatchUi::onTaskTypeChanged);
    connect(ui->setDataFolder,&QLineEdit::textEdited,
            ui->setDataFolder,&QLineEdit::textChanged);
    connect(ui->setDataFolder,&QLineEdit::textChanged,
            this,&BatchUi::checkExecutable);
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

void BatchUi::setTasks(const QStringList & fileNames) {

    taskBoxes.reserve(fileNames.size());

    for(ushort idx=0;idx<fileNames.size();idx++) {
        TaskBox * box = new TaskBox(this);
        ui->scrollLayout->addWidget(box);
        taskBoxes.emplace_back(box);
        box->setTask(fileNames[idx]);
        box->setMapBegSeqReadOnly(true);

        connect(box,&TaskBox::erase,this,&BatchUi::erased);
        connect(box,&TaskBox::seqNumChanged,this,&BatchUi::onBoxSeqNumChanged);

    }
    qDebug("setTasks完毕");
    taskBoxes.front()->setMapBegSeq(0);
    taskBoxes.front()->setMapBegSeqReadOnly(false);
    onBoxSeqNumChanged();

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
    for(auto i : taskBoxes) {
        i->setMapBegSeqReadOnly(true);
    }
    taskBoxes.front()->setMapBegSeqReadOnly(false);
    onBoxSeqNumChanged();
}

void BatchUi::onBoxSeqNumChanged() {
    uint32_t curBeg=taskBoxes.front()->begSeqNum();
    taskBoxes.front()->setMapBegSeq(curBeg);
    for(size_t idx=1;idx<taskBoxes.size();idx++) {
        taskBoxes[idx]->setMapBegSeq(curBeg+taskBoxes[idx-1]->mapSize());
        curBeg+=taskBoxes[idx-1]->mapSize();
    }
}

void BatchUi::on_BtnExecute_clicked() {

}

void BatchUi::onTaskTypeChanged() {
    qDebug("void BatchUi::onTaskTypeChanged");
    if(ui->setTypeData->isChecked())
        TaskBox::taskType=TaskType::Data;
    if(ui->setTypeLite->isChecked())
        TaskBox::taskType=TaskType::Litematica;
    if(ui->setTypeStructure->isChecked())
        TaskBox::taskType=TaskType::Structure;

    ui->setDataFolder->setEnabled(TaskBox::taskType==TaskType::Data);
    ui->BtnBrowseDataFolder->setEnabled(TaskBox::taskType==TaskType::Data);
    ui->labelDenoteDstFolder->setEnabled(TaskBox::taskType==TaskType::Data);

    for(auto i : taskBoxes) {
        i->updateTaskType();
    }

    checkExecutable();
}

void BatchUi::on_BtnBrowseDataFolder_clicked() {
    QString path=QFileDialog::getExistingDirectory(this,
                                                   tr("选择输出文件夹"),
                                                   ui->setDataFolder->text());
    if(path.isEmpty())
        return;
    path.replace("\\\\","/");
    path.replace('\\','/');
    ui->setDataFolder->setText(path);
}

void BatchUi::checkExecutable() {
    bool executable=true;
    ui->LabelShowInfo->setText("");
    if(TaskBox::taskType==TaskType::Data) {
        QString dstFolder=ui->setDataFolder->text();
        if(!QDir(dstFolder).exists()) {
            ui->LabelShowInfo->setText(tr("目标文件夹不可用"));
            executable=false;
        }


    }
    else {
        if(!qobject_cast<MainWindow * >(parent())->kernelPtr()->isVanilla()) {
            ui->LabelShowInfo->setText(tr("主窗体中选择了纯文件地图画，冲突"));
            executable=false;
        }
    }
    ui->BtnExecute->setEnabled(executable);
}

