#ifndef TOKIBASECOLOR_H
#define TOKIBASECOLOR_H

#include <QObject>
#include <QLabel>
#include <QGridLayout>
#include <QSpacerItem>
#include <QCheckBox>
#include <vector>
#include <cmath>
#include "TokiBlock.h"

class BlockListManager;

class TokiBaseColor : public QObject
{
    Q_OBJECT
    friend class BlockListManager;
public:
    explicit TokiBaseColor(uchar,
                           QGridLayout*,
                           QObject *parent = nullptr);
    ~TokiBaseColor();

const TokiBlock* getTokiBlock() const;

bool getEnabled() const;

ushort getSelected() const;

void addTokiBlock(const QJsonObject & json,
                  const QString & imgDir);
void makeLabel(QRgb);

static uchar mcVer;

signals:
    void translate(Language);
    void userClicked();

private:
    uchar baseColor;
    bool isEnabled;
    ushort selected;
    QGridLayout * layout;
    QCheckBox * checkBox;
    std::vector<TokiBlock*> tbs;
    bool isAllOverVersion() const;
    void setSelected(ushort);
private slots:
    void receiveClicked(ushort);
    void updateEnabled(bool);
    void versionCheck();

};

#endif // TOKIBASECOLOR_H
