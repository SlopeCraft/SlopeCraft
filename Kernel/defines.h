#ifndef DEFINES_H
#define DEFINES_H

#define EIGEN_NO_DEBUG
#include <Eigen/Dense>
#define WITH_QT

#ifndef WITH_QT
    #define removeQt
    #define emit ;
#endif



typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef uint ARGB;
typedef Eigen::Array<uint,Eigen::Dynamic,Eigen::Dynamic> EImage;
#endif // DEFINES_H
