#include "WaterItem.h"

TokiPos TokiRC(short row,short col)
{
    unsigned int u;
    *(short*)&u=row;
    *(((short*)&u)+1)=col;
    return u;
}
inline short TokiRow(TokiPos pos)
{
    return *((short*)&pos);
}
inline short TokiCol(TokiPos pos)
{
    return *(((short*)&pos)+1);
}
waterItem TokiWater(short high,short low)
{
    unsigned int u;
    *(short*)&u=high;
    *(((short*)&u)+1)=low;
    return u;
}
inline short waterHigh(waterItem item)
{
    return *((short*)&item);
}
inline short waterLow(waterItem item)
{
    return *(((short*)&item)+1);
}
