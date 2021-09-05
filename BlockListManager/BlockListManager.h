#ifndef BLOCKLISTMANAGER_H
#define BLOCKLISTMANAGER_H

#include <queue>

#include <QObject>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include "TokiBaseColor.h"
class BlockListManager : public QObject
{
    Q_OBJECT
public:
    explicit BlockListManager(QHBoxLayout * _area,QObject *parent = nullptr);

    ~BlockListManager();

    void addBlocks(const QJsonArray & ,QString imgDir);

    void applyPreset(const ushort * );

    void setSelected(uchar baseColor,ushort blockSeq);

    void setEnabled(uchar baseColor,bool isEnable);

    void setLabelColors(const QRgb*);

    void setVersion(uchar);

    void getEnableList(bool*) const;
    void getSimpleBlockList(simpleBlock*) const;
    std::vector<simpleBlock> getSimpleBlockList() const;
    std::vector<const TokiBlock * >getTokiBlockList() const;
    std::vector<const QRadioButton * >getQRadioButtonList() const;
    std::vector<ushort> toPreset() const;
public slots:

signals:
    void translate(Language);
    void switchToCustom() const;

private:
    bool isApplyingPreset;
    QHBoxLayout * area;
    std::vector<TokiBaseColor*> tbcs;
    static const QString baseColorNames[64];

private slots:
    void receiveClicked() const;


};

bool isValidBlockInfo(const QJsonObject &);

#endif // BLOCKLISTMANAGER_H
