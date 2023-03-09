#include "BiomeBrowser.h"
#include "BlockBrowser.h"
#include "BlockSelector.h"
#include "VCWind.h"
#include "ui_VCWind.h"
#include <QDesktopServices>
#include <QMessageBox>

void VCWind::on_ac_browse_block_triggered() noexcept {
  BlockBrowser *bb = new BlockBrowser(this);

  bb->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  bb->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  bb->setWindowFlag(Qt::WindowType::Window, true);

  connect(this, &VCWind::signal_basic_colorset_changed, bb,
          &QWidget::deleteLater);

  bb->show();
}

void VCWind::on_ac_about_VisualCraft_triggered() noexcept {
  QMessageBox::information(
      this, VCWind::tr("关于VisualCraft"),
      VCWind::tr("VisualCraft是一款全新的Minecraft像素画生"
                 "成器，由MC玩家TokiNoBug开发，是SlopeCraft的子项目。与其他类似"
                 "的软件不同，VisualCraft旨在跟进最新的MC版本(1.12~最新版)"
                 "、支持最多的MC特性"
                 "，提供最强大的功能。\n\n") +
          VCWind::tr("目前VisualCraft能够解析许多第三方资源包，也允许自定义增加"
                     "加新方块。与传统的思路不同，VisualCraft以方块模型的方式来"
                     "解析资源包，尽量贴近Minecraft的方式，因此支持各种自定义的"
                     "方块模型。\n\n") +
          VCWind::tr("在导出方面，VisualCraft支持Litematica "
                     "mod的投影(*.litematic)、WorldEdit "
                     "原理图(*.shem)(仅1.13+可用)、原版结构方块文件(*.nbt)"
                     "、平面示意图(*.png)等方式。\n\n") +
          VCWind::tr("VisualCraft支持用各种透明方块互相叠加，产生更多的颜色。软"
                     "件最多支持不超过65534种颜色，受此限制，像素画的层数不超过"
                     "3层。\n\n") +
          (VCWind::tr(
               "由于颜色数量很多，VisualCraft使用了显卡加速。目前支持的AP"
               "I有OpenCL。现在正在使用的API是%1")
               .arg(VCL_get_GPU_api_name())));
}

void VCWind::on_ac_browse_biome_triggered() noexcept {
  BiomeBrowser *bb = new BiomeBrowser(this);

  bb->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  bb->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  bb->setWindowFlag(Qt::WindowType::Window, true);

  connect(this, &VCWind::signal_basic_colorset_changed, bb,
          &QWidget::deleteLater);

  bb->show();
}

void VCWind::on_ac_contact_bilibili_triggered() noexcept {
  QDesktopServices::openUrl(QUrl("https://space.bilibili.com/351429231"));
}
void VCWind::on_ac_contact_github_repo_triggered() noexcept {
  QDesktopServices::openUrl(QUrl("https://github.com/ToKiNoBug/SlopeCraft"));
}
void VCWind::on_ac_report_bugs_triggered() noexcept {
  QDesktopServices::openUrl(
      QUrl("https://github.com/ToKiNoBug/SlopeCraft/issues/new/choose"));
}

void VCWind::on_pb_custom_select_clicked() noexcept {

  BlockSelector *bs = new BlockSelector(this);

  bs->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose, true);
  bs->setAttribute(Qt::WidgetAttribute::WA_AlwaysStackOnTop, true);
  // bb->setAttribute(Qt::WidgetAttribute::WA_NativeWindow, true);
  bs->setWindowFlag(Qt::WindowType::Window, true);

  bs->show();
}