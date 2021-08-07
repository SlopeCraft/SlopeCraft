#include "WaterItem.h"

const TokiPos nullPos=TokiRC(-1,-1);
const waterItem nullWater=TokiRC(-32768,-32768);

TokiPos TokiRC(short row,short col)
{
    unsigned int u;
    *((short*)&u)=row;
    *(((short*)&u)+1)=col;
    return u;
}
short TokiRow(TokiPos pos)
{
    return *((short*)&pos);
}
short TokiCol(TokiPos pos)
{
    return *(((short*)&pos)+1);
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
