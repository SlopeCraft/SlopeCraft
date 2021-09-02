#ifndef WATERITEM_H
#define WATERITEM_H

typedef unsigned int TokiPos;
typedef unsigned int waterItem;
extern const TokiPos nullPos;
extern const waterItem nullWater;
extern const short WaterColumnSize[3];
TokiPos TokiRC(short row,short col);//前两字节存储row，后两字节存储col
short TokiRow(TokiPos);
short TokiCol(TokiPos);

waterItem TokiWater(short high,short low);//前两字节存储high，后两字节存储low
short waterHigh(waterItem);
short waterLow(waterItem);

#endif // WATERITEM_H
