#include "WaterItem.h"

const TokiPos nullPos=TokiRC(-1,-1);
const waterItem nullWater=TokiRC(-32768,-32768);
const short WaterColumnSize[3]={11,6,1};
TokiPos TokiRC(int row,int col)
{
    /*unsigned int u;
    *((short*)&u)=row;
    *(((short*)&u)+1)=col;
    return u;*/
    return (row<<16)|(col&0x0000FFFF);
}
short TokiRow(TokiPos pos)
{
    return pos>>16;
}
short TokiCol(TokiPos pos)
{
    return pos&0x0000FFFF;
}
waterItem TokiWater(short high,short low)
{
    unsigned int u;
    *((short*)&u)=high;
    *(((short*)&u)+1)=low;
    return u;
}
short waterHigh(waterItem item)
{
    return *((short*)&item);
}
short waterLow(waterItem item)
{
    return *(((short*)&item)+1);
}
