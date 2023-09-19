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

// clang-format off
BEGIN_MESSAGE_MAP(CStackWnd, CBaseWnd)
    ON_MESSAGE(WM_REDRAWALL, OnRedrawAll)
    // ON_WM_ERASEBKGND()
END_MESSAGE_MAP()
// clang-format on

bool CStackWnd::OnEraseBkgnd(CDC *pDC)
{
    return false;
}

LRESULT CStackWnd::OnRedrawAll(WPARAM wParam, LPARAM lParam)
{
    int i;
    for (i = 0; i < numLines; i++)
        ClearLine(i);
    for (i = 0; i < totNumLines; i++)
        drawLine(lnPageStart + i);
    return TRUE;
}

void CStackWnd::ClearLine(int line)
{
    if (continuous)
        return;
    CClientDC dc(this);
    CRect     rc;
    int       x;

    GetLineRect(line - lnPageStart, &rc);
    dc.FillRect(&rc, &brNormal);

    x = CharWidth * 4 + 2 * SideMargin;
    dc.MoveTo(x, rc.top);
    dc.LineTo(x, rc.bottom);
    x += CharWidth * 4 + 2 * SideMargin;
    dc.MoveTo(x, rc.top);
    dc.LineTo(x, rc.bottom);
}

void CStackWnd::ScrollDown(int nl)
{
    if (continuous)
        return;
    CClientDC dc(this);
    CRect     rc;
    GetLineRect(0, &rc);
    dc.BitBlt(0, SideMargin + CharHeight * nl, rc.right, CharHeight * (totNumLines - nl), (CDC *) &dc, 0, SideMargin,
              SRCCOPY);
}

void CStackWnd::ScrollUp(int nl)
{
    if (continuous)
        return;
    CClientDC dc(this);
    CRect     rc;
    GetLineRect(0, &rc);
    dc.BitBlt(0, SideMargin, rc.right, CharHeight * (totNumLines - nl), (CDC *) &dc, 0, SideMargin + CharHeight * nl,
              SRCCOPY);
}

void CStackWnd::drawLine(LINENUMBER lnActualLine)
{
    if (continuous)
        return;
    CClientDC dc(this);
    CRect     rc;
    char      buffer[256];
    int       scr_line = lnActualLine - lnPageStart;
    int       code_ind = totNumLines - lnActualLine - 1;

    ClearLine(lnActualLine);
    GetLineRect(scr_line, &rc);
    // if(SelectedLine == lnActualLine)
    //	dc.DrawFocusRect(&rc);
    BYTE b[2];
    b[0] = (pCodes[code_ind].addr & 0xff00) >> 8;
    b[1] = pCodes[code_ind].addr & 0xff;

    hexer.ByteArrayToHexArray(b, 2, buffer);
    dc.TextOut(SideMargin, rc.top, buffer);
    StackInfo si = pCodes[code_ind];

    hexer.ByteToHex(mem->Read(si.addr), buffer);
    dc.TextOut(SideMargin * 3 + CharWidth * 6, rc.top, buffer);

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

    dc.TextOut(SideMargin * 5 + CharWidth * 8, rc.top, buffer);
}

void CStackWnd::UpdateScroll()
{
    SetScrollRange(SB_VERT, 0, totNumLines, false);
    SetScrollPos(SB_VERT, lnPageStart);
}

void CStackWnd::paintBkgnd(LPCRECT lpcRect)
{
    if (continuous)
        return;
    CClientDC dc(this);
    int       x;

    x = CharWidth * 4 + 2 * SideMargin;
    dc.MoveTo(x, lpcRect->top + 1);
    dc.LineTo(x, lpcRect->bottom);

    x += CharWidth * 4 + 2 * SideMargin;
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
    totNumLines++;
    StackInfo info;
    info.reg = rn;
    info.addr = addr;
    pCodes.push_back(info);
}

void CStackWnd::Push(BYTE code, bool paint)
{
    switch (code)
    {
    case 0x36: /*PSHA*/
        ScrollDown(1);
        PushEx(_ACCA, pRegs->sp + 1);
        if (paint)
            drawLine(0);
        break;
    case 0x37: /*PSHB*/
        ScrollDown(1);
        PushEx(_ACCB, pRegs->sp + 1);
        if (paint)
            drawLine(0);
        break;
    case 0x8d:
    case 0xad:
    case 0xbd: /*BSR, JSR(IND), JSR(EXT)*/
        ScrollDown(2);
        PushEx(_PCL, pRegs->sp + 2);
        PushEx(_PCH, pRegs->sp + 1);
        if (paint)
        {
            drawLine(0);
            drawLine(1);
        }
        break;
    case 0x00: /*INTERRUPT*/
        ScrollDown(7);
        PushEx(_PCL, pRegs->sp + 7);
        PushEx(_PCH, pRegs->sp + 6);
        PushEx(_XL, pRegs->sp + 5);
        PushEx(_XH, pRegs->sp + 4);
        PushEx(_ACCA, pRegs->sp + 3);
        PushEx(_ACCB, pRegs->sp + 2);
        PushEx(_CCR, pRegs->sp + 1);
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
        PushEx(_NONE, pRegs->sp + 1);
        if (paint)
            drawLine(0);
        break;
    default:
        return;
    }
}

void CStackWnd::PopEx(int numPop)
{
    totNumLines -= numPop;
    if (totNumLines < 0)
        totNumLines = 0;
    pCodes.resize(totNumLines);
}

void CStackWnd::Pop(BYTE code, bool paint)
{
    if (totNumLines)
        switch (code)
        {
        case 0x32: /*PULA*/
        case 0x33: /*PULB*/
            ScrollUp(1);
            if (paint)
                ClearLine(totNumLines - 1);
            PopEx(1);
            break;
        case 0x39: /*RTS*/
            ScrollUp(2);
            if (paint)
            {
                ClearLine(totNumLines - 1);
                ClearLine(totNumLines - 2);
            }
            PopEx(2);
            break;
        case 0x3b: /*RTI*/
        {
            ScrollUp(7);
            for (int i = 0; i < 7; i++)
            {
                if (paint)
                    ClearLine(totNumLines - 1);
                PopEx(1);
            }
        }
        break;
        case 0x34: /*DES*/
            ScrollUp(1);
            PopEx(1);
            if (paint)
                ClearLine(totNumLines - 1);
            break;
        case 0x02: // RESET
            totNumLines = 0;
            pCodes.clear();
            lnPageStart = 0;
            break;
        default:
            return;
        }
}

CStackWnd::CStackWnd(CWnd *pParentWnd, CRect &rcPos) :
    CBaseWnd(pParentWnd, rcPos, "Stack Window")
{
    totNumLines = 0;
    lnPageStart = 0;
}

CStackWnd::~CStackWnd()
{
}