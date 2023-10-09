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
#ifndef HIPPY_ADDRMNG_H
#define HIPPY_ADDRMNG_H

#include "hippy.h"
#include "xmlparser.h"

#include <wx/wx.h>

#include <vector>

class CEnvironment;

class CAddressManager
{
public:
    CAddressManager() = default;
    ~CAddressManager();

    void Create(CEnvironment *pEnv);
    bool LoadFile(const wxString &fname, std::vector<Word> &adr_arr);
    int  SaveSFile(const wxString &fileName, Word wBegin, Word wEnd);

    Word GetLastWrite() const
    {
        return m_wLastWrite;
    }
    Word GetLastRead() const
    {
        return m_wLastRead;
    }
    BYTE Read(Word wIndex, bool bDbg = false);
    void Write(Word wIndex, BYTE bVal);

private:
    AddrResEntry  m_AddrResTbl[0x10000]{};
    CEnvironment *m_env{};
    CDeviceArray  m_devices;
    BYTE          m_memory[0x10000]{};
    Word          m_wLastWrite{};
    Word          m_wLastRead{};
};

#endif
