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

#ifndef PREVIEWWIND_H
#define PREVIEWWIND_H

#include <QMainWindow>
#include <vector>
#include <QMainWindow>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <iostream>

namespace Ui {
class PreviewWind;
}

class PreviewWind : public QMainWindow
{
    Q_OBJECT

public:
    explicit PreviewWind(QWidget *parent = nullptr);

    std::vector<const QRadioButton *> Src;
    std::vector<int>BlockCount;
    std::vector<QLabel*>CountLabel;
    const QRadioButton*Water;
    int size[3];
    int TotalBlockCount;
    void ShowMaterialList();
    ~PreviewWind();

private slots:
    void on_SwitchUnit_clicked(bool checked);

private:
    Ui::PreviewWind *ui;
};

QString blockCount2string(int,int=64);
#endif // PREVIEWWIND_H
