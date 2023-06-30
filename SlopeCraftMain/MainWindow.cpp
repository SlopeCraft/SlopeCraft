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

#include <QDebug>
#include <QProcess>
#include <QRgb>
#include <qcoreapplication.h>
#include <unsupported/Eigen/CXX11/Tensor>

#include "MainWindow.h"

#include <QPushButton>

bool MainWindow::isBatchOperating = false;

blockListPreset MainWindow::preset_vanilla;
blockListPreset MainWindow::preset_elegant;
blockListPreset MainWindow::preset_cheap;
blockListPreset MainWindow::preset_shiny;

inline uint32_t inverseColor(uint32_t raw) noexcept {
  const uint32_t ASeg = raw & (0xFF000000);
  const uint32_t R = 255 - ((raw & 0x00FF0000) >> 16);
  const uint32_t G = 255 - ((raw & 0x0000FF00) >> 8);
  const uint32_t B = 255 - (raw & 0x000000FF);

  return ASeg | (R << 16) | (G << 8) | (B);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      kernel(SlopeCraft::SCL_createKernel()) {
  ui->setupUi(this);

  ui->label_show_version->setText(
      ui->label_show_version->text().arg(QStringLiteral("v") + SC_VERSION_STR));

  // qDebug("成功 setupUi");
  Collected = false;

  // qDebug("成功创建内核");
  kernel->setWindPtr(this);
  kernel->setKeepAwake(keepAwake);
  kernel->setProgressRangeSet(progressRangeSet);
  kernel->setProgressAdd(progressAdd);
  kernel->setAlgoProgressRangeSet(algoProgressRangeSet);
  kernel->setAlgoProgressAdd(algoProgressAdd);
  kernel->setReportError(showError);
  kernel->setReportWorkingStatue(showWorkingStatue);

  proTracker = nullptr;

  ProductDir = "";

  Manager = new BlockListManager(
      (QHBoxLayout *)ui->scrollAreaWidgetContents->layout());

  // qDebug("成功创建方块列表管理者");
  connect(Manager, &BlockListManager::switchToCustom, this,
          &MainWindow::ChangeToCustom);
  connect(Manager, &BlockListManager::blockListChanged, this,
          &MainWindow::onBlockListChanged);
  // connect(Kernel,SIGNAL(convertProgressSetRange(int,int,int)));

  ui->maxHeight->setValue(255);

  connect(ui->progressStart, &QPushButton::clicked, this,
          &MainWindow::turnToPage0);
  connect(ui->progressImPic, &QPushButton::clicked, this,
          &MainWindow::turnToPage1);
  connect(ui->progressType, &QPushButton::clicked, this,
          &MainWindow::turnToPage2);
  connect(ui->progressBL, &QPushButton::clicked, this,
          &MainWindow::turnToPage3);
  connect(ui->progressAdjPic, &QPushButton::clicked, this,
          &MainWindow::turnToPage4);
  connect(ui->progressExLite, &QPushButton::clicked, this,
          &MainWindow::turnToPage5);
  connect(ui->progressExStructure, &QPushButton::clicked, this,
          &MainWindow::turnToPage5);
  connect(ui->progressExWESchem, &QPushButton::clicked, this,
          &MainWindow::turnToPage5);
  connect(ui->progressExFlatDiagram, &QPushButton::clicked, this,
          &MainWindow::turnToPage5);
  // connect(ui->menuExMcF,&QAction::trigger,this,&MainWindow::turnToPage6);
  connect(ui->progressExData, &QPushButton::clicked, this,
          &MainWindow::turnToPage7);

  connect(ui->progressAbout, &QPushButton::clicked, this,
          &MainWindow::turnToPage8);
  connect(ui->actionAboutSlopeCraft, &QAction::triggered, this,
          &MainWindow::turnToPage8);
  connect(ui->actionChinese, &QAction::triggered, this, &MainWindow::turnCh);
  connect(ui->actionEnglish, &QAction::triggered, this, &MainWindow::turnEn);
  connect(ui->progressChinese, &QPushButton::clicked, this,
          &MainWindow::turnCh);
  connect(ui->progressEnglish, &QPushButton::clicked, this,
          &MainWindow::turnEn);
  connect(ui->actionToki, &QAction::triggered, this, &MainWindow::contactG);
  connect(ui->actionDoki, &QAction::triggered, this, &MainWindow::contactB);
  connect(ui->progressG, &QPushButton::clicked, this, &MainWindow::contactG);
  connect(ui->progressB, &QPushButton::clicked, this, &MainWindow::contactB);
  connect(ui->progressCheckUpdates, &QPushButton::clicked, this,
          &MainWindow::checkVersion);

  connect(ui->contact, &QPushButton::clicked, this, &MainWindow::contactB);
  connect(ui->contact, &QPushButton::clicked, this, &MainWindow::contactG);
  connect(ui->actionReportBugs, &QAction::triggered, this,
          &MainWindow::on_reportBugs_clicked);
  connect(ui->actionCheckUpdates, &QAction::triggered, this,
          &MainWindow::checkVersion);

  connect(ui->actionTestBlockList, &QAction::triggered, this,
          &MainWindow::testBlockList);
  connect(ui->actionSetBuildParameters, &QAction::triggered, this,
          &MainWindow::turnToPage5);
  connect(ui->action_export_avaliable_color_list, &QAction::triggered, this,
          &MainWindow::exportAvailableColors);

  // qDebug("成功 connect 所有的菜单");

  connect(ui->NextPage, &QPushButton::clicked, this, &MainWindow::turnToPage2);
  connect(ui->NextPage2, &QPushButton::clicked, this, &MainWindow::turnToPage3);
  connect(ui->NextPage3, &QPushButton::clicked, this, &MainWindow::turnToPage4);
  connect(ui->ExLite, &QPushButton::clicked, this, &MainWindow::turnToPage5);
  connect(ui->ExStructure, &QPushButton::clicked, this,
          &MainWindow::turnToPage5);
  connect(ui->ExWESchem, &QPushButton::clicked, this, &MainWindow::turnToPage5);
  connect(ui->ExportFlatDiagram, &QPushButton::clicked, this,
          &MainWindow::turnToPage5);

  connect(ui->ExData, &QPushButton::clicked, this, &MainWindow::turnToPage7);
  connect(ui->FinishExLite, &QPushButton::clicked, this,
          &MainWindow::turnToPage8);
  connect(ui->FinshExData, &QPushButton::clicked, this,
          &MainWindow::turnToPage8);
  connect(ui->Exit, &QPushButton::clicked, this, &MainWindow::close);
  // qDebug("成功 connect 所有的翻页按钮");

  connect(ui->isGame12, &QRadioButton::toggled, this,
          &MainWindow::onGameVerClicked);
  connect(ui->isGame13, &QRadioButton::toggled, this,
          &MainWindow::onGameVerClicked);
  connect(ui->isGame14, &QRadioButton::toggled, this,
          &MainWindow::onGameVerClicked);
  connect(ui->isGame15, &QRadioButton::toggled, this,
          &MainWindow::onGameVerClicked);
  connect(ui->isGame16, &QRadioButton::toggled, this,
          &MainWindow::onGameVerClicked);
  connect(ui->isGame17, &QRadioButton::toggled, this,
          &MainWindow::onGameVerClicked);
  connect(ui->isGame18, &QRadioButton::toggled, this,
          &MainWindow::onGameVerClicked);
  connect(ui->isGame19, &QRadioButton::toggled, this,
          &MainWindow::onGameVerClicked);

  connect(ui->isMapCreative, &QRadioButton::toggled, this,
          &MainWindow::onMapTypeClicked);
  connect(ui->isMapSurvival, &QRadioButton::toggled, this,
          &MainWindow::onMapTypeClicked);
  /*
  connect(ui->isMapWall,&QRadioButton::toggled,
          this,&MainWindow::onMapTypeClicked);*/
  connect(ui->isMapFlat, &QRadioButton::toggled, this,
          &MainWindow::onMapTypeClicked);

  connect(ui->isBLCreative, &QRadioButton::clicked, this,
          &MainWindow::onPresetsClicked);
  connect(ui->isBLSurvivalCheaper, &QRadioButton::clicked, this,
          &MainWindow::onPresetsClicked);
  connect(ui->isBLSurvivalBetter, &QRadioButton::clicked, this,
          &MainWindow::onPresetsClicked);
  connect(ui->isBLGlowing, &QRadioButton::clicked, this,
          &MainWindow::onPresetsClicked);

  connect(ui->isColorSpaceRGBOld, &QRadioButton::clicked, this,
          &MainWindow::onAlgoClicked);
  connect(ui->isColorSpaceRGB, &QRadioButton::clicked, this,
          &MainWindow::onAlgoClicked);
  connect(ui->isColorSpaceHSV, &QRadioButton::clicked, this,
          &MainWindow::onAlgoClicked);
  connect(ui->isColorSpaceLab94, &QRadioButton::clicked, this,
          &MainWindow::onAlgoClicked);
  connect(ui->isColorSpaceLab00, &QRadioButton::clicked, this,
          &MainWindow::onAlgoClicked);
  connect(ui->isColorSpaceXYZ, &QRadioButton::clicked, this,
          &MainWindow::onAlgoClicked);
  connect(ui->isColorSpaceAi, &QRadioButton::clicked, this,
          &MainWindow::onAlgoClicked);
  connect(ui->AllowDither, &QCheckBox::clicked, this,
          &MainWindow::onAlgoClicked);

  connect(ui->actionStart, &QAction::triggered, ui->progressStart,
          &QPushButton::clicked);
  connect(ui->actionImportImage, &QAction::triggered, ui->progressImPic,
          &QPushButton::clicked);
  connect(ui->actionMapType, &QAction::triggered, ui->progressType,
          &QPushButton::clicked);
  connect(ui->actionBlockList, &QAction::triggered, ui->progressBL,
          &QPushButton::clicked);
  connect(ui->actionConvert, &QAction::triggered, ui->progressAdjPic,
          &QPushButton::clicked);
  connect(ui->actionExportLite, &QAction::triggered, ui->progressExLite,
          &QPushButton::clicked);
  connect(ui->actionExportNBT, &QAction::triggered, ui->progressExStructure,
          &QPushButton::clicked);
  connect(ui->actionExportWESchem, &QAction::triggered, ui->progressExWESchem,
          &QPushButton::clicked);
  connect(ui->actionExportData, &QAction::triggered, ui->progressExData,
          &QPushButton::clicked);

  connect(ui->actionFinish, &QAction::triggered, ui->progressAbout,
          &QPushButton::clicked);
  connect(ui->actionSavePreset, &QAction::triggered, this,
          &MainWindow::onActionSavePreset);
  connect(ui->actionLoadPreset, &QAction::triggered, this,
          &MainWindow::onActionLoadPreset);
  connect(ui->actionAiCvterOption, &QAction::triggered, this,
          &MainWindow::onActionAiCvterParameters);

  turnToPage(0);
}

MainWindow::~MainWindow() {
  SlopeCraft::SCL_destroyKernel(this->kernel);
  delete Manager;
  delete ui;
}

void MainWindow::showPreview() {
  if (kernel->queryStep() < SlopeCraft::step::builded) return;

  PreviewWind *preWind = new PreviewWind(this);
  preWind->Src.resize(64);
  preWind->BlockCount.resize(64);

  preWind->Src = Manager->getQRadioButtonList();

  int totalNum = 0;
  kernel->getBlockCounts(&totalNum, preWind->BlockCount.data());

  // qDebug() << "去重前有：" << preWind->Src.size() << "个元素";
  auto iS = preWind->Src.begin();
  for (auto ib = preWind->BlockCount.begin();
       ib != preWind->BlockCount.end();) {
    if (*iS == nullptr) {
      ib = preWind->BlockCount.erase(ib);
      iS = preWind->Src.erase(iS);
    }
    if (*ib > 0) {  //  if the block is used, keep it. otherwise erase it.
      ib++;
      iS++;
      continue;
    }
    ib = preWind->BlockCount.erase(ib);
    iS = preWind->Src.erase(iS);
  }

  kernel->get3DSize(&preWind->size[0], &preWind->size[1], &preWind->size[2]);

  // qDebug() << "去重后有：" << preWind->Src.size() << "个元素";
  //  preWind->Water=Blocks[12][0];
  preWind->Water = Manager->getQRadioButtonList()[12];
  // preWind->Src[1]=Blocks[1][0];preWind->BlockCount[1]=1919810;
  EImage tempE;
  tempE.resize(kernel->getImageRows(), kernel->getImageCols());
  int a, b;
  kernel->getConvertedImage(&a, &b, tempE.data());

  QImage temp = EImage2QImage(tempE);

  preWind->ShowMaterialList();

  preWind->showConvertedImage(temp);

  preWind->show();
}

void MainWindow::keepAwake(void *) { QCoreApplication::processEvents(); }

void MainWindow::loadBlockList() {
  if (!Manager->setupFixedBlockList("./Blocks/FixedBlocks.json",
                                    "./Blocks/FixedBlocks")) {
    QMessageBox::critical(this, tr("SlopeCraft 无法加载必需方块列表"),
                          tr("SlopeCraft 必须退出"));
    exit(1);
  }

  if (!Manager->setupCustomBlockList("./Blocks/CustomBlocks.json",
                                     "./Blocks/CustomBlocks")) {
    auto ret = QMessageBox::critical(
        this, tr("SlopeCraft 无法加载可选方块列表"), tr("此错误可以忽略"),
        QMessageBox::StandardButtons{QMessageBox::StandardButton::Ignore,
                                     QMessageBox::StandardButton::Close});

    if (ret == QMessageBox::StandardButton::Close) {
      exit(1);
    }
  }

  QRgb colors[64];
  kernel->getBaseColorInARGB32(colors);
  Manager->setLabelColors(colors);
  // applyPre(BLBetter);
  Manager->loadInternalPreset(preset_elegant);
}

void MainWindow::InitializeAll() {
  ui->LeftScroll->verticalScrollBar()->setStyleSheet(
      "QScrollBar{width: 7px;margin: 0px 0 0px 0;background-color: rgba(255, "
      "255, 255, 64);color: rgba(255, 255, 255, 128);}");
  static bool needInitialize = true;
  if (needInitialize) {
    needInitialize = false;
  }

  try {
    MainWindow::preset_vanilla =
        load_preset("./Blocks/Presets/vanilla.sc_preset_json");
    MainWindow::preset_cheap =
        load_preset("./Blocks/Presets/cheap.sc_preset_json");
    MainWindow::preset_elegant =
        load_preset("./Blocks/Presets/elegant.sc_preset_json");
    MainWindow::preset_shiny =
        load_preset("./Blocks/Presets/shiny.sc_preset_json");
  } catch (std::exception &e) {
    QMessageBox::critical(this, tr("加载默认预设失败"),
                          tr("一个或多个内置的预设不能被解析。SlopeCraft "
                             "可能已经损坏，请重新安装。\n具体报错信息：\n%1")
                              .arg(e.what()));
    exit(1);
  }

  if (!Collected) {
    loadBlockList();
    // qDebug("方块列表加载完毕");
    Manager->setVersion((unsigned char)SlopeCraft::gameVersion::MC17);
    onPresetsClicked();
    Collected = true;
  }
}

void MainWindow::on_actionTutorial_triggered() {
  QDesktopServices::openUrl(QUrl{"https://slopecraft.readthedocs.io/"});
}

void MainWindow::contactG() {
  QDesktopServices::openUrl(QUrl("https://github.com/SlopeCraft/SlopeCraft"));
}

void MainWindow::contactB() {
  QDesktopServices::openUrl(QUrl("https://space.bilibili.com/351429231"));
}

#ifndef tpSDestroyer
#define tpSDestroyer
tpS::~tpS() {}
#endif

void MainWindow::turnToPage(int page) {
  page %= 9;
  QString newtitle = "SlopeCraft ";

  newtitle += SlopeCraft::SCL_getSCLVersion();
#ifdef WIN32
  newtitle += " Copyright © 2021-2023 TokiNoBug    ";  // windowsf
#elif defined(_MAC) || defined(__APPLE__)
  newtitle +=
      " Copyright © 2021-2023 TokiNoBug,AbrasiveBoar, Cubik65536   ";  // macOs
#else
  newtitle += " Copyright © 2021-2023 TokiNoBug    ";  // unknown platform
#endif

  switch (page) {
    case 0:
      newtitle += "Step 0 / 6";
      newtitle += "    ";
      newtitle += tr("开始");
      ui->stackedWidget->setCurrentIndex(page);
      break;

    case 1:
      newtitle += "Step 1 / 6";
      newtitle += "    ";
      newtitle += tr("导入图片");
      ui->stackedWidget->setCurrentIndex(page);
      break;
    case 2:
      newtitle += "Step 2 / 6";
      newtitle += "    ";
      newtitle += tr("设置地图画类型");
      ui->stackedWidget->setCurrentIndex(page);
      break;
    case 3:
      newtitle += "Step 3 / 6";
      newtitle += "    ";
      newtitle += tr("设置方块列表");
      ui->stackedWidget->setCurrentIndex(page);
      break;
    case 4:
      newtitle += "Step 4 / 6";
      newtitle += "    ";
      newtitle += tr("调整颜色");
      ui->stackedWidget->setCurrentIndex(page);
      break;
    case 5:
      newtitle += "Step 5 / 6";
      newtitle += "    ";
      newtitle += tr("导出为投影文件");
      ui->stackedWidget->setCurrentIndex(page);
      break;
    case 6:
      newtitle += "Step 5 / 6";
      newtitle += "    ";
      newtitle += tr("导出为 mcfunction");
      ui->stackedWidget->setCurrentIndex(page);
      break;
    case 7:
      newtitle += "Step 5 / 6";
      newtitle += "    ";
      newtitle += tr("导出为地图文件");
      ui->stackedWidget->setCurrentIndex(page);
      break;
    case 8:
      newtitle += "Step 6 / 6";
      newtitle += "    ";
      newtitle += tr("结束");
      ui->stackedWidget->setCurrentIndex(page);
      break;
    default:
      qDebug("尝试翻页错误");
      break;
  }
  this->setWindowTitle(newtitle);
  updateEnables();
  return;
}

void MainWindow::turnToPage0() {
  turnToPage(0);
  return;
}

void MainWindow::turnToPage1() {
  turnToPage(1);
  return;
}

void MainWindow::turnToPage2() {
  turnToPage(2);
  return;
}

void MainWindow::turnToPage3() {
  turnToPage(3);
  return;
}

void MainWindow::turnToPage4() {
  turnToPage(4);
  return;
}

void MainWindow::turnToPage5() {
  turnToPage(5);
  return;
}

void MainWindow::turnToPage6() {
  turnToPage(6);
  return;
}

void MainWindow::turnToPage7() {
  turnToPage(7);
  return;
}

void MainWindow::turnToPage8() {
  turnToPage(8);
  return;
}

void MainWindow::updateEnables() {
  bool temp = true;
  ui->StartWithFlat->setEnabled(temp);
  ui->StartWithNotVanilla->setEnabled(temp);
  ui->StartWithNotVanilla->setEnabled(temp);

  temp = kernel->queryStep() >= SlopeCraft::step::wait4Image;
  ui->actionTestBlockList->setEnabled(temp);
  ui->action_export_avaliable_color_list->setEnabled(temp);

  temp = kernel->queryStep() >= SlopeCraft::step::convertionReady;
  ui->Convert->setEnabled(temp);
  ui->ShowRaw->setEnabled(temp);
  // ui->Convert->setEnabled(temp);

  // ////////////////
  temp = kernel->queryStep() >= SlopeCraft::step::converted;
  ui->ShowAdjed->setEnabled(temp);
  // ui->menuExport->setEnabled(temp);
  // ui->ExData->setEnabled(temp);
  ui->ExportData->setEnabled(temp);
  // ui->actionExportData->setEnabled(temp);
  // ui->progressEx->setEnabled(temp);
  // ui->progressExData->setEnabled(temp);

  // ////////////////
  temp = (!ui->isMapCreative->isChecked()) &&
         kernel->queryStep() >= SlopeCraft::step::converted;
  ui->Build4Lite->setEnabled(temp);
  // ui->actionExportLite->setEnabled(temp);
  // ui->actionExportNBT->setEnabled(temp);
  // ui->actionExportWESchem->setEnabled(temp);
  // ui->ExLite->setEnabled(temp);
  // ui->ExStructure->setEnabled(temp);
  // ui->ExWESchem->setEnabled(temp);
  // ui->progressExLite->setEnabled(temp);
  // ui->progressExStructure->setEnabled(temp);
  // ui->progressExFlatDiagram->setEnabled(temp);
  // ui->progressExWESchem->setEnabled(temp);

  // ////////////////
  temp = kernel->queryStep() >= SlopeCraft::step::builded;
  ui->ExportLite->setEnabled(temp);
  ui->ManualPreview->setEnabled(temp);
  ui->ExportFlatDiagram->setEnabled(temp && (kernel->isFlat()));
}

void MainWindow::on_StartWithSlope_clicked() {
  ui->isMapSurvival->setChecked(true);
  turnToPage(1);
  onBlockListChanged();
}

void MainWindow::on_StartWithFlat_clicked() {
  ui->isMapFlat->setChecked(true);
  onBlockListChanged();
  turnToPage(1);
}

void MainWindow::on_StartWithNotVanilla_clicked() {
  ui->isMapCreative->setChecked(true);
  onBlockListChanged();
  turnToPage(1);
}
/*
void MainWindow::on_StartWithWall_clicked() {
    ui->isMapWall->setChecked(true);
    onBlockListChanged();
    turnToPage(1);
}
*/

void MainWindow::preprocessImage(const QString &Path) {
  if (!rawPic.load(Path)) {
    QMessageBox::information(this, tr("打开图片失败"),
                             tr("要不试试换一张图片吧！"));
    return;
  }
  bool needSearch = rawPic.hasAlphaChannel();
  rawPic = rawPic.convertToFormat(QImage::Format_ARGB32);
  bool OriginHasTp = false;
  if (needSearch) {
    QRgb *CL = nullptr;
    for (short r = 0; r < rawPic.height(); r++) {
      CL = (QRgb *)rawPic.scanLine(r);
      for (short c = 0; c < rawPic.width(); c++) {
        if (qAlpha(CL[c]) < 255) {
          r = rawPic.height() + 1;
          OriginHasTp = true;
          break;
        }
      }
    }
  }

  // ui->ShowRawPic->setPixmap(QPixmap::fromImage(rawPic));

  ui->IntroPicInfo->setText(tr("图片尺寸：") +
                            QString::number(rawPic.height()) + "×" +
                            QString::number(rawPic.width()) + tr("像素"));
  if (OriginHasTp) {
    preProcess(Strategy.pTpS, Strategy.hTpS, Strategy.BGC);
    ui->IntroPicInfo->setText(
        ui->IntroPicInfo->text() + "\n" +
        tr("图片中存在透明/"
           "半透明像素，已处理，您可以点击“设置”重新选择处理透明/"
           "半透明像素的方式。\n重新设置处理方式后，需要重新导入一次。"));
  } else {
    rawPic = rawPic.copy();
  }
  ui->ShowRawPic->setPixmap(QPixmap::fromImage(rawPic));
  ui->ShowPic->setPixmap(QPixmap::fromImage(rawPic));

  kernel->decreaseStep(SlopeCraft::step::nothing);
  this->kernelSetType();
  this->kernelSetImg();
  this->updateEnables();
}

void MainWindow::on_ImportPic_clicked() {
  QStringList userSelected = QFileDialog::getOpenFileNames(
      this, tr("选择图片"), this->prevOpenedDir,
      tr("图片 (*.png *.bmp *.jpg *.tif *.GIF )"));

  if (userSelected.isEmpty()) return;

  this->prevOpenedDir = QFileInfo(userSelected.front()).filePath();

  if (userSelected.size() == 1) {
    QString Path = userSelected.front();

    if (Path.isEmpty()) {
      return;
    }
    preprocessImage(Path);

    return;
  } else {
    auto bo = new BatchUi(this);
    // qDebug("开始创建 BatchUi");
    bo->setTasks(userSelected);
    bo->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
    bo->show();
    /*
    connect(this,&MainWindow::mapTypeChanged,
            batchOperator,&BatchUi::taskTypeUpdated);
            */
    // qDebug("Mainwindow setTasks 完毕");
    return;
  }
}

void MainWindow::on_ImportSettings_clicked() {
  auto transSubWind = new tpStrategyWind(this);
  connect(transSubWind, &tpStrategyWind::Confirm, this,
          &MainWindow::ReceiveTPS);
  transSubWind->setVal(Strategy);

  transSubWind->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  transSubWind->show();
}

void MainWindow::ReceiveTPS(tpS t) {
  this->Strategy = t;
  // qDebug("接收成功");
  // qDebug() << "pTpS=" << t.pTpS << ". hTpS=" << t.hTpS;
}

QRgb ComposeColor(const QRgb front, const QRgb back) {
  QRgb red =
      (qRed(front) * qAlpha(front) + qRed(back) * (255 - qAlpha(front))) / 255;
  QRgb green =
      (qGreen(front) * qAlpha(front) + qGreen(back) * (255 - qAlpha(front))) /
      255;
  QRgb blue =
      (qBlue(front) * qAlpha(front) + qBlue(back) * (255 - qAlpha(front))) /
      255;
  return qRgb(red, green, blue);
}

inline void MainWindow::preProcess(char pureTpStrategy, char halfTpStrategy,
                                   QRgb BGC) {
  // qDebug("调用了 preProcess");
  //  透明像素处理策略：B->替换为背景色；A->空气；W->暂缓，等待处理
  //  半透明像素处理策略：B->替换为背景色；C->与背景色叠加；R->保留颜色；W->暂缓，等待处理
  // qDebug("Cpoied");
  bool hasTotalTrans = false;
  if (pureTpStrategy != 'W' && halfTpStrategy != 'W') {
    QRgb *CL = nullptr;
    for (int r = 0; r < rawPic.height(); r++) {
      CL = (QRgb *)rawPic.scanLine(r);
      for (int c = 0; c < rawPic.width(); c++) {
        if (qAlpha(CL[c]) >= 255) continue;
        if (qAlpha(CL[c]) == 0) switch (pureTpStrategy) {
            case 'B':
              CL[c] = BGC;
              continue;
            case 'A':
              if (!hasTotalTrans) {
                // qDebug() << "发现纯透明像素";
                hasTotalTrans = true;
              }
              CL[c] = qRgba(0, 0, 0, 0);
              continue;
          }

        // qDebug("neeeee");
        switch (halfTpStrategy) {
          case 'B':
            CL[c] = BGC;
            break;
          case 'C':
            CL[c] = ComposeColor(CL[c], BGC);
            break;
          case 'R':
            CL[c] = qRgb(qRed(CL[c]), qGreen(CL[c]), qBlue(CL[c]));
        }
      }
    }
  }
}

void MainWindow::onGameVerClicked() {
  if (ui->isGame12->isChecked()) {
    Manager->setVersion(12);
  }
  if (ui->isGame13->isChecked()) {
    Manager->setVersion(13);
  }
  if (ui->isGame14->isChecked()) {
    Manager->setVersion(14);
  }
  if (ui->isGame15->isChecked()) {
    Manager->setVersion(15);
  }
  if (ui->isGame16->isChecked()) {
    Manager->setVersion(16);
  }
  if (ui->isGame17->isChecked()) {
    Manager->setVersion(17);
  }
  if (ui->isGame18->isChecked()) {
    Manager->setVersion(18);
  }
  if (ui->isGame19->isChecked()) {
    Manager->setVersion(19);
  }
  kernel->decreaseStep(SlopeCraft::step::nothing);
  onBlockListChanged();

  this->kernelSetType();
  if (!this->rawPic.isNull()) {
    kernelSetImg();
  }
  this->updateEnables();
}

void MainWindow::onMapTypeClicked() {
  if (ui->isMapCreative->isChecked()) {
    Manager->setEnabled(12, true);
  }
  if (ui->isMapFlat->isChecked()) {
    Manager->setEnabled(12, true);
  }
  if (ui->isMapSurvival->isChecked()) {
    Manager->setEnabled(12, false);
  }
  /*
  if(ui->isMapWall->isChecked()) {
      Manager->setEnabled(12,false);
  }*/
  kernel->decreaseStep(SlopeCraft::step::nothing);
  onBlockListChanged();

  this->kernelSetType();
  if (!this->rawPic.isNull()) {
    kernelSetImg();
  }
  this->updateEnables();
}

void MainWindow::ChangeToCustom() {
  ui->isBLCustom->setChecked(true);
  kernel->decreaseStep(SlopeCraft::step::nothing);
  updateEnables();
}

void MainWindow::onPresetsClicked() {
  if (ui->isBLCreative->isChecked()) {
    Manager->loadInternalPreset(preset_vanilla);
  }
  if (ui->isBLSurvivalCheaper->isChecked()) {
    Manager->loadInternalPreset(preset_cheap);
  }
  if (ui->isBLSurvivalBetter->isChecked()) {
    Manager->loadInternalPreset(preset_elegant);
  }
  if (ui->isBLGlowing->isChecked()) {
    Manager->loadInternalPreset(preset_shiny);
  }

  if (ui->isMapSurvival->isChecked()) Manager->setEnabled(12, false);

  kernel->decreaseStep(SlopeCraft::step::nothing);

  this->kernelSetType();
  if (!this->rawPic.isNull()) {
    this->kernelSetImg();
  }

  updateEnables();
}

void MainWindow::onAlgoClicked() {
  static SlopeCraft::convertAlgo lastChoice =
      SlopeCraft::convertAlgo::RGB_Better;
  static bool lastDither = false;

  SlopeCraft::convertAlgo now;
  const bool nowDither = ui->AllowDither->isChecked();
  if (ui->isColorSpaceRGBOld->isChecked()) now = SlopeCraft::convertAlgo::RGB;
  if (ui->isColorSpaceRGB->isChecked())
    now = SlopeCraft::convertAlgo::RGB_Better;
  if (ui->isColorSpaceHSV->isChecked()) now = SlopeCraft::convertAlgo::HSV;
  if (ui->isColorSpaceLab94->isChecked()) now = SlopeCraft::convertAlgo::Lab94;
  if (ui->isColorSpaceLab00->isChecked()) now = SlopeCraft::convertAlgo::Lab00;
  if (ui->isColorSpaceXYZ->isChecked()) now = SlopeCraft::convertAlgo::XYZ;
  if (ui->isColorSpaceAi->isChecked()) now = SlopeCraft::convertAlgo::gaCvter;

  if (lastChoice == SlopeCraft::convertAlgo::gaCvter) kernelSetImg();

  if ((lastChoice != now) || (lastDither != nowDither))
    kernel->decreaseStep(SlopeCraft::step::convertionReady);

  updateEnables();
  lastChoice = now;
  lastDither = nowDither;
}

void MainWindow::kernelSetType() {
  SlopeCraft::mapTypes type = SlopeCraft::mapTypes::Slope;
  {
    if (ui->isMapCreative->isChecked()) type = SlopeCraft::mapTypes::FileOnly;
    if (ui->isMapFlat->isChecked()) type = SlopeCraft::mapTypes::Flat;
    if (ui->isMapSurvival->isChecked()) type = SlopeCraft::mapTypes::Slope;
    /*
    if(ui->isMapWall->isChecked())
        type=SlopeCraft::mapTypes::Wall;*/
  }

  SlopeCraft::gameVersion ver = SlopeCraft::gameVersion::MC19;
  {
    if (ui->isGame12->isChecked()) ver = SlopeCraft::gameVersion::MC12;
    if (ui->isGame13->isChecked()) ver = SlopeCraft::gameVersion::MC13;
    if (ui->isGame14->isChecked()) ver = SlopeCraft::gameVersion::MC14;
    if (ui->isGame15->isChecked()) ver = SlopeCraft::gameVersion::MC15;
    if (ui->isGame16->isChecked()) ver = SlopeCraft::gameVersion::MC16;
    if (ui->isGame17->isChecked()) ver = SlopeCraft::gameVersion::MC17;
    if (ui->isGame18->isChecked()) ver = SlopeCraft::gameVersion::MC18;
    if (ui->isGame19->isChecked()) ver = SlopeCraft::gameVersion::MC19;
    if (ui->isGame20->isChecked()) ver = SlopeCraft::gameVersion::MC20;
  }

  bool allowedBaseColor[64];
  Manager->getEnableList(allowedBaseColor);

  std::array<const SlopeCraft::AbstractBlock *, 64> palette;
  // const AbstractBlock * palette[64];
  Manager->getSimpleBlockList(palette.data());

  kernel->setType(type, ver, allowedBaseColor, palette.data());
  const uint8_t *allowedMap;
  int colorN = 0;

  SlopeCraft::SCL_getColorMapPtrs(nullptr, nullptr, nullptr, &allowedMap,
                                  &colorN);
  /*
  cout<<"\n\nAllowedMap=";
  for(int i=0;i<colorN;i++) {
      cout<<int(allowedMap[i])<<" , ";
  }
  cout<<endl<<endl;
  */
  // updateEnables();

  // TokiTask::canExportLite=kernel->isVanilla();
  emit mapTypeChanged();
}

void MainWindow::kernelSetImg() {
  EImage rawImg = QImage2EImage(rawPic);
  kernel->setRawImage(rawImg.data(), rawImg.rows(), rawImg.cols());
  // this->updateEnables();
}

EImage QImage2EImage(const QImage &qi) {
  EImage ei;
  if (qi.isNull() || qi.height() <= 0 || qi.width() <= 0) {
    ei.setZero(0, 0);
    return ei;
  }
  ei.setZero(qi.height(), qi.width());
  const QRgb *CL = nullptr;
  for (int r = 0; r < ei.rows(); r++) {
    CL = (const QRgb *)qi.scanLine(r);
    for (int c = 0; c < ei.cols(); c++) ei(r, c) = CL[c];
  }
  return ei;
}

QImage EImage2QImage(const EImage &ei, ushort scale) {
  QImage qi(ei.cols() * scale, ei.rows() * scale,
            QImage::Format::Format_ARGB32);
  QRgb *CL = nullptr;
  for (int r = 0; r < qi.height(); r++) {
    CL = (QRgb *)qi.scanLine(r);
    for (int c = 0; c < qi.width(); c++) CL[c] = ei(r / scale, c / scale);
  }
  return qi;
}

void MainWindow::progressRangeSet(void *p, int min, int max, int val) {
  MainWindow *wind = (MainWindow *)p;
  // 设置进度条的取值范围和值
  if (wind->proTracker == nullptr) {
    qDebug("错误！proTracker==nullptr");
    return;
  }
  wind->proTracker->setRange(min, max);
  wind->proTracker->setValue(val);
}

void MainWindow::progressAdd(void *p, int deltaVal) {
  MainWindow *wind = (MainWindow *)p;
  if (wind->proTracker == nullptr) {
    qDebug("错误！proTracker==nullptr");
    return;
  }
  wind->proTracker->setValue(deltaVal + wind->proTracker->value());
}

void MainWindow::algoProgressRangeSet(void *p, int min, int max, int val) {
  MainWindow *wind = (MainWindow *)p;
  wind->ui->algoBar->setRange(min, max);
  wind->ui->algoBar->setValue(val);
}

void MainWindow::algoProgressAdd(void *p, int deltaVal) {
  MainWindow *wind = (MainWindow *)p;
  wind->ui->algoBar->setValue(wind->ui->algoBar->value() + deltaVal);
}

void MainWindow::on_Convert_clicked() {
  if (kernel->queryStep() < SlopeCraft::step::wait4Image) {
    // qDebug("setType again");
    onBlockListChanged();
    if (kernel->queryStep() < SlopeCraft::step::wait4Image) return;
  }

  if (kernel->queryStep() < SlopeCraft::step::convertionReady) {
    // qDebug("setImage again");
    kernelSetImg();
    if (kernel->queryStep() < SlopeCraft::step::convertionReady) return;
  }

  SlopeCraft::convertAlgo now = SlopeCraft::convertAlgo::RGB;
  bool nowDither = ui->AllowDither->isChecked();
  {
    if (ui->isColorSpaceRGBOld->isChecked()) now = SlopeCraft::convertAlgo::RGB;
    if (ui->isColorSpaceRGB->isChecked())
      now = SlopeCraft::convertAlgo::RGB_Better;
    if (ui->isColorSpaceHSV->isChecked()) now = SlopeCraft::convertAlgo::HSV;
    if (ui->isColorSpaceLab94->isChecked())
      now = SlopeCraft::convertAlgo::Lab94;
    if (ui->isColorSpaceLab00->isChecked())
      now = SlopeCraft::convertAlgo::Lab00;
    if (ui->isColorSpaceXYZ->isChecked()) now = SlopeCraft::convertAlgo::XYZ;
    if (ui->isColorSpaceAi->isChecked()) now = SlopeCraft::convertAlgo::gaCvter;
  }

  proTracker = ui->ShowProgressABbar;

  kernel->decreaseStep(SlopeCraft::step::convertionReady);
  updateEnables();

  bool temp = false;
  ui->Convert->setEnabled(temp);  // 防止用户在繁忙时重复操作
  ui->isColorSpaceHSV->setEnabled(temp);
  ui->isColorSpaceRGB->setEnabled(temp);
  ui->isColorSpaceLab94->setEnabled(temp);
  ui->isColorSpaceLab00->setEnabled(temp);
  ui->isColorSpaceXYZ->setEnabled(temp);
  ui->isColorSpaceRGBOld->setEnabled(temp);
  ui->isColorSpaceAi->setEnabled(temp);
  ui->AllowDither->setEnabled(temp);

  std::clock_t startTime = std::clock();
  qDebug("Start to convert");
  kernel->convert(now, nowDither);

  qDebug() << "Convertion finished in "
           << double(std::clock() - startTime) * 1000.0 / CLOCKS_PER_SEC
           << " miliseconds.";
  proTracker = nullptr;

  temp = true;
  ui->Convert->setEnabled(temp);  // 恢复锁定
  ui->isColorSpaceHSV->setEnabled(temp);
  ui->isColorSpaceRGB->setEnabled(temp);
  ui->isColorSpaceLab94->setEnabled(temp);
  ui->isColorSpaceLab00->setEnabled(temp);
  ui->isColorSpaceXYZ->setEnabled(temp);
  ui->isColorSpaceRGBOld->setEnabled(temp);
  ui->isColorSpaceAi->setEnabled(temp);
  ui->AllowDither->setEnabled(temp);
  on_ShowAdjed_clicked();
  updateEnables();
}

void MainWindow::on_ShowRaw_clicked() {
  ui->ShowPic->setPixmap(QPixmap::fromImage(rawPic));
}

void MainWindow::on_ShowAdjed_clicked() {
  EImage ei(kernel->getImageRows(), kernel->getImageCols());
  // int a, b;
  kernel->getConvertedImage(nullptr, nullptr, ei.data());
  ui->ShowPic->setPixmap(QPixmap::fromImage(EImage2QImage(ei)));
}

void MainWindow::on_ExData_clicked() {
  int mapRows = ceil(kernel->getImageRows() / 128.0);
  int mapCols = ceil(kernel->getImageCols() / 128.0);
  int mapCounts = mapRows * mapCols;
  ui->ShowDataRows->setText(QString::number(mapRows));
  ui->ShowDataCols->setText(QString::number(mapCols));
  ui->ShowDataCounts->setText(QString::number(mapCounts));
  ui->InputDataIndex->setText("0");
}

void MainWindow::on_ExportFlatDiagram_clicked() {
  const QString path =
      QFileDialog::getSaveFileName(this, "保存为平面示意图", "", "*.png");

  if (path.isEmpty()) return;

  ui->ExportFlatDiagram->setEnabled(false);
  ui->progressExFlatDiagram->setEnabled(false);
  ui->ExportLite->setEnabled(false);
  ui->progressExLite->setEnabled(false);

  this->proTracker = ui->ShowProgressExLite;
  // ui->ShowProgressExLite->setValue(0);

  constexpr int charHeight = 14;
  constexpr int charWidth = 10;
  constexpr int charSpace = 1;
  constexpr int leftSpace = 2;
  constexpr int topSpace = 2;

  /*
  here [] means an blank pixel
  left border : [][]char[]char[]...char[]Blocks

  top border :
  []
  []
  char
  []
  char
  []
  ...
  char
  []
  Blocks
  */

  const int buildRows = kernel->getZRange();
  const int buildCols = kernel->getXRange();

  /// pixel rows and cols of single block
  constexpr int blockRowsCols = 16;

  const int numberRowsDigits = std::ceil(std::log10(1e-2 + buildRows));
  const int numberColsDigits = std::ceil(std::log10(1e-2 + buildCols));

  const int leftBorderWidth =
      leftSpace + numberColsDigits * (charWidth + charSpace);
  const int topBorderWidth =
      topSpace + numberRowsDigits * (charHeight + charSpace);

  /// the height of exported image
  const int diagramRows = blockRowsCols * buildRows + topBorderWidth;
  /// the width of exported image
  const int diagramCols = blockRowsCols * buildCols + leftBorderWidth;

  // progress bar range: 0~ rows + cols + pixNum + rows + cols + pixNum/10
  const int progressMax = buildRows + buildCols + buildRows * buildCols +
                          buildRows + buildCols + buildRows * buildCols / 10;
  // ui->ShowProgressExLite->setRange(0,progressMax);
  progressRangeSet(this, 0, progressMax, 0);

  using blockEImg_t =
      Eigen::Array<uint32_t, blockRowsCols, blockRowsCols, Eigen::RowMajor>;
  std::vector<blockEImg_t> blockImgs16;
  blockImgs16.reserve(64);

  // fill blockImgs16 with images of each block
  {
    auto tokiBlockList = Manager->getTokiBlockList();
    for (auto tokiBlock : tokiBlockList) {
      if (tokiBlock == nullptr) {
        break;
      }
      QImage tempImg = tokiBlock->getTarget()
                           ->icon()
                           .pixmap(blockRowsCols, blockRowsCols)
                           .toImage()
                           .convertToFormat(QImage::Format_ARGB32);
      Eigen::Map<blockEImg_t> map(
          reinterpret_cast<uint32_t *>(tempImg.scanLine(0)));
      SlopeCraft::SCL_preprocessImage(map.data(), map.size());
      blockImgs16.emplace_back(map);
    }
  }

  using charEImg_t =
      Eigen::Array<uint32_t, charHeight, charWidth, Eigen::RowMajor>;

  std::vector<charEImg_t> numberImgs(10);
  // fill numberImgs with images of each number
  {
    QImage tempImg = QImage(":/new/Pic/MCStyledNumbers.png")
                         .convertToFormat(QImage::Format_ARGB32);
    Eigen::Map<
        Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        mappedTempImg(reinterpret_cast<uint32_t *>(tempImg.scanLine(0)),
                      tempImg.height(), tempImg.width());
    for (int idx = 0; idx < 10; idx++) {
      numberImgs[idx] =
          mappedTempImg.block<charHeight, charWidth>(0, idx * charWidth);
    }
  }
  // cout<<"Size of blockImgs16 = "<<blockImgs16.size()<<endl;

  Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      EDiagram;

  EDiagram.setConstant(diagramRows, diagramCols, 0xFFFFFFFFU);

  // write row numbers
  for (int row = 0; row < buildRows; row++) {
    const std::string str = std::to_string(row);

    const int rowOffset =
        topBorderWidth + blockRowsCols * row + (blockRowsCols - charHeight) / 2;
    // cout<<"\n\nrowOffset = "<<rowOffset<<"\n";
    //  write inversly
    for (int invIdx = 0; invIdx < numberRowsDigits; invIdx++) {
      if (invIdx >= (int)str.size()) break;

      // the reverse idx
      const char curChar = str[str.size() - 1 - invIdx];
      const int curCharIdx = curChar - '0';

      const int colOffset =
          leftBorderWidth - (invIdx + 1) * (charWidth + charSpace);

      // cout<<"colOffset = "<<colOffset<<'\n';

      EDiagram.block<charHeight, charWidth>(rowOffset, colOffset) =
          numberImgs[curCharIdx];
    }
  }

  progressAdd(this, buildRows);
  keepAwake(this);
  // cout<<endl;
  //  write col numbers
  for (int col = 0; col < buildCols; col++) {
    const std::string str = std::to_string(col);
    const int colOffset =
        leftBorderWidth + blockRowsCols * col + (blockRowsCols - charWidth) / 2;
    for (int invIdx = 0; invIdx < numberColsDigits; invIdx++) {
      if (invIdx >= (int)str.size()) break;
      const int curCharIdx = str[str.size() - 1 - invIdx] - '0';

      const int rowOffset =
          topBorderWidth - (invIdx + 1) * (charHeight + charSpace);
      EDiagram.block<charHeight, charWidth>(rowOffset, colOffset) =
          numberImgs[curCharIdx];
    }
  }

  progressAdd(this, buildCols);
  keepAwake(this);

  const Eigen::TensorMap<const Eigen::Tensor<uint16_t, 3>> build(
      kernel->getBuild(), kernel->getXRange(), kernel->getHeight(),
      kernel->getZRange());

  static constexpr int reportInterval = 512;

  // fill diagram with blocks
  for (int bRow = 0; bRow < buildRows; bRow++) {
    constexpr int yPos = 0;
    const int rowOffset = topBorderWidth + blockRowsCols * bRow;
    const int zPos = bRow;
    for (int bCol = 0; bCol < buildCols; bCol++) {
      const int xPos = bCol;
      const int eleIdx = &build(xPos, yPos, zPos) - build.data();

      if (eleIdx % reportInterval == 0) {
        progressAdd(this, reportInterval);
        keepAwake(this);
      }

      if (build(xPos, yPos, zPos) <= 0) continue;

      const int blockIdx = build(xPos, yPos, zPos) - 1;

      const int colOffset = leftBorderWidth + blockRowsCols * bCol;

      EDiagram.block<blockRowsCols, blockRowsCols>(rowOffset, colOffset) =
          blockImgs16[blockIdx];
    }
  }

  // draw horizontal lines
  for (int lineRows = 1; lineRows < buildRows; lineRows += 16) {
    const int row = topBorderWidth + blockRowsCols * lineRows;
    for (int col = leftBorderWidth; col < diagramCols; col++) {
      EDiagram(row, col) = inverseColor(EDiagram(row, col));
    }
  }
  progressAdd(this, buildRows);

  // draw vertical lines
  for (int lineCols = 1; lineCols < buildCols; lineCols += 16) {
    const int col = leftBorderWidth + blockRowsCols * lineCols;
    for (int row = topBorderWidth; row < diagramRows; row++) {
      // if((row-topBorderWidth)%(blockRowsCols*16)==0)                continue;

      EDiagram(row, col) = inverseColor(EDiagram(row, col));
    }
  }
  progressAdd(this, buildCols);
  keepAwake(this);

  QImage(reinterpret_cast<uint8_t *>(EDiagram.data()), diagramCols, diagramRows,
         QImage::Format_ARGB32)
      .save(path);

  progressRangeSet(this, 0, progressMax, progressMax);
  keepAwake(this);

  this->proTracker = nullptr;

  ui->FinishExLite->setEnabled(true);

  this->ProductDir = path;

  ProductDir = ProductDir.replace('\\', '/');
  ProductDir = ProductDir.left(ProductDir.lastIndexOf('/'));

  ui->seeExported->setEnabled(true);

  updateEnables();
}

// Page5

void MainWindow::on_Build4Lite_clicked() {
  bool naturalCompress = ui->AllowNaturalOpti->isChecked();
  bool forcedCompress = ui->AllowForcedOpti->isChecked();
  SlopeCraft::compressSettings cS;
  if (naturalCompress) {
    if (forcedCompress)
      cS = SlopeCraft::compressSettings::Both;
    else
      cS = SlopeCraft::compressSettings::NaturalOnly;
  } else {
    if (forcedCompress)
      cS = SlopeCraft::compressSettings::ForcedOnly;
    else
      cS = SlopeCraft::compressSettings::noCompress;
  }

  bool allowBridge = ui->allowGlassBridge->isChecked();
  SlopeCraft::glassBridgeSettings gBS =
      allowBridge ? SlopeCraft::glassBridgeSettings::withBridge
                  : SlopeCraft::glassBridgeSettings::noBridge;

  kernel->decreaseStep(SlopeCraft::step::converted);
  ui->ExportLite->setEnabled(false);
  ui->FinishExLite->setEnabled(false);
  ui->ManualPreview->setEnabled(false);

  proTracker = ui->ShowProgressExLite;
  qDebug() << "ui->maxHeight->value()=" << ui->maxHeight->value();
  kernel->build(
      cS, ui->maxHeight->value(), gBS, ui->glassBridgeInterval->value(),
      ui->allowAntiFire->isChecked(), ui->allowAntiEnderman->isChecked());

  int size3D[3], total;

  kernel->get3DSize(&size3D[0], &size3D[1], &size3D[2]);
  total = kernel->getBlockCounts();
  ui->ShowLiteBlocks->setText(QString::number(total));
  ui->ShowLiteXYZ->setText(QString::fromStdString(
      "X:" + std::to_string(size3D[0]) + "  × Y:" + std::to_string(size3D[1]) +
      "  × Z:" + std::to_string(size3D[2])));
  proTracker = nullptr;
  updateEnables();
  if (!isBatchOperating) showPreview();
}

void MainWindow::on_ManualPreview_clicked() { showPreview(); }

void MainWindow::on_ExportLite_clicked() { onExportLiteclicked(""); }

void MainWindow::onExportLiteclicked(QString path) {
  std::string FileName;
  if (path.isEmpty()) {
    QStringList suffixes({tr("投影文件 (*.litematic)"),
                          tr("结构方块文件 (*.nbt)"),
                          tr("WorldEdit 原理图 (*.schem)")});

    const int first_format_idx = ui->tabExport3DInfo->currentIndex();

    QString suffix = suffixes[first_format_idx];

    for (int idx = 0; idx < suffixes.size(); idx++) {
      if (idx != first_format_idx) {
        suffix += ";;";
        suffix += suffixes[idx];
      }
    }

    FileName = QFileDialog::getSaveFileName(this, tr("导出为投影/结构方块文件"),
                                            "", suffix)
                   .toLocal8Bit()
                   .data();
  } else {
    FileName = path.toLocal8Bit().data();
  }
  // std::string unCompressed;
  char failed_file_name[512] = "";
  if (FileName.empty()) return;
  const bool putLitematic =
      (FileName.substr(FileName.length() - strlen(".litematic")) ==
       ".litematic");
  const bool putStructure =
      (FileName.substr(FileName.length() - strlen(".nbt")) == ".nbt");
  const bool putWESchem =
      (FileName.substr(FileName.length() - strlen(".schem")) == ".schem");

  if (!putLitematic && !putStructure && !putWESchem) {
    qDebug("Invalid save file name : ");
    qDebug() << FileName.data();
    return;
  }
  // qDebug("开始导出投影");
  // cout << FileName << endl;

  ui->FinishExLite->setEnabled(false);
  ui->seeExported->setEnabled(false);
  ui->Build4Lite->setEnabled(false);

  this->proTracker = ui->ShowProgressExLite;

  if (putStructure)
    kernel->exportAsStructure(FileName.data(), failed_file_name);
  else if (putLitematic)
    kernel->exportAsLitematic(
        FileName.data(), ui->InputLiteName->text().toUtf8().data(),
        (ui->InputRegionName->text() + tr("(xz 坐标=-65±128×整数)"))
            .toUtf8()
            .data(),
        failed_file_name);

  else {
    int offset[3] = {0, 0, 0}, weOffset[3] = {0, 0, 0};
    QString dependModsListString = ui->schem_required_mods->toPlainText();
    QStringList modList = dependModsListString.split('\n');

    std::vector<std::string> stdStrList(modList.size());
    std::vector<const char *> charPtrs;
    for (int idx = 0; idx < int(stdStrList.size()); idx++) {
      stdStrList[idx] = modList[idx].toUtf8().data();
      charPtrs.emplace_back(stdStrList[idx].data());
    }

    const std::array<const QLineEdit *, 3> offsetSrc(
        {ui->schem_offsetX, ui->schem_offsetY, ui->schem_offsetZ});
    const std::array<const QLineEdit *, 3> weOffsetSrc(
        {ui->schem_weOffsetX, ui->schem_weOffsetY, ui->schem_weOffsetZ});

    for (int d = 0; d < 3; d++) {
      bool ok = true;
      int result = offsetSrc[d]->text().toInt(&ok);
      if (ok) offset[d] = result;

      result = weOffsetSrc[d]->text().toInt(&ok);
      if (ok) weOffset[d] = result;
    }

    kernel->exportAsWESchem(FileName.data(), offset, weOffset,
                            ui->schem_name->text().toUtf8().data(),
                            charPtrs.data(), charPtrs.size(), failed_file_name);
  }

  if (std::strlen(failed_file_name) <= 0) {
    // success
    // qDebug("压缩成功");
    ProductDir = QString::fromLocal8Bit(FileName.data());
    ProductDir = ProductDir.replace('\\', '/');
    ProductDir = ProductDir.left(ProductDir.lastIndexOf('/'));

    qDebug() << "ProductDir=" << ProductDir;

  } else {
    qDebug("Failed to export.");
    QMessageBox::warning(
        this, tr("投影文件导出失败"),
        tr("这可能是汉字编码错误造成的。请检查路径中是否有汉字") + '\n' +
            tr("错误信息：") + '\n' + failed_file_name);
    return;
  };

  ui->FinishExLite->setEnabled(true);
  ui->seeExported->setEnabled(true);
  ui->Build4Lite->setEnabled(true);

  updateEnables();
  this->proTracker = nullptr;
  // success
  // qDebug("导出为投影成功");
  return;
}

constexpr inline int ceil_to_128(int x) noexcept {
  const bool add_1 = (x % 128) > 0;
  x = x / 128;
  if (add_1) {
    x += 1;
  }
  return std::max(1, x);
}

int MainWindow::mapRows() const noexcept {
  return ceil_to_128(this->kernel->getImageRows());
}
int MainWindow::mapCols() const noexcept {
  return ceil_to_128(this->kernel->getImageCols());
}
int MainWindow::mapCount() const noexcept {
  return this->mapRows() * this->mapCols();
}

void MainWindow::on_InputDataIndex_textChanged() {
  bool isIndexValid = false;
  const int indexStart = ui->InputDataIndex->toPlainText().toInt(&isIndexValid);
  isIndexValid = isIndexValid && (indexStart >= 0);
  if (isIndexValid) {
    if (this->mapCount() == 1)
      ui->ShowDataFileName->setText(
          QStringLiteral("map_%1.dat").arg(indexStart));
    else
      ui->ShowDataFileName->setText(
          QStringLiteral("map_%1.dat ~ map_%2.dat")
              .arg(indexStart)
              .arg(indexStart + this->mapCount() - 1));
    ui->ExportData->setEnabled(true);
    return;
  }

  ui->ShowDataFileName->setText(
      tr("你输入的起始序号不可用，请输入大于等于 0 的整数！"));
  ui->ExportData->setEnabled(false);
  return;
}

void MainWindow::on_ExportData_clicked() { onExportDataclicked(""); }

std::vector<QString> MainWindow::existingMapDataFiles(
    QString prefix) const noexcept {
  std::vector<QString> ret;
  ret.reserve(this->mapCount());
  const int start_number = this->ui->InputDataIndex->toPlainText().toInt();
  if (prefix.back() == '/' || prefix.back() == '\\') {
  } else {
    prefix += '/';
  }

  for (int idx = start_number; idx < this->mapCount() + start_number; idx++) {
    QString filename = QStringLiteral("map_%1.dat").arg(idx);
    QString abs_filename = prefix + filename;
    if (!QFile(abs_filename).exists()) {
      continue;
    }
    ret.emplace_back(filename);
  }

  return ret;
}

void MainWindow::onExportDataclicked(QString path) {
  bool isIndexValid = false;
  const int indexStart = ui->InputDataIndex->toPlainText().toInt(&isIndexValid);
  isIndexValid = isIndexValid && (indexStart >= 0);
  if (!isIndexValid) {
    QMessageBox::information(this, tr("你输入的起始序号不可用"),
                             tr("请输入大于等于 0 的整数！"));
    return;
  }
  QString FolderPath;

  if (path.isEmpty())
    FolderPath = (QFileDialog::getExistingDirectory(
        this, tr("请选择导出的文件夹"), this->prevOpenedDir));
  else
    FolderPath = path;

  if (FolderPath.isEmpty()) {
    QMessageBox::information(this, tr("你选择的文件夹不存在！"),
                             tr("你可以选择存档中的 data 文件夹"));
    return;
  }
  this->prevOpenedDir = FolderPath;

  {
    auto files_to_be_covered = this->existingMapDataFiles(FolderPath);
    if (files_to_be_covered.size() > 0) {
      QString files{files_to_be_covered.front()};
      for (size_t idx = 1; idx < files_to_be_covered.size(); idx++) {
        files += ';' + files_to_be_covered[idx];
      }

      auto ret = QMessageBox::warning(
          this, tr("导出时将会覆盖部分地图文件"),
          tr("%1 "
             "个文件将被覆盖：\n%"
             "2\n\n点击\"Yes\"将继续并覆盖这些文件，点击\"No\"将撤销本次操作。")
              .arg(files_to_be_covered.size())
              .arg(files),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Yes,
                                       QMessageBox::StandardButton::No});
      if (ret == QMessageBox::StandardButton::No) {
        return;
      }
    }
  }

  ui->InputDataIndex->setEnabled(false);
  ui->ExportData->setEnabled(false);
  ui->FinshExData->setEnabled(false);
  ui->ExportData->setText(tr("请稍等"));

  proTracker = ui->ShowProgressExData;

  FolderPath = FolderPath.replace('\\', '/');
  ProductDir = FolderPath;

  kernel->exportAsData(FolderPath.toLocal8Bit().data(), indexStart,
                       //&fileCount,unCompressedBuffers.data()
                       nullptr, nullptr);
  // qDebug("导出地图文件成功");

  ui->InputDataIndex->setEnabled(true);
  ui->ExportData->setEnabled(true);
  ui->FinshExData->setEnabled(true);
  ui->ExportData->setText(tr("导出"));
  proTracker = nullptr;
  updateEnables();
}

