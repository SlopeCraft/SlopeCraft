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

signals:
    void start();
    void reportStatue(QString);
    void finish();

private:
    std::vector<TokiTask> tasks;

    BatchUi * batchWind;

};

#endif // BATCHOPERATOR_H
