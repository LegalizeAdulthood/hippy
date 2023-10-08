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
#include "registerwnd.h"

#include "disassembler.h"

// clang-format off
BEGIN_MESSAGE_MAP(CRegisterWnd, CWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_KEYDOWN()
    ON_WM_ERASEBKGND()
    ON_MESSAGE(WM_REDRAWALL, OnRedrawAll)
END_MESSAGE_MAP()
// clang-format on

// calculate the metrics related to selected font
void CRegisterWnd::UpdateMetrics()
{
    TEXTMETRIC tm;
    CRect      rc;
    CClientDC  dc(this);
    dc.GetTextMetrics(&tm);
    GetClientRect(&rc);
    m_charWidth = tm.tmMaxCharWidth;
    m_sideMargin = m_charWidth / 2;
    m_charHeight = tm.tmHeight;
}

void CRegisterWnd::drawRegister(bool bActive, LPCRECT lprc, RegEnum reReg, int regsize, int regval)
{
    CClientDC dc(this);
    char      buffer[100];
    CRect     rc(lprc);
    rc.left -= m_sideMargin;
    rc.right += m_sideMargin;

    // set the color of text (red for recently changed registers)
    COLORREF cr = bActive ? RGB(200, 10, 20) : RGB(0, 0, 0);
    dc.SetTextColor(cr);

    // if the register is selected then draw selection rectangle
    if (m_selReg == reReg)
    {
        dc.FillRect(&rc, &m_selected);
        dc.DrawFocusRect(&rc);
    }
    else
    {
        dc.FillRect(&rc, &m_normal);
    }

    if (regsize == 1)
    { // condition code bit
        itoa(regval, buffer, 2);
        dc.TextOut(lprc->left, lprc->top, CA2T(buffer), 1);
    }
    else if (regsize == 2)
    {
        BYTE b = (BYTE) regval;
        HexDumper::ByteToHex(b, buffer);
        dc.TextOut(lprc->left, lprc->top, CA2T(buffer), 2);
    }
    else
    {
        BYTE    b[2];
        ADDRESS ad = (ADDRESS) regval;
        b[0] = ((BYTE *) &ad)[1];
        b[1] = ((BYTE *) &ad)[0];
        HexDumper::ByteArrayToHexArray(b, 2, buffer);
        dc.TextOut(lprc->left, lprc->top, CA2T(buffer), 4);
    }
}

LRESULT CRegisterWnd::OnRedrawAll(WPARAM wParam, LPARAM lParam)
{
    Update();
    return TRUE;
}

// called after each inst. if drawAll is true then (called from paint) all of the registers
// are redrawn in the prevState, else the new states are calculated and only changed ones are
// drawn
void CRegisterWnd::Update(bool drawAll)
{
    Registers regValChanged;
    //	Registers regStChanged;
    COLORREF  crAct = RGB(200, 10, 20);
    COLORREF  crNorm = RGB(0, 0, 0);
    CClientDC dc(this);

    if (drawAll)
    {
        // find changed registers
        regValChanged = m_prevRegsAct;
    }
    else
    {
        // find the registers that are modified
        regValChanged.a = (m_prevRegs.a ^ m_regs->a);
        regValChanged.b = (m_prevRegs.b ^ m_regs->b);
        regValChanged.x = (m_prevRegs.x ^ m_regs->x);
        regValChanged.sp = (m_prevRegs.sp ^ m_regs->sp);
        regValChanged.pc = (m_prevRegs.pc ^ m_regs->pc);
        regValChanged.ccr.c = (m_prevRegs.ccr.c ^ m_regs->ccr.c);
        regValChanged.ccr.v = (m_prevRegs.ccr.v ^ m_regs->ccr.v);
        regValChanged.ccr.i = (m_prevRegs.ccr.i ^ m_regs->ccr.i);
        regValChanged.ccr.h = (m_prevRegs.ccr.h ^ m_regs->ccr.h);
        regValChanged.ccr.z = (m_prevRegs.ccr.z ^ m_regs->ccr.z);
        regValChanged.ccr.n = (m_prevRegs.ccr.n ^ m_regs->ccr.n);

        //
    }

    int    y = m_sideMargin * 3 + m_charHeight;
    int    x = m_sideMargin * 4 + 15 * m_charWidth;
    CPoint pt(0, m_sideMargin * 2 + m_charHeight);

    CRect rc(x, y, x + 2 * m_charWidth, y + m_charHeight);

    if (regValChanged.a || drawAll || m_prevRegsAct.a)
    {
        drawRegister(regValChanged.a > 0, &rc, rgA, 2, m_regs->a);
    }
    rc.OffsetRect(pt);
    if (regValChanged.b || drawAll || m_prevRegsAct.b)
    {
        drawRegister(regValChanged.b > 0, &rc, rgB, 2, m_regs->b);
    }
    rc.OffsetRect(pt);
    rc.right += 2 * m_charWidth;
    if (regValChanged.pc || drawAll || m_prevRegsAct.pc)
    {
        drawRegister(regValChanged.pc > 0, &rc, rgPC, 4, m_regs->pc);
    }
    rc.OffsetRect(pt);
    if (regValChanged.x || drawAll || m_prevRegsAct.x)
    {
        drawRegister(regValChanged.x > 0, &rc, rgX, 4, m_regs->x);
    }
    rc.OffsetRect(pt);
    if (regValChanged.sp || drawAll || m_prevRegsAct.sp)
    {
        drawRegister(regValChanged.sp > 0, &rc, rgSP, 4, m_regs->sp);
    }
    rc.OffsetRect(pt);
    dc.SetTextColor(RGB(0, 0, 0));
    dc.TextOut(rc.left, rc.top, _T("H I N Z V C"), 11);
    rc.OffsetRect(pt);
    rc.right = rc.left + m_charWidth;
    if (regValChanged.ccr.h || drawAll || m_prevRegsAct.ccr.h)
    {
        drawRegister(regValChanged.ccr.h, &rc, rgH, 1, m_regs->ccr.h);
    }
    pt.x = 2 * m_charWidth;
    pt.y = 0;

    rc.OffsetRect(pt);
    if (regValChanged.ccr.i || drawAll || m_prevRegsAct.ccr.i)
    {
        drawRegister(regValChanged.ccr.i, &rc, rgI, 1, m_regs->ccr.i);
    }
    rc.OffsetRect(pt);
    if (regValChanged.ccr.n || drawAll || m_prevRegsAct.ccr.n)
    {
        drawRegister(regValChanged.ccr.n, &rc, rgN, 1, m_regs->ccr.n);
    }
    rc.OffsetRect(pt);
    if (regValChanged.ccr.z || drawAll || m_prevRegsAct.ccr.z)
    {
        drawRegister(regValChanged.ccr.z, &rc, rgZ, 1, m_regs->ccr.z);
    }
    rc.OffsetRect(pt);
    if (regValChanged.ccr.v || drawAll || m_prevRegsAct.ccr.v)
    {
        drawRegister(regValChanged.ccr.v, &rc, rgV, 1, m_regs->ccr.v);
    }
    rc.OffsetRect(pt);
    if (regValChanged.ccr.c || drawAll || m_prevRegsAct.ccr.c)
    {
        drawRegister(regValChanged.ccr.c, &rc, rgC, 1, m_regs->ccr.c);
    }
    // if this is not a paint call then store the new state
    if (!drawAll)
    {
        m_prevRegs = *m_regs;
        m_prevRegsAct = regValChanged;
    }
}

// paint event handler
void CRegisterWnd::OnPaint()
{
    CPen        newp, *oldp;
    CClientDC   dc(this);
    CRect       rc;
    PAINTSTRUCT ps;

    BeginPaint(&ps);

    // if the window has got the focus draw a stronger border
    newp.CreatePen(PS_SOLID, GetFocus() == this ? 2 : 1, RGB(0, 0, 0));

    oldp = dc.SelectObject(&newp);
    GetClientRect(&rc);

    dc.Rectangle(-1, -1, rc.right, rc.bottom);

    // draw the rectangle around the window
    {
        dc.MoveTo(rc.left + 1, rc.top + 1);
        dc.LineTo(rc.right - 1, rc.top + 1);
        dc.LineTo(rc.right - 1, rc.bottom - 1);
        dc.LineTo(rc.left + 1, rc.bottom - 1);
        dc.LineTo(rc.left + 1, rc.top + 1);
    }

    // draw the grids
    newp.DeleteObject();
    newp.CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
    dc.SelectObject(newp);
    int y, x;
    for (int i = 1; i < 7; i++)
    {
        y = i * (2 * m_sideMargin + m_charHeight);
        dc.MoveTo(m_sideMargin, y);
        dc.LineTo(rc.right - m_sideMargin, y);
    }

    x = 15 * m_charWidth + 2 * m_sideMargin;
    dc.MoveTo(x, m_sideMargin);
    dc.LineTo(x, rc.bottom - m_sideMargin);

    // write the table header (register, value) and Reg names
    y = m_sideMargin;
    dc.SetTextColor(RGB(0, 0, 0));
    dc.TextOut(m_sideMargin, y, _T("Register"), 8);
    dc.TextOut(x + 2 * m_sideMargin, y, _T("Value"), 5);
    y += 2 * m_sideMargin + m_charHeight;
    dc.SetTextColor(RGB(100, 100, 100));
    dc.TextOut(m_sideMargin, y, _T("Accumulator A"), 13);
    y += 2 * m_sideMargin + m_charHeight;
    dc.TextOut(m_sideMargin, y, _T("Accumulator B"), 13);
    y += 2 * m_sideMargin + m_charHeight;
    dc.TextOut(m_sideMargin, y, _T("Program Counter"), 15);
    y += 2 * m_sideMargin + m_charHeight;
    dc.TextOut(m_sideMargin, y, _T("Index Reg."), 10);
    y += 2 * m_sideMargin + m_charHeight;
    dc.TextOut(m_sideMargin, y, _T("Stack Pointer"), 13);
    y += 2 * m_sideMargin + m_charHeight;
    dc.TextOut(m_sideMargin, y, _T("Cond. Code Reg."), 15);

    dc.SelectObject(oldp);
    newp.DeleteObject();
    Update(true);
    EndPaint(&ps);
}

BOOL CRegisterWnd::OnEraseBkgnd(CDC *pDC)
{
    return false;
}

void CRegisterWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetFocus();

    // find selection
    RegEnum re = m_selReg;
    m_selReg = rgNone;
    int y = point.y / (m_charHeight + m_sideMargin * 2);
    int x = point.x - (m_sideMargin * 4 + 15 * m_charWidth);
    if (y < 8 && y > 0 && x > 0)
    {
        switch (y)
        {
        case 1:
            m_selReg = rgA;
            break;
        case 2:
            m_selReg = rgB;
            break;
        case 3:
            m_selReg = rgPC;
            break;
        case 4:
            m_selReg = rgX;
            break;
        case 5:
            m_selReg = rgSP;
            break;
        case 7:
        {
            x /= m_charWidth * 2;
            switch (x)
            {
            case 0:
                m_selReg = rgH;
                break;
            case 1:
                m_selReg = rgI;
                break;
            case 2:
                m_selReg = rgN;
                break;
            case 3:
                m_selReg = rgZ;
                break;
            case 4:
                m_selReg = rgV;
                break;
            case 5:
                m_selReg = rgC;
                break;
            }
        }
        };
    }

    if (m_selReg == rgNone)
    {
        m_selReg = re;
    }
    else if (re != m_selReg)
    {
        Update(true);
    }

    m_editMask = 0; // reset the mask
}

void CRegisterWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
    case VK_UP:
    {
        int i = (int) m_selReg - 1;
        if (i)
        {
            if (i >= 6)
            {
                i = 5;
            }
            m_selReg = (RegEnum) i;
            Update(true);
        }
        m_editMask = 0; // reset the mask
    }
    break;
    case VK_DOWN:
    {
        int i = (int) m_selReg + 1;
        if (i < 7)
        {
            m_selReg = (RegEnum) i;
            Update(true);
        }
        m_editMask = 0; // reset the mask
    }
    break;
    case VK_RIGHT:
    {
        int i = (int) m_selReg + 1;
        if (i >= 7 && i <= 11)
        {
            m_selReg = (RegEnum) i;
            Update(true);
        }
        m_editMask = 0; // reset the mask
    }
    break;
    case VK_LEFT:
    {
        int i = (int) m_selReg - 1;
        if (i >= 6 && i <= 11)
        {
            m_selReg = (RegEnum) i;
            Update(true);
        }
        m_editMask = 0; // reset the mask
    }
    };

    // get the hexadecimal value pressed
    int val = -1;

    if ((nChar >= '0') && (nChar <= '9'))
    {
        val = nChar - '0';
    }
    else if ((nChar >= 'A') && (nChar <= 'F'))
    {
        val = nChar - 'A' + 10;
    }

    if (val >= 0)
    {
        if (m_selReg >= rgH && val < 2)
        { // bit field ? (CCR bit)
            // edit mask has no meaning, just change the value
            switch (m_selReg)
            {
            case rgH:
                m_regs->ccr.h = val;
                break;
            case rgI:
                m_regs->ccr.i = val;
                break;
            case rgN:
                m_regs->ccr.n = val;
                break;
            case rgZ:
                m_regs->ccr.z = val;
                break;
            case rgV:
                m_regs->ccr.v = val;
                break;
            case rgC:
                m_regs->ccr.c = val;
                break;
            };

            Update();
        }
        else if ((m_selReg < rgH) && val < 16)
        { // byte or word
            int   editSize;
            Word *pwEdit;

            switch (m_selReg)
            {
            case rgA:
                editSize = 2;
                pwEdit = (Word *) &m_regs->a;
                break;
            case rgB:
                editSize = 2;
                pwEdit = (Word *) &m_regs->b;
                break;
            case rgPC:
                editSize = 4;
                pwEdit = &m_regs->pc;
                break;
            case rgX:
                editSize = 4;
                pwEdit = &m_regs->x;
                break;
            case rgSP:
                editSize = 4;
                pwEdit = &m_regs->sp;
                break;
            }

            int  m = editSize - m_editMask - 1;
            Word mask = 0x000f << (m * 4);
            mask = ~mask;
            *pwEdit &= mask; // the portion to be edited is now all zero
            mask = val << (m * 4);
            *pwEdit |= mask;
            Update();
            m_editMask++;
            if (m_editMask >= editSize)
            {
                m_editMask = 0;
            }
        }
    }
}