void MainWindow::turnCh() { switchLan(Language::ZH); }

void MainWindow::turnEn() { switchLan(Language::EN); }

void MainWindow::switchLan(Language lang) {
  emit Manager->translate(lang);

  if (lang == Language::EN) {
    QString msg{""};

    if (!this->trans_SC.load(":/i18n/SlopeCraft_en_US.qm")) {
      msg += QStringLiteral("Failed to load translation file \"%1\"\n")
                 .arg(":/i18n/SlopeCraft_en_US.qm");
    }
    if (!this->trans_BLM.load(":/i18n/BlockListManager_en_US.qm")) {
      msg += QStringLiteral("Failed to load translation file \"%1\"\n")
                 .arg(":/i18n/BlockListManager_en_US.qm");
    }
    if (!this->trans_VD.load(":/i18n/VersionDialog_en_US.qm")) {
      msg += QStringLiteral("Failed to load translation file \"%1\"\n")
                 .arg(":/i18n/VersionDialog_en_US.qm");
    }

    if (!msg.isEmpty()) {
      QMessageBox::warning(this, "Failed to load translation.", msg);
      return;
    }

    qApp->installTranslator(&this->trans_SC);
    qApp->installTranslator(&this->trans_BLM);
    qApp->installTranslator(&this->trans_VD);
    ui->retranslateUi(this);
    // qDebug("Changed language to English");
  } else {
    qApp->removeTranslator(&this->trans_SC);
    qApp->removeTranslator(&this->trans_BLM);
    qApp->removeTranslator(&this->trans_VD);
    ui->retranslateUi(this);
    // qDebug("Changed language to Chinese");
  }
  return;
}

