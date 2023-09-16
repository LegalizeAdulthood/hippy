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
#ifndef _ENVIRONMENT_H__
#define _ENVIRONMENT_H__

#include <afxwin.h>

class CEnvironment
{
private:
    void   *m_pMainWnd;
    void   *m_pDebugWnd;
    CString devicefile;

public:
    void SetMainWnd(void *pWnd)
    {
        m_pMainWnd = pWnd;
    }
    void *GetMainWnd()
    {
        return m_pMainWnd;
    }
    void SetDebugWnd(void *pDbg)
    {
        m_pDebugWnd = pDbg;
    }
    void *GetDebugWnd()
    {
        return m_pDebugWnd;
    }
    void SetDeviceFile(CString str)
    {
        devicefile = str;
    }
    CString GetDeviceFile()
    {
        return devicefile;
    }
};

#endif