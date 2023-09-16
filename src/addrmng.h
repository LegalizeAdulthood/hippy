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
#ifndef _ADDRMNG__H__
#define _ADDRMNG__H__

#include "disassembler.h"
#include "environment.h"
#include "hippy.h"
#include "xmlparser.h"
#include <afx.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

class CAddressManager
{
protected:
private:
    AddrResEntry  AddrResTbl[0x10000];
    CEnvironment *pEnv;
    CDeviceFile   xp;
    CDeviceArray  devices;
    BYTE          memory[0x10000];
    Word          wLastWrite;
    Word          wLastRead;
    CDevice      *GetDeviceFromAddr(Word addr);

public:
    CAddressManager();
    ~CAddressManager();

    void Create(CEnvironment *pEnv);
    bool LoadFile(char *fname, CArray<Word, Word &> &adr_arr);
    int  SaveSFile(CString str, Word wBegin, Word wEnd);

    Word GetLastWrite()
    {
        return wLastWrite;
    }
    Word GetLastRead()
    {
        return wLastRead;
    }
    BYTE Read(Word wIndex, bool bDbg = false);
    void Write(Word wIndex, BYTE bVal);
};

#endif