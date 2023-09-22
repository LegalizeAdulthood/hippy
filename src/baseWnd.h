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
#ifndef HIPPY_BASEWND_H
#define HIPPY_BASEWND_H

#include "addrmng.h"
#include "disassembler.h"
#include "hippy.h"

#include <afxwin.h>

class CBaseWnd : public CWnd
{
public:
    CBaseWnd(CWnd *pParentWnd, CRect &rcPos, LPCTSTR szWindowName = NULL);
    ~CBaseWnd() override;

    void GetLineRect(int screenNum, LPRECT lpRect);
    void SetMemory(CAddressManager *pbMem)
    {
        m_memoryBase = pbMem;
    }
    afx_msg void OnKillFocus(CWnd *pNewWnd);
    afx_msg void OnSetFocus(CWnd *pOldWnd);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void OnSize(UINT p1, int p2, int p3);
    afx_msg BOOL OnMouseWheel(UINT nFlag, short zDelta, CPoint pt);

    DECLARE_MESSAGE_MAP()

protected:
    CAddressManager *m_memoryBase;
    LINENUMBER       m_pageStart;
    HexDumper        m_hexer;
    int              m_charWidth;
    int              m_charHeight;
    int              m_sideMargin;
    LINENUMBER       m_numLines;
    int              m_totNumLines;
    int              m_lineWidth;
    int              m_selectedLine;
    CBrush           m_selected;
    CBrush           m_active;
    CBrush           m_normal;
    CFont            m_font;
    CFont           *m_defaultFont;

    virtual void drawLine(LINENUMBER lnActualLine)
    {
    }
    virtual void paintBkgnd(LPCRECT lpcRect)
    {
    }
    bool isLineVisible(Word wLineNum);
    void UpdateMetrics();
};

#endif
