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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <device.h>
#include <hippy.h>

class CEprom : public CDevice
{
private:
    BYTE    memory[0x2000]{};
    bool    LoadFile();
    CString GetRomFileName();

public:
    BYTE OnRead(Word addr) override;
    void OnInitialize() override;
};

/////////////////////////////////////////////////////////////////////////////
// The one and only CEpromApp object

CEpromApp theApp;

///////////////////////////////////////

int HexToByte(char c)
{
    c = toupper(c);
    if (c <= 'F' && c >= 'A')
        return c - 'A' + 10;
    else
        return c - '0';
}

BYTE GetNextByte(FILE *f)
{
    int hi, lo;
    hi = fgetc(f);
    lo = fgetc(f);
    return HexToByte(hi) * 0x10 + HexToByte(lo);
}

CString CEprom::GetRomFileName()
{
    char buf[256];
    GetModuleFileName(theApp.m_hInstance, buf, 256);

    int i = strlen(buf);
    while (buf[i] != '\\')
        i--;
    buf[i + 1] = 0;
    CString str(buf);

    str += *lpszDeviceName;
    str += ".rom";
    return str;
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

    FILE *f = fopen(GetRomFileName(), "r");
    if (!f)
    {
        // char msg[1024];
        CString msg;
        msg.Format("Rom image file not found :\n %s", GetRomFileName());
        m_pParentWnd->MessageBox(msg, lpszDeviceName->GetBuffer(1));
        return false;
    }
    while (!feof(f))
    {
        do
        {
            if (feof(f))
                break;
            hi = fgetc(f);
        } while (hi != 'S');
        lo = fgetc(f);
        if (hi == 'S')
        {
            if (lo == '0' || lo == '9')
            {
                while (fgetc(f) != '\n' && !feof(f))
                    ;
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
                    memory[addr] = val;
                    addr++;
                }

                ck = GetNextByte(f);
                //	if( (BYTE)~checksum != ck ){
                // MessageBox(NULL, "CHECK SUM ERROR", "ERROR", MB_OK|MB_ICONWARNING);
                //	return false;
                //}
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
        ret = memory[addr];
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

/////////////////////////////////////////////////////////////////////////////
// CEpromApp construction

CEpromApp::CEpromApp()
{
}
