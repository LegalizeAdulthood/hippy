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
#ifndef HIPPY_REGISTERWND_H
#define HIPPY_REGISTERWND_H

#include "hippy.h"

#include <afxwin.h>

enum RegEnum
{
    rgNone,
    rgA,
    rgB,
    rgPC,
    rgX,
    rgSP,
    rgH,
    rgI,
    rgN,
    rgZ,
    rgV,
    rgC
};

class CRegisterWnd : public CWnd
{
public:
    // constructor & destructor
    CRegisterWnd(CWnd *pParentWnd, CRect &rcPos, Registers *pRegs, LPCTSTR szWindowName = nullptr);
    ~CRegisterWnd() override = default;

    void            Update(bool drawAll = false);
    afx_msg void    OnDestroy();
    afx_msg LRESULT OnRedrawAll(WPARAM wParam, LPARAM lParam);
    afx_msg void    OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void    OnSetFocus(CWnd *pOldWnd);
    afx_msg void    OnKillFocus(CWnd *pNewWnd);
    afx_msg void    OnPaint();
    afx_msg void    OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL    OnEraseBkgnd(CDC *pDC);

    DECLARE_MESSAGE_MAP()

private:
    CFont      m_font;
    CFont     *m_defaultFont{};
    CBrush     m_normal;
    CBrush     m_selected;
    CBrush     m_recent;
    int        m_charWidth{};
    int        m_charHeight{};
    int        m_sideMargin{};
    Registers *m_regs{};        // points to actual processor registers
    Registers  m_prevRegs{};    // previous register states
    Registers  m_prevRegsAct{}; // used as boolean type, true for each element if
                                // that element was modified in previous instruction.
    RegEnum m_selReg{};         // shows the selected reg

    // editing-----------
    Word m_editMask{}; // mask showing last quadrant edited (like 0x0f)

    void drawRegister(bool bActive, LPCRECT lprc, RegEnum reReg, int regsize, int regval);
    void UpdateMetrics();
};

#endif