void MainWindow::on_allowGlassBridge_stateChanged(int arg1) {
  ui->glassBridgeInterval->setEnabled(arg1);
}

void MainWindow::showError(void *p, SlopeCraft::errorFlag error,
                           const char *msg) {
  MainWindow *wind = (MainWindow *)p;
  QString title, text;
  bool isFatal = false;

  const QString detail =
      (msg == nullptr) ? ("")
                       : (tr("\n具体信息：") + QString::fromStdString(msg));

  switch (error) {
    case SlopeCraft::errorFlag::NO_ERROR_OCCUR:
      return;
    case SlopeCraft::errorFlag::UNKNOWN_MAJOR_GAME_VERSION:
      title = tr("未知游戏版本");
      text = detail;
      break;
    case SlopeCraft::errorFlag::EXPORT_SCHEM_BLOCK_PALETTE_OVERFLOW:
      title = tr("导出原理图失败");
      text = tr("方块种类超出上限。") + detail;
      break;
    case SlopeCraft::errorFlag::EXPORT_SCHEM_WRONG_EXTENSION:
      title = tr("导出原理图失败");
      text = tr("错误的文件扩展名") + detail;
      break;
    case SlopeCraft::errorFlag::EXPORT_SCHEM_HAS_INVALID_BLOCKS:
      title = tr("导出原理图失败");
      text = tr("三维结构中存在错误的方块") + detail;
      break;
    case SlopeCraft::errorFlag::EXPORT_SCHEM_FAILED_TO_CREATE_FILE:
      title = tr("导出原理图失败");
      text = tr("无法创建/打开文件") + detail;
      break;
    case SlopeCraft::errorFlag::EXPORT_SCHEM_MC12_NOT_SUPPORTED:
      title = tr("导出 WorldEdit 原理图失败");
      text = tr("不支持导出 1.12 "
                "WorldEdit 原理图（.schematic 格式），仅支持.schem 格式") +
             detail;
      break;
    case SlopeCraft::errorFlag::EXPORT_SCHEM_STRUCTURE_REQUIRES_AIR:
      title = tr("导出原版结构方块文件失败");
      text = tr("导出时指定不使用结构空位表示空气，但方块列表中不包含空气。") +
             detail;
      break;

    case SlopeCraft::errorFlag::EMPTY_RAW_IMAGE:
      title = tr("转化原图为地图画时出错");
      text = tr("原图为空！你可能没有导入原图！");
      break;
    case SlopeCraft::errorFlag::DEPTH_3_IN_VANILLA_MAP:
      title = tr("构建高度矩阵时出现错误");
      text = tr(
          "原版地图画不允许出现第三个阴影（不存在的几何关系不可能生存实装！）\n"
          "请检查你的地图画类型，纯文件地图画不可以导出为投影！");
      break;
    case SlopeCraft::errorFlag::HASTY_MANIPULATION:
      title = tr("跳步操作");
      text = tr("SlopeCraft 不允许你跳步操作，请按照左侧竖边栏的顺序操作！");
      break;
    case SlopeCraft::errorFlag::LOSSYCOMPRESS_FAILED:
      title = tr("有损压缩失败");
      text =
          tr("在构建高度矩阵时，有损压缩失败，没能将地图画压缩到目标高度。 \
        这可能是因为地图画行数过大。 \
        尝试启用无损压缩，或者提高最大允许高度");
      break;
    case SlopeCraft::errorFlag::MAX_ALLOWED_HEIGHT_LESS_THAN_14:
      title = tr("最大允许高度太小了");
      text = tr("有损压缩的最大允许不要低于 14，否则很容易压缩失败");
      break;
    case SlopeCraft::errorFlag::USEABLE_COLOR_TOO_FEW:
      title = tr("允许使用的颜色过少");
      text = tr("你应该勾选启用尽可能多的基色，颜色太少是不行的！");
      break;
    case SlopeCraft::errorFlag::FAILED_TO_COMPRESS:
      title = tr("导出时 Gzip 压缩文件失败");
      text = tr("这可能是因为路径中含有中文字符！");
      break;
    case SlopeCraft::errorFlag::FAILED_TO_REMOVE:
      title = tr("删除临时文件失败");
      text = tr("这可能是因为路径中含有中文字符！");
      break;
  }
  if (isFatal)
    QMessageBox::warning(wind, title, text + detail,
                         QMessageBox::StandardButton::Ok,
                         QMessageBox::StandardButton::NoButton);
  else {
    QMessageBox::critical(wind, title, text + detail,
                          QMessageBox::StandardButton::Close);
    emit wind->ui->Exit->clicked();
  }
  wind->updateEnables();
  return;
}

