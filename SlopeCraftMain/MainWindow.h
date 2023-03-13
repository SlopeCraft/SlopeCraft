/*
 Copyright © 2021-2023  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// #define dispDerivative
// #define putDitheredImg
// #define putMapData
// #define putBlockList
#include "ui_mainwindow.h"

#include <QButtonGroup>
#include <QCloseEvent>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFuture>
#include <QHash>
#include <QImage>
#include <QKeyEvent>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressBar>
#include <QScrollArea>
#include <QScrollBar>
#include <QString>
#include <QTranslator>
#include <QUrl>

#include <QJsonArray>
#include <QJsonDocument>
// #include <QJsonError>
#include <QJsonObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <Eigen/Dense>

// #define SLOPECRAFTL_WITH_AICVETR
#include "SlopeCraftL.h"

#include "AiCvterParameterDialog.h"
#include "BatchUi.h"
#include "BlockListManager.h"
#include "VersionDialog.h"
#include "previewwind.h"
#include "tpstrategywind.h"

class MainWindow;

using std::cout, std::endl, std::cerr;

#include <QColor>
#include <QRgb>

// class ColorSet;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class tpStrategyWind;

#ifndef TPS__
#define TPS__
class tpS {
public:
  tpS(char _pTpS = 'B', char _hTpS = 'C', QRgb _BGC = qRgb(220, 220, 220)) {
    pTpS = _pTpS;
    hTpS = _hTpS;
    BGC = _BGC;
  }
  ~tpS();
  char pTpS;
  char hTpS;
  QRgb BGC;
};
#endif

class MainWindow : public QMainWindow {
  Q_OBJECT
  friend class BatchUi;

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  // 初始化方块列表用
  void InitializeAll();
  static void putSettings(const QJsonObject &jo);

  static const QString selfVersion;

signals:
  void mapTypeChanged();

  void closed();

protected:
  void closeEvent(QCloseEvent *) override;
  void keyPressEvent(QKeyEvent *) override;

public:
  inline void preProcess(char pureTpStrategy = 'B', char halfTpStrategy = 'C',
                         QRgb BGC = qRgb(220, 220, 220));

  inline auto kernelPtr() const { return kernel; }

public slots:
  void ReceiveTPS(tpS);
  // 透明像素处理策略：B->替换为背景色；A->空气；W->暂缓，等待处理
  // 半透明像素处理策略：B->替换为背景色；C->与背景色叠加；R->保留颜色；W->暂缓，等待处理

  void showPreview();
  // 语言槽
  void turnCh();
  void turnEn();

  void checkVersion();

  void setAutoCheckUpdate(bool);

  void grabVersion(bool isAuto = true);

private slots:

  void contactG();
  void contactB();
  // 翻页的自定义槽
  void turnToPage0();
  void turnToPage1();
  void turnToPage2();
  void turnToPage3();
  void turnToPage4();
  void turnToPage5();
  void turnToPage6();
  void turnToPage7();
  void turnToPage8();

  // for Page0
  void on_StartWithSlope_clicked();
  void on_StartWithFlat_clicked();
  void on_StartWithNotVanilla_clicked();

  // for Page1
  void on_ImportPic_clicked();
  void on_ImportSettings_clicked();

  // for Page2
  void onGameVerClicked();
  void onMapTypeClicked();

  // for Page3
  // 应用预设方块列表的自定义槽
  void ChangeToCustom();
  void onPresetsClicked();
  void onBlockListChanged();

  void onActionSavePreset();
  void onActionLoadPreset();

  void exportAvailableColors();

  // for Page4
  void onAlgoClicked();
  void on_Convert_clicked();
  void on_ShowRaw_clicked();
  void on_ShowAdjed_clicked();
  void on_ExData_clicked();
  void on_ExportFlatDiagram_clicked();
  // void on_ExFlatDiagram_clicked();

  void onActionAiCvterParameters();

  void on_ExLite_clicked();

  void on_ExStructure_clicked();

  void on_ExWESchem_clicked();

  // for Page5
  void on_Build4Lite_clicked();
  void on_ManualPreview_clicked();
  void on_ExportLite_clicked();
  void onExportLiteclicked(QString);
  void on_allowGlassBridge_stateChanged(int arg1);

  // for Page7
  void on_InputDataIndex_textChanged();
  void on_ExportData_clicked();
  void onExportDataclicked(QString);

  void on_seeExported_clicked();

  void on_AllowForcedOpti_stateChanged(int arg1);

  void on_reportBugs_clicked();

  // void on_StartWithWall_clicked();

  void on_ExImage_clicked();

  void on_FirstConcrete_clicked();

  void on_FirstWool_clicked();

  void on_FirstStainedGlass_clicked();

  void testBlockList();

private:
  Ui::MainWindow *ui;

  SlopeCraft::Kernel *const kernel;
  BlockListManager *Manager;

  QImage rawPic;

  // QTranslator translater;
  // bool Enabled[64];//被启动的方块列表，相当于最终的MIndex
  static const ushort BLCreative[64];
  static const ushort BLCheaper[64];
  static const ushort BLBetter[64];
  static const ushort BLGlowing[64];

  static bool isBatchOperating;

  tpS Strategy;

  QString prevOpenedDir{"./"};
  QString ProductDir;
  QTranslator trans;
  bool Collected;
  QProgressBar *proTracker;
  // void applyPre(ushort*BL);

private:
  void loadBlockList();
  void turnToPage(int);
  void updateEnables();
  void preprocessImage(const QString &);
  void switchLan(Language);
  void kernelSetType();
  void kernelSetImg();

  QJsonArray getFixedBlocksList(QString);
  QString getFixedBlockListDir(QString);
  QJsonArray getCustomBlockList(QString);
  QString getCustomBlockListDir(QString);

  void selectBlockByString(const std::string &);

  static void progressRangeSet(void *, int min, int max,
                               int val); // 设置进度条的取值范围和值
  static void progressAdd(void *, int deltaVal);
  static void keepAwake(void *);

  static void showError(void *, SlopeCraft::errorFlag, const char *errInfo);
  static void showWorkingStatue(void *, SlopeCraft::workStatues);

  static void algoProgressRangeSet(void *, int min, int max, int val);
  static void algoProgressAdd(void *, int deltaVal);

  static QJsonObject GithubAPIJson2Latest3xVer(const QJsonArray &);
};

QJsonObject loadIni(bool = false);
bool isValidIni(const QJsonObject &);

using EImage = Eigen::ArrayXX<uint32_t>;

EImage QImage2EImage(const QImage &);
QImage EImage2QImage(const EImage &, ushort scale = 1);
#endif // MAINWINDOW_H
