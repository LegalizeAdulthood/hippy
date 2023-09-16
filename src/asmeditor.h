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
#ifndef _ASMEDITOR_H__
#define _ASMEDITOR_H__
#include <afx.h>
#include <afxcmn.h>
#include <afxwin.h>

#include "hippy.h"
#include "string.h"

class CAsmEdit : public CRichEditCtrl
{
public:
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    DECLARE_MESSAGE_MAP()
};

class CBuildEdit : public CEdit
{
public:
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    DECLARE_MESSAGE_MAP()
};

static int filenumber = 0;

class CAsmEditorWnd : public CMDIChildWnd
{
private:
    CAsmEdit   Editor;
    CString    szFileName;
    CFont      Font;
    CFont     *oldFont;
    bool       newFile;
    CBuildEdit buildWnd;
    int        buildWndHeight;
    bool       OpenFile();
    int        SaveFile();

public:
    DECLARE_DYNAMIC(CAsmEditorWnd)
    CAsmEditorWnd(CMDIFrameWnd *pParent, LPCSTR lpcFileName);
    ~CAsmEditorWnd();
    bool            IsNewFile();
    void            GetFileName(CString &str);
    int             CompileCode();
    int             Save();
    int             SaveAs(CString szFileName);
    CString         GetHexFileName();
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    afx_msg bool    OnEraseBkgnd(CDC *pDC);
    afx_msg void    OnClose();
    afx_msg LRESULT OnJumpToLine(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnHideBuildWnd(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

#endif