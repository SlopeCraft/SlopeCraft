#ifndef SLOPECRAFT_VISUALCRAFT_CALLBACKFUNCTIONS_H
#define SLOPECRAFT_VISUALCRAFT_CALLBACKFUNCTIONS_H

#include <VisualCraftL.h>

class QWidget;
namespace VC_callback {
void callback_receive_report(VCL_report_type_t, const char *msg,
                             bool flush) noexcept;

extern QWidget *wind;
} // namespace VC_callback

#endif // SLOPECRAFT_VISUALCRAFT_CALLBACKFUNCTIONS_H