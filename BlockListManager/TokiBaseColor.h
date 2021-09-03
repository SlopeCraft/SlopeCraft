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
class TokiBaseColor : public QObject
{
    Q_OBJECT
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

private:
    uchar baseColor;
    bool isEnabled;
    ushort selected;
    QGridLayout * layout;
    QCheckBox * checkBox;
    std::vector<TokiBlock*> tbs;
    bool isAllOverVersion() const;
private slots:
    void receiveClicked(ushort);
    void updateEnabled(bool);
    void versionCheck();

};

#endif // TOKIBASECOLOR_H
