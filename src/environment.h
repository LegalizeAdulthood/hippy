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
#ifndef HIPPY_ENVIRONMENT_H
#define HIPPY_ENVIRONMENT_H

#include <wx/wx.h>

class CWnd;

class CEnvironment
{
public:
    void SetMainWnd(CWnd *pWnd)
    {
        m_mainWnd = pWnd;
    }
    CWnd *GetMainWnd() const
    {
        return m_mainWnd;
    }
    void SetDeviceFile(const wxString &str)
    {
        m_deviceFile = str;
    }
    const wxString &GetDeviceFile() const
    {
        return m_deviceFile;
    }

private:
    CWnd    *m_mainWnd{};
    wxString m_deviceFile;
};

#endif
