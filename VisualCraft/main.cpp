#include <QApplication>

#include "CallbackFunctions.h"
#include "VCWind.h"

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);

  VCWind wind;

  VC_callback::wind = &wind;

  VCL_set_report_callback(VC_callback::callback_receive_report);

  wind.show();
  return qapp.exec();
}