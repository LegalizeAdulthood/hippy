//
// Copyright (c) Yeditepe University Computer Engineering Department, 2005.
// This file is part of HIPPY
//
// Hippy is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Hippy is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Hippy; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
#ifndef _TABLES_H__
#define _TABLES_H__

#include <memory.h>
#include <malloc.h>
#include <string.h>
#include "codes.h"
#include <assert.h>

typedef enum{
	REL,
	IMMED,
	DIRECT,
	INDEXED,
	EXTEND,
	INHER
} INST_MODE;

#ifndef BYTE
typedef unsigned char BYTE;
#endif

typedef char *			LPSTR;
typedef const char *	LPCSTR;
typedef unsigned short Word;

typedef enum{
	IMMEDIATE, ADDRESS
} NumberMode;

/*
 * String Table
 */

class CStringTable{
private:
	char	**	tree[256];
	int			listSize[256];
public:
	CStringTable();
	~CStringTable();
	LPSTR	AddString(BYTE hash, LPCSTR lpcStr);
	LPSTR	Find(BYTE hash, LPCSTR lpcStr);
};


typedef struct Symbol{
	LPSTR	name;
	Word	addr;
	BYTE	type;
} Symbol, * PSymbol;

/*
 * Symbol Table
 */

class CSymbolTable{
private:
	PSymbol	*	tree[256];
	int			listSize[256];
public:
	CSymbolTable();
	~CSymbolTable();
	bool	Insert(BYTE hash, PSymbol pSym);
	bool	Find(BYTE hash, PSymbol pSym);
};

typedef struct {
	Word	addr;
	Byte	val;
}MEMBYTE, *PMEMBYTE;

/*
 * Lookup Table
 */

typedef struct LookupEnt{
	LPSTR		name;
	PMEMBYTE	pbMem;
	INST_MODE	instmode;
	Byte		hash;
	Word		line;
	Word		pc;
} LookupEnt, * PLookupEnt;

class CLookupTable{
private:
	PLookupEnt list;
	int size;
public:
	CLookupTable(){
		size=0;
	}

	~CLookupTable(){
		free(list);
	}
	
	void Insert(PLookupEnt ple){
		size++;
		list = (PLookupEnt)realloc(list, sizeof(LookupEnt)*size);
		list[size-1] = *ple;
	}

	int GetSize(){
		return size;
	}

	void GetEntry(int ind, PLookupEnt ple){
		*ple = list[ind];
	}

};

#endif