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
#include "stackwnd.h"

#include "addrmng.h"
#include "disassembler.h"

// clang-format off
BEGIN_MESSAGE_MAP(CStackWnd, CBaseWnd)
    ON_MESSAGE(WM_REDRAWALL, OnRedrawAll)
END_MESSAGE_MAP()
// clang-format on

bool CStackWnd::OnEraseBkgnd(CDC *pDC)
{
    return false;
}

LRESULT CStackWnd::OnRedrawAll(WPARAM wParam, LPARAM lParam)
{
    for (int i = 0; i < m_numLines; i++)
    {
        ClearLine(i);
    }
    for (int i = 0; i < m_totNumLines; i++)
    {
        drawLine(m_pageStart + i);
    }
    return TRUE;
}

void CStackWnd::ClearLine(int line)
{
    if (m_continuous)
    {
        return;
    }

    CClientDC dc(this);
    CRect     rc;
    int       x;

    GetLineRect(line - m_pageStart, &rc);
    dc.FillRect(&rc, &m_normal);

    x = m_charWidth * 4 + 2 * m_sideMargin;
    dc.MoveTo(x, rc.top);
    dc.LineTo(x, rc.bottom);
    x += m_charWidth * 4 + 2 * m_sideMargin;
    dc.MoveTo(x, rc.top);
    dc.LineTo(x, rc.bottom);
}

void CStackWnd::ScrollDown(int nl)
{
    if (m_continuous)
    {
        return;
    }

    CClientDC dc(this);
    CRect     rc;
    GetLineRect(0, &rc);
    dc.BitBlt(0, m_sideMargin + m_charHeight * nl, rc.right, m_charHeight * (m_totNumLines - nl), (CDC *) &dc, 0,
              m_sideMargin, SRCCOPY);
}

void CStackWnd::ScrollUp(int nl)
{
    if (m_continuous)
    {
        return;
    }

    CClientDC dc(this);
    CRect     rc;
    GetLineRect(0, &rc);
    dc.BitBlt(0, m_sideMargin, rc.right, m_charHeight * (m_totNumLines - nl), (CDC *) &dc, 0,
              m_sideMargin + m_charHeight * nl, SRCCOPY);
}

void CStackWnd::drawLine(LINENUMBER lnActualLine)
{
    if (m_continuous)
    {
        return;
    }

    CClientDC dc(this);
    CRect     rc;
    char      buffer[256];
    int       scr_line = lnActualLine - m_pageStart;
    int       code_ind = m_totNumLines - lnActualLine - 1;

    ClearLine(lnActualLine);
    GetLineRect(scr_line, &rc);
    // if(m_selectedLine == lnActualLine)
    //	dc.DrawFocusRect(&rc);
    BYTE b[2];
    b[0] = (m_codes[code_ind].addr & 0xff00) >> 8;
    b[1] = m_codes[code_ind].addr & 0xff;

    HexDumper::ByteArrayToHexArray(b, 2, buffer);
    dc.TextOut(m_sideMargin, rc.top, buffer);
    StackInfo si = m_codes[code_ind];

    HexDumper::ByteToHex(m_memory->Read(si.addr), buffer);
    dc.TextOut(m_sideMargin * 3 + m_charWidth * 6, rc.top, buffer);

    switch (si.reg)
    {
    case _ACCA:
        strcpy(buffer, "A");
        break;
    case _ACCB:
        strcpy(buffer, "B");
        break;
    case _PCH:
        strcpy(buffer, "PC HIGH");
        break;
    case _PCL:
        strcpy(buffer, "PC LOW");
        break;
    case _CCR:
        strcpy(buffer, "CCR");
        break;
    case _XH:
        strcpy(buffer, "X HIGH");
        break;
    case _XL:
        strcpy(buffer, "X LOW");
        break;
    case _NONE:
        strcpy(buffer, "");
    }

    dc.TextOut(m_sideMargin * 5 + m_charWidth * 8, rc.top, buffer);
}

void CStackWnd::UpdateScroll()
{
    SetScrollRange(SB_VERT, 0, m_totNumLines, false);
    SetScrollPos(SB_VERT, m_pageStart);
}