void MainWindow::showWorkingStatue(void *p, SlopeCraft::workStatues statue) {
  MainWindow *wind = (MainWindow *)p;
  QString title = wind->windowTitle();
  const char spacer[] = "   |   ";
  if (title.contains(spacer)) {
    title = title.left(title.lastIndexOf(spacer));
  }

  if (statue != SlopeCraft::workStatues::none) title += spacer;

  switch (statue) {
    case SlopeCraft::workStatues::none:
      break;
    case SlopeCraft::workStatues::buidingHeighMap:
      title += tr("正在构建高度矩阵");
      break;
    case SlopeCraft::workStatues::building3D:
      title += tr("正在构建三维结构");
      break;
    case SlopeCraft::workStatues::collectingColors:
      title += tr("正在收集整张图片的颜色");
      break;
    case SlopeCraft::workStatues::compressing:
      title += tr("正在压缩立体地图画");
      break;
    case SlopeCraft::workStatues::constructingBridges:
      title += tr("正在为立体地图画搭桥");
      break;
    case SlopeCraft::workStatues::converting:
      title += tr("正在匹配颜色");
      break;
    case SlopeCraft::workStatues::dithering:
      title += tr("正在使用抖动仿色");
      break;
    case SlopeCraft::workStatues::flippingToWall:
      title += tr("正在将平板地图画变为墙面地图画");
      break;
    case SlopeCraft::workStatues::writing3D:
      title += tr("正在写入三维结构");
      break;
    case SlopeCraft::workStatues::writingBlockPalette:
      title += tr("正在写入方块列表");
      break;
    case SlopeCraft::workStatues::writingMapDataFiles:
      title += tr("正在写入地图数据文件");
      break;
    case SlopeCraft::workStatues::writingMetaInfo:
      title += tr("正在写入基础信息");
      break;
  }

  wind->setWindowTitle(title);
  return;
}

