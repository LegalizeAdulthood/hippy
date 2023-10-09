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

#ifndef HIPPY_DEVICE_H
#define HIPPY_DEVICE_H

#include "hippy.h"

#include <afxwin.h>

#include <wx/wx.h>

class CSemaphore;

enum TInterrupt
{
    IRQ,
    NMI,
    RESET
};

class CDevice
{
public:
    CDevice();
    virtual ~CDevice();

    int  Create(CWnd *parentWnd, const wxString &name, const wxString &libName);
    void Interrupt(TInterrupt tint);
    bool Read(Word addr, BYTE &val, bool debug);
    bool Write(Word addr, BYTE val, bool debug);

    void GetDeviceName(wxString &value) const
    {
        value = m_deviceName;
    }
    void GetLibraryName(wxString &value) const
    {
        value = m_libraryName;
    }

protected:
    CWnd   *m_parentWnd{};
    bool    m_debug{};
    wxString m_deviceName{};
    wxString m_libraryName{};

    virtual void Reset()
    {
    }
    virtual void OnInitialize()
    {
    }
    virtual void OnFinalize()
    {
    }
    virtual BYTE OnRead(Word addr)
    {
        return 0;
    }
    virtual void OnWrite(Word addr, BYTE wVal)
    {
    }

private:
    CSemaphore *m_irq;
    CSemaphore *m_nmi;
    CSemaphore *m_reset;
};

using pdevFunctv = CDevice *(*) ();

#endif
