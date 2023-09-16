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

#include "device.h"

CDevice::CDevice()
{
    psem_irq = new CSemaphore(0, 1000, INT_IRQ);
    psem_nmi = new CSemaphore(0, 1000, INT_NMI);
    psem_reset = new CSemaphore(0, 1000, INT_RST);
    Reset();
}

CDevice::~CDevice()
{
    delete psem_irq;
    delete psem_nmi;
    delete psem_reset;
    OnFinalize();
}

int CDevice::Create(CWnd *parentWnd, CString szName)
{
    char buffer[1024];
    m_pParentWnd = parentWnd;
    lpszDeviceName = new CString(szName);
    GetModuleFileName(AfxGetInstanceHandle(), buffer, 1024);
    lpszLibraryName = new CString(buffer);
    OnInitialize();
    return 0;
}

void CDevice::Interrupt(TInterrupt tint)
{
    switch (tint)
    {
    case IRQ:
        psem_irq->Unlock();
        break;
    case NMI:
        psem_nmi->Unlock();
        break;
    case RESET:
        psem_reset->Unlock();
        break;
    }
}

// reads the specifies address from the chip and returns the value in val.
// if the chip is not selected with the address specified returns false.
// in this case the val remains untouched.
bool CDevice::Read(Word addr, BYTE &val, bool bDbg)
{
    this->bDbg = bDbg;
    val = OnRead(addr);
    return true;
}

// write the specifies address to the chip if the chip is not selected with
// the address specified returns false.
bool CDevice::Write(Word addr, BYTE val, bool bDbg)
{
    this->bDbg = bDbg;
    OnWrite(addr, val);
    return true;
}