void MainWindow::on_seeExported_clicked() {
  if (ProductDir.isEmpty()) {
    return;
  }
  qDebug() << "ProductDir=" << ProductDir;
  QDesktopServices::openUrl(QUrl::fromLocalFile(ProductDir));
}

void MainWindow::on_AllowForcedOpti_stateChanged(int arg1) {
  ui->maxHeight->setEnabled(arg1);
}

void MainWindow::on_reportBugs_clicked() {
  QUrl url("https://github.com/SlopeCraft/SlopeCraft/issues/new/choose");
  QDesktopServices::openUrl(url);
}

void MainWindow::checkVersion() {
  // QtConcurrent::run(grabVersion,this);
  grabVersion(false);
  return;
}

void MainWindow::grabVersion(bool isAuto) {
  VersionDialog::start_network_request(
      this, "SlopeCraft",
      QUrl("https://api.github.com/repos/SlopeCraft/SlopeCraft/releases"),
      networkManager(), !isAuto);
}

void MainWindow::onBlockListChanged() {
  // qDebug("onBlockListChanged");
  if (kernel->queryStep() < SlopeCraft::step::nothing) {
    return;
  }

  this->kernelSetType();
  if (!this->rawPic.isNull()) {
    this->kernelSetImg();
  }
  this->updateEnables();

  ushort colorCount = kernel->getColorCount();
  ui->IntroColorCount->setText(tr("可用") + QString::number(colorCount) +
                               tr("种颜色"));
}

