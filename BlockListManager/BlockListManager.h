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

#ifndef BLOCKLISTMANAGER_H
#define BLOCKLISTMANAGER_H

#include <queue>

#include <QObject>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include "TokiBaseColor.h"
class BlockListManager : public QObject
{
    Q_OBJECT
public:
    explicit BlockListManager(QHBoxLayout * _area,QObject *parent = nullptr);

    ~BlockListManager();

    void addBlocks(const QJsonArray & ,QString imgDir);

    void applyPreset(const ushort * );

    void setSelected(uchar baseColor,ushort blockSeq);

    void setEnabled(uchar baseColor,bool isEnable);

    void setLabelColors(const QRgb*);

    void setVersion(uchar);

    void getEnableList(bool*) const;
    void getSimpleBlockList(simpleBlock*) const;
    std::vector<simpleBlock> getSimpleBlockList() const;
    std::vector<const TokiBlock * >getTokiBlockList() const;
    std::vector<const QRadioButton * >getQRadioButtonList() const;
    std::vector<ushort> toPreset() const;
public slots:

signals:
    void translate(Language);
    void switchToCustom() const;
    void blockListChanged() const;

private:
    bool isApplyingPreset;
    QHBoxLayout * area;
    std::vector<TokiBaseColor*> tbcs;
    static const QString baseColorNames[64];

private slots:
    void receiveClicked() const;


};

bool isValidBlockInfo(const QJsonObject &);

#endif // BLOCKLISTMANAGER_H
