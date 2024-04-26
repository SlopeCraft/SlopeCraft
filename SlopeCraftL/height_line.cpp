/*
 Copyright © 2021-2023  TokiNoBug
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
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#include "height_line.h"

const ARGB height_line::BlockColor = ARGB32(0, 0, 0);
const ARGB height_line::AirColor = ARGB32(255, 255, 255);
const ARGB height_line::WaterColor = ARGB32(0, 64, 255);
const ARGB height_line::greyColor = ARGB32(192, 192, 192);

height_line::height_line() {}

float height_line::make(const TokiColor **src,
                        const Eigen::Array<uint8_t, Eigen::Dynamic, 1> &g,
                        bool allowNaturalCompress, Eigen::ArrayXi *dst) {
  float sumDiff = 0;
  Eigen::ArrayXi mapColorCol(g.rows());

  for (uint32_t r = 0; r < g.rows(); r++) {
    if (src[r] == nullptr) {
      std::cerr << "Fatal Error! nullptr found in src\n";
      return 0;
    }
    switch (g(r)) {
      case 0:
        mapColorCol(r) = src[r]->Result;
        sumDiff += src[r]->ResultDiff;
        break;
      case 1:
        mapColorCol(r) = src[r]->sideResult[0];
        sumDiff += src[r]->sideSelectivity[0];
        break;
      default:
        mapColorCol(r) = src[r]->sideResult[1];
        sumDiff += src[r]->sideSelectivity[1];
        break;
    }
  }

  if (dst != nullptr) *dst = mapColorCol;

  make(mapColorCol, allowNaturalCompress);
  return sumDiff;
}

void height_line::make(const Eigen::ArrayXi &mapColorCol,
                       bool allowNaturalCompress) {
  ///////////////////////1
  waterMap.clear();
  const uint32_t picRows = mapColorCol.rows();
  base.setConstant(1 + picRows, 11);
  HighLine.setZero(1 + picRows);
  LowLine.setZero(1 + picRows);
  // qDebug("初始化完毕");

  //////////////////////////////2
  // qDebug()<<"size(base.segment(1,picRows))=["<<base.segment(1,picRows).rows()<<','<<base.segment(1,picRows).cols()<<']';
  // qDebug()<<"size(mapColorCol)=["<<mapColorCol.rows()<<','<<mapColorCol.cols()<<']';
  base.segment(1, picRows) = mapColorCol / 4;
  Eigen::ArrayXi rawShadow = mapColorCol - 4 * (mapColorCol / 4);

  assert(!(rawShadow >= 3).any());
  //
  //  if () {
  // #warning "Fix this error handling"
  //    std::cerr << "Fatal Error: depth=3 in vanilla map!" << std::endl;
  //    std::cerr << "SlopeCraft will crash." << std::endl;
  //    exit(1);
  //    // delete &rawShadow;
  //    return;
  //  }
  Eigen::ArrayXi dealedDepth(picRows + 1);
  dealedDepth.setZero();
  dealedDepth.segment(1, picRows) = rawShadow - 1;

  if (base(1) == 12 || base(1) == 0 || rawShadow(0) == 2) {
    base(0) = 0;
    dealedDepth(1) = 0;
  }
  for (uint32_t r = 1; r < picRows; r++) {
    if (base(r + 1) == 0) {
      dealedDepth(r + 1) = 0;
      continue;
    }
    if (base(r + 1) == 12) {
      dealedDepth(r + 1) = 0;
      if (waterMap.contains(r + 1)) {
        waterMap.erase(r + 1);
      }
    }
  }
  ///////////////////////3
  for (uint32_t r = 0; r < picRows; r++) {
    // HighMap.row(r+1)=HighMap.row(r)+dealedDepth.row(r+1);
    HighLine(r + 1) = HighLine(r) + dealedDepth(r + 1);
  }
  //////////////////4
  LowLine = HighLine;
  for (auto it = waterMap.cbegin(); it != waterMap.cend(); it++) {
    /*
    LowMap(TokiRow(it->first),TokiCol(it->first))=
            HighMap(TokiRow(it->first),TokiCol(it->first))
            -WATER_COLUMN_SIZE[rawShadow(TokiRow(it->first)-1,TokiCol(it->first))]+1;
*/
    LowLine(it->first) =
        HighLine(it->first) - WATER_COLUMN_SIZE[rawShadow(it->first - 1)] + 1;
  }
  /////////////////5
  HighLine -= LowLine.minCoeff();
  LowLine -= LowLine.minCoeff();

  if (allowNaturalCompress) {
    optimize_chain OC(base, HighLine, LowLine);
    OC.divide_and_compress();
    HighLine = OC.high_line();
    LowLine = OC.low_line();
  }
  for (auto it = waterMap.begin(); it != waterMap.end(); it++) {
    waterMap[it->first] =
        water_y_range{HighLine(it->first), LowLine(it->first)};
    HighLine(it->first) += 1;
  }
}

uint32_t height_line::maxHeight() const {
  return HighLine.maxCoeff() - LowLine.minCoeff() + 1;
}