void MainWindow::closeEvent(QCloseEvent *event) {
  emit closed();
  qDebug("closed Signal emitted");
  QMainWindow::closeEvent(event);
  exit(0);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key::Key_F5: {
      QString destName = QFileDialog::getSaveFileName(
          this, tr("保存截屏"), "",
          tr("图片 (*.jpg *.jpeg *.tif *.bmp *.png)"));

      if (destName.isEmpty()) {
        break;
      }

      QPixmap pix = this->grab();
      pix.save(destName);
      break;
    }
    default:
      break;
  }

  QMainWindow::keyPressEvent(event);
}

void MainWindow::on_ExImage_clicked() {
  QPixmap image = ui->ShowPic->pixmap();

  if (image.isNull()) {
    return;
  }

  QString savePath = QFileDialog::getSaveFileName(this, tr("保存当前显示图片"),
                                                  "./", tr("图片 (*.png)"));

  if (savePath.isEmpty()) {
    return;
  }

  image.save(savePath);
}

void MainWindow::selectBlockByString(const std::string &key) {
  std::vector<const TokiBaseColor *> tbcs;
  Manager->getTokiBaseColors(&tbcs);

  for (uint8_t baseColor = 0; baseColor < tbcs.size(); baseColor++) {
    std::vector<const TokiBlock *> tbs;
    tbcs[baseColor]->getTokiBlockList(tbs);
    for (uint16_t idx = 0; idx < tbs.size(); idx++) {
      std::string curBlockId = tbs[idx]->getSimpleBlock()->getId();
      if (curBlockId.find(key) != std::string::npos) {
        Manager->setSelected(baseColor, idx);
        continue;
      }
    }
  }
}

