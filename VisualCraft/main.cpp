#include <QApplication>

#include "VCWind.h"

int main(int argc, char **argv) {
  QApplication qapp(argc, argv);

  VCWind wind;
  wind.show();
  return qapp.exec();
}