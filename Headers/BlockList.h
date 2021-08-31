#ifndef BLOCKLIST_H
#define BLOCKLIST_H
#include "TokiBaseColor.h"
#include <QObject>

class BlockList : public QObject
{
    Q_OBJECT
public:
    explicit BlockList(QObject *parent = nullptr);
    TokiBaseColor * operator() (int);
    TokiBlock * operator() (int,int);
    void addTokiBaseColor(QRgb,const QString & );
signals:

public slots:
    void retranslate(bool isEng);
private:
    std::vector<TokiBaseColor*> self;
};

#endif // BLOCKLIST_H
