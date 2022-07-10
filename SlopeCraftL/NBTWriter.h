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
#include "SCLDefines.h"
//using namespace std;
#define TwinStackSize 128
namespace NBT{
	const char idEnd=0;
	const char idByte=1;
	const char idShort=2;
	const char idInt=3;
	const char idLong=4;
	const char idFloat=5;
	const char idDouble=6;
	const char idByteArray=7;
	const char idString=8;
	const char idList=9;
	const char idCompound=10;
	const char idIntArray=11;
	const char idLongArray=12;

    template <typename T>
    void IE2BE(T &Val);

    template <typename T>
    T IE2BE(T *Val);

    bool isSysBE();

class NBTWriter
{
	private:
		//Vars
		bool isOpen;
		bool isBE;
        std::fstream *File;
		unsigned long long ByteCount;
		short top;
		char CLA[TwinStackSize];
		int Size[TwinStackSize];
		//StackFun
		void pop();
		void push(char typeId,int size);
		bool isEmpty();
		bool isFull();
		char readType();
		char readSize();
		//WriterFun
		void elementWritten();
		void endList();
		int writeEnd();
		bool typeMatch(char typeId);
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
		int writeSingleTag(char typeId,const char*Name,T value);

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
