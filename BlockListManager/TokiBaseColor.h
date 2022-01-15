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

#ifndef TOKIBASECOLOR_H
#define TOKIBASECOLOR_H

#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QSpacerItem>
#include <QCheckBox>
#include <vector>
#include <cmath>
#include "TokiBlock.h"

class BlockListManager;

class TokiBaseColor : public QObject
{
    Q_OBJECT
    friend class BlockListManager;
public:
    explicit TokiBaseColor(uchar,
                           QGridLayout*,
                           QObject *parent = nullptr);
    ~TokiBaseColor();

const TokiBlock* getTokiBlock() const;

bool getEnabled() const;

ushort getSelected() const;

void addTokiBlock(const QJsonObject & json,
                  const QString & imgDir);
void makeLabel(QRgb);

void getTokiBlockList(std::vector<const TokiBlock*> & ) const;

static uchar mcVer;

signals:
    void userClicked();
    void translate(Language);

private:
    uchar baseColor;
    bool isEnabled;
    ushort selected;
    QGridLayout * layout;
    QCheckBox * checkBox;
    std::vector<TokiBlock*> tbs;
    bool isAllOverVersion() const;
    void setSelected(ushort);
private slots:
    void receiveClicked(ushort);
    void updateEnabled(bool);
    void versionCheck();
    void translateCheckBox(Language);
};

#endif // TOKIBASECOLOR_H
