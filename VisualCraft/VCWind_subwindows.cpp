#include "BlockBrowser.h"
#include "VCWind.h"
#include "ui_VCWind.h"

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
