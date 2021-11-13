#ifndef TOKITASK_H
#define TOKITASK_H
#include <QString>
#include <cmath>
#include "WaterItem.h"

class TokiTask
{
public:
    TokiTask();
    ~TokiTask();
    //friend class BatchOperator;
    uint mapCount() const;
    ushort rows() const;
    ushort cols() const;
    void clear();

    QString src_imageName;
    TokiPos src_imageSize;
    QString dst_liteFileName;
    QString dst_DataFileName;
    uint dst_beginSeqNum;

    static bool canExportLite;
    static const bool canExportData;

};
#endif // TOKITASK_H
