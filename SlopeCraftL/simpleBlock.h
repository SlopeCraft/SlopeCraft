/*
 Copyright © 2021-2022  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef SIMPLEBLOCK_H
#define SIMPLEBLOCK_H

#include "defines.h"
using namespace SlopeCraft;
#include <string>
#include <vector>
#include <iostream>

typedef unsigned char uchar ;

typedef std::vector<std::string> stringList;

namespace SlopeCraft {
#ifdef SLOPECRAFTL_CAPI
struct AbstractBlock
{
public:
    AbstractBlock();
    //virtual ~AbstractBlock() {};
    ///create a block
    static AbstractBlock * create();
    ///real size of this block
    virtual unsigned long long size()const=0;
    ///id of this block
    virtual const char* getId()const=0;
    ///first version
    virtual unsigned char getVersion()const=0;
    ///id in 1.12
    virtual const char* getIdOld()const=0;
    ///if this block needs a glass block under it
    virtual bool getNeedGlass()const=0;
    ///if this block emits light
    virtual bool getDoGlow()const=0;
    ///if this block can be stolen by enderman
    virtual bool getEndermanPickable()const=0;
    ///if this block can be burnt
    virtual bool getBurnable()const=0;
    ///if this block can be used in wall-map
    virtual bool getWallUseable()const=0;

    ///set block id
    virtual void setId(const char*)=0;
    ///set first version
    virtual void setVersion(unsigned char)=0;
    ///set id in 1.12
    virtual void setIdOld(const char*)=0;
    ///set if this block needs a glass block under it
    virtual void setNeedGlass(bool)=0;
    ///set if this block emits light
    virtual void setDoGlow(bool)=0;
    ///set if this block can be stolen by enderman
    virtual void setEndermanPickable(bool)=0;
    ///set if this block can be burnt
    virtual void setBurnable(bool)=0;
    ///set if this block can be used in wall-map
    virtual void setWallUseable(bool)=0;
    ///let *b equal to *this
    void copyTo(AbstractBlock * b) const;
    ///set this block to air
    void clear();
    ///replacement for operator delete
    virtual void destroy()=0;
};
#endif  //  ifndef SLOPECRAFT_CAPI
}

class simpleBlock : public AbstractBlock
{
public:
    simpleBlock();
    virtual ~simpleBlock(){};
    std::string id;
    uchar version;
    std::string idOld;
    bool needGlass;
    bool doGlow;
    bool endermanPickable;
    bool burnable;
    bool wallUseable;

    unsigned long long size() const {
        return sizeof(simpleBlock);
    }

    const char* getId() const {
        return id.data();
    };
    unsigned char getVersion() const {
        return version;
    };
    const char* getIdOld() const {
        return idOld.data();
    };
    bool getNeedGlass() const {
        return needGlass;
    };
    bool getDoGlow() const {
        return doGlow;
    };
    bool getEndermanPickable() const {
        return endermanPickable;
    };
    bool getBurnable() const {
        return burnable;
    };
    bool getWallUseable() const {
        return wallUseable;
    };

    void setId(const char* _id) {
        id=_id;
    };
    void setVersion(unsigned char _ver) {
        version=_ver;
    };
    void setIdOld(const char* _idOld) {
        idOld=_idOld;
    };
    void setNeedGlass(bool _needGlass) {
        needGlass=_needGlass;
    };
    void setDoGlow(bool _doGlow) {
        doGlow=_doGlow;
    };
    void setEndermanPickable(bool _enderman) {
        endermanPickable=_enderman;
    };
    void setBurnable(bool _burn) {
        burnable=_burn;
    };
    void setWallUseable(bool _wall) {
        wallUseable=_wall;
    };

    void destroy() {
        delete this;
    }

    static bool dealBlockId(const std::string & id ,
                            std::string & netBlockId,
                            stringList * proName,
                            stringList * proVal);
    //simpleBlock& operator =(const simpleBlock &);
};

#endif // SIMPLEBLOCK_H
