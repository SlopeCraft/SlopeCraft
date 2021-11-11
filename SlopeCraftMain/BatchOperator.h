#ifndef BATCHOPERATOR_H
#define BATCHOPERATOR_H
#include <QObject>
#include <QString>
#include <vector>

#include "BatchUi.h"
#include "TokiTask.h"

class BatchOperator : public QObject
{
    Q_OBJECT
public:
    explicit BatchOperator(QObject *parent = nullptr);

private:
    std::vector<TokiTask> tasks;

    BatchUi * batchWind;

signals:

};

#endif // BATCHOPERATOR_H
