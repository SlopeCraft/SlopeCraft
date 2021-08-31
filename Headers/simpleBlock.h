#ifndef SIMPLEBLOCK_H
#define SIMPLEBLOCK_H

#include <string>
#include <vector>
#include <iostream>
typedef unsigned char uchar ;
typedef std::vector<std::string> stringList;
class simpleBlock
{
public:
    simpleBlock();
    std::string id;
    uchar version;
    std::string idOld;
    bool needGlass;
    bool doGlow;
    static bool dealBlockId(const std::string & id ,
                            std::string & netBlockId,
                            stringList * proName,
                            stringList * proVal);
};

#endif // SIMPLEBLOCK_H