void MainWindow::onActionSavePreset() {
  QString dst = QFileDialog::getSaveFileName(
      this, tr("保存预设"), "",
      QStringLiteral("*") + MainWindow::sc_preset_extension);
  if (dst.isEmpty()) return;

  auto preset = this->Manager->currentPreset();

  QString content = serialize_preset(preset);

  QFile ofile(dst);
  if (!ofile.open(QFile::OpenMode::enum_type::WriteOnly)) {
    QMessageBox::warning(this, tr("保存预设文件失败"),
                         tr("无法创建文件\"%1\"").arg(dst));
    return;
  }

  ofile.write(content.toUtf8());
  ofile.close();
}

void MainWindow::onActionLoadPreset() {
  QString src = QFileDialog::getOpenFileName(
      this, tr("选择预设文件"), this->prevOpenedDir,
      QStringLiteral("*") + MainWindow::sc_preset_extension);
  if (src.isEmpty()) return;

  this->prevOpenedDir = QFileInfo(src).filePath();
  QString err;
  auto preset = load_preset(src, err);
  if (!err.isEmpty()) {
    QMessageBox::warning(this, tr("加载预设失败"), err);
    return;
  }

  this->Manager->loadPreset(preset);
}

void MainWindow::onActionAiCvterParameters() {
  auto acpDialog = new AiCvterParameterDialog(this);
  acpDialog->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  acpDialog->show();
}

