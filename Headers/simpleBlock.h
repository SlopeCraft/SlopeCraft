#ifndef SIMPLEBLOCK_H
#define SIMPLEBLOCK_H

#include <string>
typedef unsigned char uchar ;
class simpleBlock
{
public:
    simpleBlock();
    std::string id;
    uchar version;
    std::string idOld;
    bool needGlass;
    bool doGlow;
};

#endif // SIMPLEBLOCK_H
