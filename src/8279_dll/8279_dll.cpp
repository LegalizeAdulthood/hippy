// 8279_dll.cpp : Defines the initialization routines for the DLL.
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
#include "8279_dll.h"
#include "keypad.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" __declspec(dllexport) CDevice *  GetNewDevice();
__declspec(dllexport) CDevice * GetNewDevice(){
	return new CIntel8279();
}

/////////////////////////////////////////////////////////////////////////////
// CMy8279_dllApp

BEGIN_MESSAGE_MAP(CMy8279_dllApp, CWinApp)
	//{{AFX_MSG_MAP(CMy8279_dllApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMy8279_dllApp construction

CMy8279_dllApp::CMy8279_dllApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMy8279_dllApp object

CMy8279_dllApp theApp;