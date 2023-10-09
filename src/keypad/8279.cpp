// 8279.cpp : Defines the initialization routines for the DLL.
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

#include "8279.h"
#include "keypad.h"

extern "C" __declspec(dllexport) CDevice *GetNewDevice();
__declspec(dllexport) CDevice *GetNewDevice()
{
    return new CIntel8279();
}

/////////////////////////////////////////////////////////////////////////////
// CKeypadApp

BEGIN_MESSAGE_MAP(CKeypadApp, CWinApp)
//{{AFX_MSG_MAP(CKeypadApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeypadApp construction

CKeypadApp::CKeypadApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CKeypadApp object

CKeypadApp theApp;