void MainWindow::on_FirstConcrete_clicked() { selectBlockByString("concrete"); }

void MainWindow::on_FirstWool_clicked() { selectBlockByString("wool"); }

void MainWindow::on_FirstStainedGlass_clicked() {
  selectBlockByString("stained_glass");
}

void MainWindow::testBlockList() {
  std::vector<const SlopeCraft::AbstractBlock *> ptr_buffer;
  std::vector<uchar> base_buffer;

  const int buffSize = Manager->getBlockNum();
  ptr_buffer.resize(buffSize + 1);
  base_buffer.resize(buffSize + 1);

  Manager->getBlockPtrs(ptr_buffer.data(), base_buffer.data());

  QString targetName = QFileDialog::getSaveFileName(
      this, tr("测试方块列表的结构文件"), "", "*.nbt");
  if (targetName.isEmpty()) {
    return;
  }

  char failed_file_name[512] = "";
  kernel->makeTests(ptr_buffer.data(), base_buffer.data(),
                    targetName.toLocal8Bit().data(), failed_file_name);
  if (std::string_view(failed_file_name).empty()) {
    // cerr<<"Success"<<endl;
    return;
  } else {
    QMessageBox::warning(this, tr("测试方块列表失败"),
                         tr("具体信息：") + failed_file_name);
  }

  // cerr << "Compress success\n";
}

void MainWindow::on_ExLite_clicked() {
  ui->tabExport3DInfo->setCurrentIndex(0);
}

void MainWindow::on_ExStructure_clicked() {
  ui->tabExport3DInfo->setCurrentIndex(1);
}

void MainWindow::on_ExWESchem_clicked() {
  ui->tabExport3DInfo->setCurrentIndex(2);
}

void MainWindow::exportAvailableColors() {
  constexpr int basecolors_per_row = 4;
  constexpr int basecolors_per_col = 16;

  static_assert(basecolors_per_row * basecolors_per_col == 64);

  constexpr int row_pixels = basecolors_per_row * 4;
  constexpr int col_pixels = basecolors_per_col * 1;

  static_assert(row_pixels * col_pixels == 256);

  const QString dest_file =
      QFileDialog::getSaveFileName(this, tr("保存颜色表"), "", "*.png");

  if (dest_file.isEmpty()) {
    return;
  }

  QImage img(row_pixels, col_pixels, QImage::Format::Format_ARGB32);

  if (img.isNull()) {
    QMessageBox::warning(this, tr("保存颜色表失败"), tr("分配内存失败"));
    return;
  }

  img.fill(0x00FFFFFFU);

  Eigen::Map<
      Eigen::Array<uint32_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
      map(reinterpret_cast<uint32_t *>(img.scanLine(0)), row_pixels,
          col_pixels);

  uint32_t argb_colors[256];
  uint8_t map_colors[256];
  const int available_colors = kernel->getColorCount();
  kernel->getAvailableColors(argb_colors, map_colors);

  for (int cidx = 0; cidx < available_colors; cidx++) {
    /*
    const int basecolor = (map_colors[cidx] / 4);
    const int shade = (map_colors[cidx] % 4);
    const int pixel_row = basecolor / basecolors_per_col;
    const int pixel_col = (basecolor % basecolors_per_col) * 4 + shade;
    */
    map(map_colors[cidx]) = argb_colors[cidx];
  }

  if (img.save(dest_file)) {
    this->ProductDir = dest_file;
  }
  return;
}

QNetworkAccessManager &MainWindow::networkManager() noexcept {
  static QNetworkAccessManager manager;
  return manager;
}