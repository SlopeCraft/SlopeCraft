#ifndef BATCHOPERATOR_H
#define BATCHOPERATOR_H
#include <QObject>
#include <QString>
#include <vector>
#include "WaterItem.h"

class BatchOperator;

class TokiTask
{
public:
    TokiTask();
    ~TokiTask();
    friend class BatchOperator;
    uint mapCount() const;
private:
    QString src_imageName;
    TokiPos src_imageSize;
    QString dst_liteFileName;
    QString dst_DataFileName;
    uint dst_beginSeqNum;

};

class BatchOperator : public QObject
{
    Q_OBJECT
public:
    explicit BatchOperator(QObject *parent = nullptr);


signals:

};

#endif // BATCHOPERATOR_H
