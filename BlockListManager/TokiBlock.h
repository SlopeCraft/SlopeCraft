#ifndef TOKIBLOCK_H
#define TOKIBLOCK_H

#include <QObject>
#include <QRadioButton>
#include <QString>
#include <QJsonObject>
#include <QFile>
#include <QMessageBox>
#include "simpleBlock.h"

enum Language {
    ZH,EN
};

class TokiBaseColor;

class TokiBlock : public QObject
{
    Q_OBJECT
    friend class TokiBaseColor;
public:
    explicit TokiBlock(QRadioButton * _target,
                       const QJsonObject & json,
                       const QString & imgDir,
                       ushort _self,
                       QObject *parent = nullptr);
    ~TokiBlock();
    const QRadioButton * getTarget() const;
    const simpleBlock *getSimpleBlock() const;
signals:
    void radioBtnClicked(ushort);
private:
    ushort self;//指明自己是所属基色的第i个方块
    QRadioButton * target;
    QString nameZH;
    QString nameEN;
    simpleBlock block;
    QRadioButton * getNCTarget() const;
private slots:
    void translate(Language);
    void onTargetClicked(bool);

};

#endif // TOKIBLOCK_H
