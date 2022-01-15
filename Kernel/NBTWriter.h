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


//Made by TokiNoBug

#ifndef _NBTWRITER_H
#define _NBTWRITER_H

#pragma once

#include <iostream>
#include <fstream>


#include <cstring>
#include <stack>
#include "defines.h"
//using namespace std;
#define TwinStackSize 512
namespace NBT{

    using tagType_t = char;
    const tagType_t idEnd=0;
    const tagType_t idByte=1;
    const tagType_t idShort=2;
    const tagType_t idInt=3;
    const tagType_t idLong=4;
    const tagType_t idFloat=5;
    const tagType_t idDouble=6;
    const tagType_t idByteArray=7;
    const tagType_t idString=8;
    const tagType_t idList=9;
    const tagType_t idCompound=10;
    const tagType_t idIntArray=11;
    const tagType_t idLongArray=12;


    template <typename T>
    void IE2BE(T &Val);

    template <typename T>
    T IE2BE(T *Val);

    bool isSysBE();

class NBTWriter
{
public:
	private:
    using stackUnit = std::pair<tagType_t,int>;
		//Vars
		bool isOpen;
		bool isBE;
        std::fstream *File;
        unsigned long long ByteCount;
        std::stack<stackUnit> stack;
		//StackFun
		void pop();
        void push(const stackUnit &&);
		bool isEmpty();
		bool isFull();
        tagType_t readType();
		//WriterFun
		void elementWritten();
		void endList();
		int writeEnd();
        bool typeMatch(tagType_t typeId);
		//AutoFiller
		int emergencyFill();
	public:
		//Construct&deConstruct
		NBTWriter(const char*path);
		~NBTWriter();
        NBTWriter();
        void open(const char*path);
		//Vars
		bool allowEmergencyFill;
		//WriterFun


        int writeLongDirectly(const char*Name,long long value);

		bool isInList();
		bool isInCompound();
		unsigned long long close();
		bool isListFinished();
		char CurrentType();
		//WriteAbstractTags
		template <typename T>
        int writeSingleTag(tagType_t typeId,const char*Name,T value);

		//int writeArrayHead(char typeId,const char*Name,int arraySize);
		//WriteSpecialTags
		int writeCompound(const char*Name);
		int writeListHead(const char*Name,char typeId,int listSize);
		int endCompound();
		int writeString(const char*Name,const char*value);
		//WriteRealSingleTags
		int writeByte(const char*Name,char value);
		int writeShort(const char*Name,short value);
		int writeInt(const char*Name,int value);
		int writeLong(const char*Name,long long value);
		int writeFloat(const char*Name,float value);
		int writeDouble(const char*Name,double value);
		//WriteArrayHeads
		int writeLongArrayHead(const char*Name,int arraySize);
		int writeByteArrayHead(const char*Name,int arraySize);
		int writeIntArrayHead(const char*Name,int arraySize);
        unsigned long long getByteCount();
};



//NameSpace NBT ends here
}


#endif