void CStackWnd::paintBkgnd(LPCRECT lpcRect)
{
    if (m_continuous)
    {
        return;
    }

    CClientDC dc(this);
    int       x;

    x = m_charWidth * 4 + 2 * m_sideMargin;
    dc.MoveTo(x, lpcRect->top + 1);
    dc.LineTo(x, lpcRect->bottom);

    x += m_charWidth * 4 + 2 * m_sideMargin;
    dc.MoveTo(x, lpcRect->top + 1);
    dc.LineTo(x, lpcRect->bottom);
}

void CStackWnd::CheckStack(BYTE code, bool paint)
{
    Push(code, paint);
    Pop(code, paint);
}

void CStackWnd::PushEx(RegName rn, Word addr)
{
    m_totNumLines++;
    StackInfo info;
    info.reg = rn;
    info.addr = addr;
    m_codes.push_back(info);
}

void CStackWnd::Push(BYTE code, bool paint)
{
    switch (code)
    {
    case 0x36: /*PSHA*/
        ScrollDown(1);
        PushEx(_ACCA, m_regs->sp + 1);
        if (paint)
        {
            drawLine(0);
        }
        break;
    case 0x37: /*PSHB*/
        ScrollDown(1);
        PushEx(_ACCB, m_regs->sp + 1);
        if (paint)
        {
            drawLine(0);
        }
        break;
    case 0x8d:
    case 0xad:
    case 0xbd: /*BSR, JSR(IND), JSR(EXT)*/
        ScrollDown(2);
        PushEx(_PCL, m_regs->sp + 2);
        PushEx(_PCH, m_regs->sp + 1);
        if (paint)
        {
            drawLine(0);
            drawLine(1);
        }
        break;
    case 0x00: /*INTERRUPT*/
        ScrollDown(7);
        PushEx(_PCL, m_regs->sp + 7);
        PushEx(_PCH, m_regs->sp + 6);
        PushEx(_XL, m_regs->sp + 5);
        PushEx(_XH, m_regs->sp + 4);
        PushEx(_ACCA, m_regs->sp + 3);
        PushEx(_ACCB, m_regs->sp + 2);
        PushEx(_CCR, m_regs->sp + 1);
        if (paint)
        {
            drawLine(0);
            drawLine(1);
            drawLine(2);
            drawLine(3);
            drawLine(4);
            drawLine(5);
            drawLine(6);
        }
        break;
    case 0x31:
        ScrollDown(1);
        PushEx(_NONE, m_regs->sp + 1);
        if (paint)
        {
            drawLine(0);
        }
        break;
    default:
        return;
    }
}

void CStackWnd::PopEx(int numPop)
{
    m_totNumLines -= numPop;
    if (m_totNumLines < 0)
    {
        m_totNumLines = 0;
    }
    m_codes.resize(m_totNumLines);
}

void CStackWnd::Pop(BYTE code, bool paint)
{
    if (m_totNumLines)
    {
        switch (code)
        {
        case 0x32: /*PULA*/
        case 0x33: /*PULB*/
            ScrollUp(1);
            if (paint)
            {
                ClearLine(m_totNumLines - 1);
            }
            PopEx(1);
            break;
        case 0x39: /*RTS*/
            ScrollUp(2);
            if (paint)
            {
                ClearLine(m_totNumLines - 1);
                ClearLine(m_totNumLines - 2);
            }
            PopEx(2);
            break;
        case 0x3b: /*RTI*/
        {
            ScrollUp(7);
            for (int i = 0; i < 7; i++)
            {
                if (paint)
                {
                    ClearLine(m_totNumLines - 1);
                }
                PopEx(1);
            }
        }
        break;
        case 0x34: /*DES*/
            ScrollUp(1);
            PopEx(1);
            if (paint)
            {
                ClearLine(m_totNumLines - 1);
            }
            break;
        case 0x02: // RESET
            m_totNumLines = 0;
            m_codes.clear();
            m_pageStart = 0;
            break;
        default:
            return;
        }
    }
}

CStackWnd::CStackWnd(CWnd *pParentWnd, CRect &rcPos) :
    CBaseWnd(pParentWnd, rcPos, _T("Stack Window"))
{
}
