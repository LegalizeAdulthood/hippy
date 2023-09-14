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
#ifndef XML_parser_H__
#define XML_parser_H__

#include <afx.h>
#include <afxwin.h>
#include <stdio.h>
#include <string.h>
#include "hippy.h"
#include "device.h"
#include <afxtempl.h>

typedef CArray<CDevice*, CDevice*&> CDeviceArray;

typedef enum{
	ttHdf,
	ttDevice,
	ttWordSelect,
	ttChipSelect,
	ttWordSelectCld,
	ttChipSelectCld,
	ttAI,
	ttCS
} XmlTagType;

typedef struct AddrResEntry{
	Word decodedAddr;
	BYTE devIndex;
}AddrResEntry;

typedef struct XmlTag{
	XmlTagType	type;
	double		version;
	int			id;
	CString		szData;
	CString		szName;
	CString		szChip;
	bool		close;
} XmlTag, *PXmlTag;

class CDeviceFile{
private:
	FILE * 		file;
	bool		bOpened;
	XmlTag		xmlTag;
	char		GetNextToken();

	bool	EvalEqn(Word addr, CString & eqn);
public:
	bool	CheckCSEquation(CString eqn, Word wAddr);
	void	PutCharBack();
	bool	FindChar(char c);
	bool	FindPattern(LPSTR lpStr);
	char	GetString(CString & szString);
	bool	GetField(LPCSTR lpcField, CString & szVal);
	bool	GetUntilChar(const char chr, CString & szStr);
	bool	FindNextTag();
	bool	CompileData();
	int		ParseFile(CWnd * parent,CString szFileName, CDeviceArray & devArr, AddrResEntry * AddrResTbl);
};

#endif