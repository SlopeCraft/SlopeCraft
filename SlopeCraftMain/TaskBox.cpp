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

#include "TaskBox.h"

TaskType TaskBox::taskType=TaskType::Litematica;

TaskBox::TaskBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TaskBox)
{
    ui->setupUi(this);
    mapCols=0;
    mapRows=0;
}

TaskBox::~TaskBox()
{
    delete ui;
}

