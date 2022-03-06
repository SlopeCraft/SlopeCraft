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

#ifndef TASKBOX_H
#define TASKBOX_H

#include <QWidget>
#include <QFileDialog>
#include "ui_TaskBox.h"

namespace Ui{
class TaskBox;
}

class BatchUi;

enum TaskType{
    Litematica,
    Structure,
    Data
};

class TaskBox : public QWidget
{
    Q_OBJECT
    //friend class BatchUi;
public:
    explicit TaskBox(QWidget *parent = nullptr);
    ~TaskBox();

    inline uint32_t mapSize() const {
        return mapRows*mapCols;
    }

    void setTask(const QString & rawImg) {
        QImage img;
        uint32_t prevMapSize=mapSize();
        if(!img.load(rawImg)) {
            mapRows=0;
            mapCols=0;
            ui->preView->setPixmap(QPixmap());
            ui->preView->setText(tr("图片格式损坏"));
        }
        mapRows=std::ceil(img.height()/128.0);
        mapCols=std::ceil(img.width()/128.0);
        ui->preView->setPixmap(QPixmap::fromImage(img));
        ui->preView->setText("");
        ui->imageName->setText(rawImg);
        ui->liteName->setText(rawImgPath()+liteSuffix());

        if(prevMapSize!=mapSize()&&taskType==Data) {
            emit seqNumChanged(this);
        }
    }


    inline QString rawImgPath() const {
        return ui->imageName->text();
    }

    inline QString liteName() const {
        return ui->liteName->text();
    }

    inline uint32_t begSeqNum() const {
        return ui->setMapBegSeq->value();
    }
    static TaskType taskType;

    inline static QString liteSuffix() {
        switch (taskType) {
        case Litematica:
            return ".litematic";
        case Structure:
            return ".nbt";
        default:
            return ".ERROR_TASK_TYPE";
        }
    }

    inline void setMapBegSeqReadOnly(bool mbsro) {
        ui->setMapBegSeq->setReadOnly(mbsro);
    }

    inline void setMapBegSeqMinVal(uint32_t mbsmv) {
        ui->setMapBegSeq->setMaximum(mbsmv);
    }

    inline void setMapBegSeq(uint32_t mbs) {
        ui->setMapBegSeq->setValue(mbs);
        ui->setMapBegSeq->setSuffix(" ~ "+QString::number(mbs+mapSize()-1));
    }

signals:
    void erase(TaskBox *);
    void seqNumChanged(TaskBox *);

private slots:
    void on_imageName_editingFinished() {
        setTask(ui->imageName->text());
    }

    void on_setMapBegSeq_valueChanged(int) {
        if(!ui->setMapBegSeq->isReadOnly()) {
            emit seqNumChanged(this);
        }
    }

    void on_BtnErase_clicked();

private:
    Ui::TaskBox *ui;
    uint16_t mapRows;
    uint16_t mapCols;

};

#endif // TASKBOX_H