void CRegisterWnd::OnSetFocus(CWnd *pOldWnd)
{
    RedrawWindow();
}

void CRegisterWnd::OnKillFocus(CWnd *pNewWnd)
{
    RedrawWindow();
}

// constructor:
// register wnd class, create window and font
CRegisterWnd::CRegisterWnd(CWnd *pParentWnd, CRect &rcPos, Registers *pRegs, LPCTSTR szWindowName)
{
    LPCTSTR p = AfxRegisterWndClass(CS_OWNDC | CS_SAVEBITS, LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr);
    CWnd::Create(p, szWindowName, WS_CHILD | WS_TABSTOP, rcPos, pParentWnd, 1);

    m_regs = pRegs;

    m_font.CreateFont(12,                       // nHeight
                    0,                        // nWidth
                    0,                        // nEscapement
                    0,                        // nOrientation
                    FW_NORMAL,                // nWeight
                    FALSE,                    // bItalic
                    FALSE,                    // bUnderline
                    0,                        // cStrikeOut
                    ANSI_CHARSET,             // nCharSet
                    OUT_DEFAULT_PRECIS,       // nOutPrecision
                    CLIP_DEFAULT_PRECIS,      // nClipPrecision
                    DEFAULT_QUALITY,          // nQuality
                    DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
                    _T("FixedSys"));          // lpszFacename

    SetFont(&m_font, false);
    CClientDC dc(this);
    m_selected.CreateSolidBrush(RGB(180, 180, 180));
    m_recent.CreateSolidBrush(RGB(255, 50, 50));
    m_normal.CreateSolidBrush(RGB(255, 255, 255));
    m_defaultFont = dc.SelectObject(&m_font);
    dc.SetBkMode(TRANSPARENT);
    UpdateMetrics();
}

CRegisterWnd::~CRegisterWnd()
{
    CClientDC dc(this);
    dc.SelectObject(m_defaultFont);
    m_font.DeleteObject();
    m_normal.DeleteObject();
    m_recent.DeleteObject();
    m_selected.DeleteObject();
}
