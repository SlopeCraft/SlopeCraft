#include <QApplication>
#include "SCWind.h"
int main(int argc, char** argv) {
  QApplication qapp(argc, argv);

  SCWind wind;

  wind.show();

  return qapp.exec();
}