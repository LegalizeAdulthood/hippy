// eprom.cpp : Defines the initialization routines for the DLL.
//
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

#include "stdafx.h"

#include "eprom.h"

#include <device.h>
#include <hippy.h>

#include <wx/filename.h>
#include <wx/msgdlg.h>

class CEprom : public CDevice
{
public:
    BYTE OnRead(Word addr) override;
    void OnInitialize() override;

private:
    BYTE    m_memory[0x2000]{};

    bool    LoadFile();
    wxString GetRomFileName();
};

/////////////////////////////////////////////////////////////////////////////
// The one and only CEpromApp object

CEpromApp g_theApp;

///////////////////////////////////////

int HexToByte(char c)
{
    c = toupper(c);
    return c <= 'F' && c >= 'A' ? c - 'A' + 10 : c - '0';
}

BYTE GetNextByte(FILE *f)
{
    int hi, lo;
    hi = fgetc(f);
    lo = fgetc(f);
    return HexToByte(hi) * 0x10 + HexToByte(lo);
}

wxString CEprom::GetRomFileName()
{
    wxString libName;
    GetLibraryName(libName);
    HMODULE module = GetModuleHandle(libName);
    TCHAR buf[256];
    GetModuleFileName(module, buf, 256);
    wxFileName modulePath(buf);
    return wxFileName(modulePath.GetPath(), m_deviceName + wxT(".rom")).GetFullPath();
}

///////////////////////////////////////////////

bool CEprom::LoadFile()
{
    int  hi, lo;
    int  lg;
    Word addr;
    BYTE val, ck;
    BYTE checksum;
    Word last_addr = 0xffff;

    FILE *f = fopen(CT2A(GetRomFileName()), "r");
    if (!f)
    {
        wxMessageBox(wxT("Rom image file not found:\n") + GetRomFileName(), m_deviceName);
        return false;
    }
    while (!feof(f))
    {
        do
        {
            if (feof(f))
            {
                break;
            }
            hi = fgetc(f);
        } while (hi != 'S');
        lo = fgetc(f);
        if (hi == 'S')
        {
            if (lo == '0' || lo == '9')
            {
                while (fgetc(f) != '\n' && !feof(f))
                {
                }
            }
            else
            {
                lg = GetNextByte(f);
                addr = GetNextByte(f);
                checksum = addr + lg;
                val = GetNextByte(f);
                checksum += val;
                addr = addr * 0x100 + val;
                lg -= 3;
                for (; lg > 0; lg--)
                {
                    val = GetNextByte(f);
                    checksum += val;
                    m_memory[addr] = val;
                    addr++;
                }

                ck = GetNextByte(f);
#if 0
                if ((BYTE) ~checksum != ck)
                {
                    wxMessageBox(wxT("CHECK SUM ERROR"), wxT("ERROR"), wxOK | wxICON_WARNING);
                    return false;
                }
#endif
                fgetc(f); // newline
            }
        }
    }
    fclose(f);
    return true;
}

void CEprom::OnInitialize()
{
    LoadFile();
}

BYTE CEprom::OnRead(Word addr)
{
    BYTE ret = 0xff;
    if (addr < 0x2000)
    {
        ret = m_memory[addr];
    }
    return ret;
}

extern "C" __declspec(dllexport) CDevice *GetNewDevice();
__declspec(dllexport) CDevice *GetNewDevice()
{
    return new CEprom();
}

/////////////////////////////////////////////////////////////////////////////
// CEpromApp

BEGIN_MESSAGE_MAP(CEpromApp, CWinApp)
//{{AFX_MSG_MAP(CEpromApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
