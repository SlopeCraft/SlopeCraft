#ifndef DEFINES_H
#define DEFINES_H

#define EIGEN_NO_DEBUG
#include <Eigen/Dense>

#include "SlopeCraftL.h"

#ifndef WITH_QT
    #define removeQt
    #define emit ;
#endif

#ifdef STRAIGHT_INCLUDE_KERNEL
#ifndef NO_DLL
#error YOU MUST DEFINE NO_DLL WHEN DEFINED STRAIGHT_INCLUDE_KERNEL
#endif
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned int ARGB;
typedef Eigen::Array<ARGB,Eigen::Dynamic,Eigen::Dynamic> EImage;
typedef Eigen::Array<uint8_t,Eigen::Dynamic,1,Eigen::ColMajor,256> MapList;
typedef Eigen::Array<float,Eigen::Dynamic,3,Eigen::ColMajor,256> ColorList;
typedef Eigen::Array<float,Eigen::Dynamic,1,Eigen::ColMajor,256> TempVectorXf;
#endif // DEFINES_H
