// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
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

#if !defined(AFX_STDAFX_H__37B0BC1F_FDEB_434D_80B9_12E0BA68BC78__INCLUDED_)
#define AFX_STDAFX_H__37B0BC1F_FDEB_434D_80B9_12E0BA68BC78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers

#include <afxext.h> // MFC extensions
#include <afxwin.h> // MFC core and standard components

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdisp.h>  // MFC Automation classes
#include <afxodlgs.h> // MFC OLE dialog classes
#include <afxole.h>   // MFC OLE classes
#endif                // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h> // MFC ODBC database classes
#endif             // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h> // MFC DAO database classes
#endif              // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h> // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif              // _AFX_NO_AFXCMN_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#include <wx/wx.h>

#endif // !defined(AFX_STDAFX_H__37B0BC1F_FDEB_434D_80B9_12E0BA68BC78__INCLUDED_)
