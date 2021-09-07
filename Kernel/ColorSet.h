#ifndef COLORSET_H
#define COLORSET_H
#include <Eigen/Dense>
#include <QDebug>
#include "TokiColor.h"
using namespace Eigen;
class ColorSet{
    public:
        ArrayXXf _RGB;
        ArrayXXf  HSV;
        ArrayXXf  Lab;
        ArrayXXf  XYZ;
        VectorXi  Map;
        ColorSet();
        void ApplyAllowed(ColorSet*standard,bool *MIndex);
};

void GetMap(unsigned char *Map);

void GetMap(VectorXi &Map);

void f(float &);
void invf(float&);
void RGB2HSV(float, float, float,  float &, float &, float &);
void HSV2RGB(float,float,float,float&,float&,float&);
void RGB2XYZ(float R, float G, float B, float &X, float &Y, float &Z);
void XYZ2Lab(float X, float Y, float Z, float &L, float &a, float &b);
void Lab2XYZ(float L,float a,float b,float&X,float&Y,float&Z);

float Lab00(float,float,float,float,float,float);

float squeeze01(float);
QRgb RGB2QRGB(float,float,float);
QRgb HSV2QRGB(float,float,float);
QRgb XYZ2QRGB(float,float,float);
QRgb Lab2QRGB(float,float,float);
QRgb ComposeColor(const QRgb&front,const QRgb&back);
bool readFromFile(const char*FileName,ArrayXXf & M);
bool readFromTokiColor(const char*,ArrayXXf &,const string &);
uchar h2d(char);
#endif // COLORSET_H
