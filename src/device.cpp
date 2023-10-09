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

#include <afxmt.h>

CDevice::CDevice() :
    m_irq(new CSemaphore(0, 1000, INT_IRQ)),
    m_nmi(new CSemaphore(0, 1000, INT_NMI)),
    m_reset(new CSemaphore(0, 1000, INT_RST))
{
    CDevice::Reset();
}

CDevice::~CDevice()
{
    delete m_irq;
    delete m_nmi;
    delete m_reset;
    CDevice::OnFinalize();
}

int CDevice::Create(CWnd *parentWnd, const CString &szName, const CString &libName)
{
    m_parentWnd = parentWnd;
    m_deviceName = szName;
    m_libraryName = libName;
    OnInitialize();
    return 0;
}

void CDevice::Interrupt(TInterrupt tint)
{
    switch (tint)
    {
    case IRQ:
        m_irq->Unlock();
        break;
    case NMI:
        m_nmi->Unlock();
        break;
    case RESET:
        m_reset->Unlock();
        break;
    }
}

// reads the specifies address from the chip and returns the value in val.
// if the chip is not selected with the address specified returns false.
// in this case the val remains untouched.
bool CDevice::Read(Word addr, BYTE &val, bool debug)
{
    m_debug = debug;
    val = OnRead(addr);
    return true;
}

// write the specifies address to the chip if the chip is not selected with
// the address specified returns false.
bool CDevice::Write(Word addr, BYTE val, bool debug)
{
    m_debug = debug;
    OnWrite(addr, val);
    return true;
}
