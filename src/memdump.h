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
#ifndef _MEMDUMP_H__
#define _MEMDUMP_H__

#include "baseWnd.h"

class CMemDumpWnd : public CBaseWnd
{
private:
    BYTE editMask;
    void invertMask();
    int  SelectedByte;
    void paintBkgnd(LPCRECT lpcRect);
    void drawLine(LINENUMBER lnActualNum);

public:
    CMemDumpWnd(CWnd *pParentWnd, CRect &rcPos, LPCTSTR szWindowName = NULL);

    afx_msg void    OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void    OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg LRESULT OnMemLocChange(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

#endif