void height_line::updateWaterMap() {
  waterMap.clear();
  for (uint32_t r = 1; r < base.rows(); r++) {
    if (base(r) != 12) continue;
    waterMap[r] = water_y_range{HighLine(r) - 1, LowLine(r)};
  }
}

const std::map<uint32_t, water_y_range> &height_line::getWaterMap() const {
  return waterMap;
}

EImage height_line::toImg() const {
  const int rMax = maxHeight() - 1;
  EImage img(maxHeight(), HighLine.size());
  img.setConstant(AirColor);
  //  short y = 0, r = rMax - y;
  for (uint32_t x = 0; x < HighLine.size(); x++) {
    const int y = HighLine(x);
    int r = rMax - y;
    if (base(x)) {
      if (base(x) != 12) {
        img(r, x) = BlockColor;
      } else {
        img(r, x) = greyColor;
        short rmin = rMax - LowLine(x);
        r++;
        img.col(x).segment(r, rmin - r + 1) = WaterColor;
        if (rmin < rMax) {
          rmin++;
          img(rmin, x) = greyColor;
        }
      }
    }
  }
  return img;
}

/*
Base.setConstant(sizePic(0)+1,sizePic(1),11);
    WaterList.clear();
    HighMap.setZero(sizePic(0)+1,sizePic(1));
    LowMap.setZero(sizePic(0)+1,sizePic(1));

    Base.block(1,0,sizePic(0),sizePic(1))=mapPic/4;

    ArrayXXi dealedDepth;
    ArrayXXi rawShadow=mapPic-4*(mapPic/4);

    if((rawShadow>=3).any())
    {
        qDebug("错误：Depth中存在深度为3的方块");
        return;
    }
    dealedDepth.setZero(sizePic(0)+1,sizePic(1));
    dealedDepth.block(1,0,sizePic(0),sizePic(1))=rawShadow-1;
    //Depth的第一行没有意义，只是为了保持行索引一致

    for(short r=0;r<Base.rows();r++)
    {
        for(short c=0;c<Base.cols();c++)
        {
            if(Base(r,c)==12)
            {
                WaterList[TokiRC(r,c)]=nullWater;
                dealedDepth(r,c)=0;
                continue;
            }
            if(Base(r,c)==0)
            {
                dealedDepth(r,c)=0;
                continue;
            }
        }
        emit progressAdd(sizePic(0));
    }

    HighMap.setZero(sizePic(0)+1,sizePic(1));
    LowMap.setZero(sizePic(0)+1,sizePic(1));

    int waterCount=WaterList.size();
    qDebug()<<"共有"<<waterCount<<"个水柱";
    for(short r=0;r<sizePic(0);r++)//遍历每一行，根据高度差构建高度图
    {
        HighMap.row(r+1)=HighMap.row(r)+dealedDepth.row(r+1);
        emit progressAdd(sizePic(0));
    }

    for(short c=0;c<Base.cols();c++)
    {
        if(Base(1,c)==0||Base(1,c)==12||rawShadow(0,c)==2)
        {
            Base(0,c)=0;
            HighMap(0,c)=HighMap(1,c);
        }
        emit progressAdd(sizePic(1));
    }

    cerr<<"extra north side stones removed"<<endl;

    LowMap=HighMap;

    for(auto it=WaterList.begin();it!=WaterList.end();it++)
    {
        LowMap(TokiRow(it->first),TokiCol(it->first))=
                HighMap(TokiRow(it->first),TokiCol(it->first))
                -WATER_COLUMN_SIZE[rawShadow(TokiRow(it->first)-1,TokiCol(it->first))]+1;
    }

    cerr<<"LowMap updated"<<endl;

    for(short c=0;c<sizePic(1);c++)
    {
        HighMap.col(c)-=LowMap.col(c).minCoeff();
        LowMap.col(c)-=LowMap.col(c).minCoeff();
        //沉降每一列
        emit progressAdd(sizePic(1));
    }

    cerr<<"basic sink done"<<endl;

    if(compress_method==NaturalOnly)
    {
        //执行高度压缩
        optimize_chain::Base=Base;
        for(int c=0;c<sizePic(1);c++)
        {
            optimize_chain Compressor(HighMap.col(c),LowMap.col(c),c);
            Compressor.divide_and_compress();
            HighMap.col(c)=Compressor.HighLine;
            LowMap.col(c)=Compressor.LowLine;
            emit progressAdd(sizePic(1));
        }
    }

    cerr<<"waterList updated again"<<endl;

    int maxHeight=HighMap.maxCoeff();

    for(auto it=WaterList.begin();it!=WaterList.end();it++)
    {
        int r=TokiRow(it->first),c=TokiCol(it->first);
        it->second=TokiWater(HighMap(r,c),LowMap(r,c));
        maxHeight=max(maxHeight,HighMap(r,c)+1);
        //遮顶玻璃块
    }
    size3D[2]=2+sizePic(0);//z
    size3D[0]=2+sizePic(1);//x
    size3D[1]=1+maxHeight;//y
    return;
*/
