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

#include "baseWnd.h"

// clang-format off
BEGIN_MESSAGE_MAP(CBaseWnd, CWnd)
    ON_WM_KEYDOWN()
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_VSCROLL()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_SIZE()
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()
// clang-format on

void CBaseWnd::OnSize(UINT p1, int p2, int p3)
{
    UpdateMetrics();
}

BOOL CBaseWnd::OnMouseWheel(UINT nFlag, short zDelta, CPoint pt)
{
    zDelta /= 120;
    if (zDelta < 0)
        for (; zDelta; zDelta++)
            OnVScroll(SB_PAGEDOWN, 0, NULL);
    else
        for (; zDelta; zDelta--)
            OnVScroll(SB_PAGEUP, 0, NULL);

    return true;
}

void CBaseWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (GetFocus() != this)
    { // gain focus
        SetFocus();
        RedrawWindow();
    }
}

void CBaseWnd::OnKillFocus(CWnd *pNewWnd)
{
    CClientDC dc(this);
    CRect     rc;
    GetClientRect(&rc);
    dc.Rectangle(-1, -1, rc.right, rc.bottom);
    RedrawWindow();
}

void CBaseWnd::OnSetFocus(CWnd *pOldWnd)
{
    CClientDC dc(this);
    CRect     rc;
    GetClientRect(&rc);
    dc.Rectangle(-1, -1, rc.right, rc.bottom);
    RedrawWindow();
}

// returns true if the line is within screen rectangle
bool CBaseWnd::isLineVisible(Word wLineNum)
{
    bool res = false;
    int  line = wLineNum;
    line -= lnPageStart;
    if (line >= 0)
    {
        line -= numLines;
        if (line < 0)
            res = true;
    }
    return res;
}

// returns the bounding rectangle of a line. no test is done.
void CBaseWnd::GetLineRect(int screenNum, LPRECT lpRect)
{
    lpRect->left = SideMargin;
    lpRect->right = LineWidth;
    lpRect->top = SideMargin + screenNum * CharHeight;
    lpRect->bottom = lpRect->top + CharHeight;
}

// paints the window
void CBaseWnd::OnPaint()
{
    // BYTE * p = pbMemoryPageStart;
    ADDRESS a = lnPageStart;
    // int ret;
    // char buffer[1024];
    PAINTSTRUCT ps;
    BeginPaint(&ps);

    // First paint the borders and grids for columns

    CRect     rc;
    CClientDC dc(this);

    CPen *oldp, newp;

    // if the window has got the focus draw a stronger border
    if (GetFocus() == this)
        newp.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    else
        newp.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

    oldp = dc.SelectObject(&newp);
    GetClientRect(&rc);

    // draw the rectangle around the window
    {
        dc.MoveTo(rc.left + 1, rc.top + 1);
        dc.LineTo(rc.right - 1, rc.top + 1);
        dc.LineTo(rc.right - 1, rc.bottom - 1);
        dc.LineTo(rc.left + 1, rc.bottom - 1);
        dc.LineTo(rc.left + 1, rc.top + 1);
    }

    dc.SelectObject(oldp);
    newp.DeleteObject();

    paintBkgnd(&rc);

    for (int start = numLines - 1; start >= 0; start--)
    {
        if (totNumLines <= a)
            break;
        drawLine(a);
        a++;
    }

    EndPaint(&ps);
}

void CBaseWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
    case VK_PRIOR:
    {
        if (!lnPageStart)
            break;
        int i = lnPageStart;
        i -= numLines;
        lnPageStart = (i < 0) ? 0 : i;
        this->RedrawWindow();
    }
    break;
    case VK_NEXT:
    {
        int line = lnPageStart + numLines;
        if (line + numLines >= totNumLines)
            lnPageStart = totNumLines - numLines;
        else
            lnPageStart += numLines;
        this->RedrawWindow();
    }
    break;
    case VK_UP:
    {
        // selection shifts to the above line if possible
        // if the new selected line is not shown then the
        //  we jump there
        if (SelectedLine > 0)
        { // we a an even smaller line there
            int old = SelectedLine;
            int olds = old - lnPageStart;
            SelectedLine--;
            if (olds > 0)
            {
                // no need for shifts
                drawLine(old);
                drawLine(SelectedLine);
            }
            else if (olds)
            {
                // old was not shown, so we need a jump
                lnPageStart = SelectedLine;
                RedrawWindow();
            }
            else
            {
                // Shift 1 line
                lnPageStart--;
                if (lnPageStart < 0)
                {
                    lnPageStart = 0;
                    break;
                }
                CClientDC dc(this);
                CRect     rc;
                GetClientRect(&rc);
                dc.BitBlt(0, SideMargin + CharHeight * 2, rc.right, CharHeight * (numLines - 2), (CDC *) &dc, 0,
                          SideMargin + CharHeight, SRCCOPY);
                drawLine(lnPageStart);
                drawLine(old);
            }
        }
    }
    break;
    case VK_DOWN:
    {
        // selection shifts to the below line if possible
        // if the new selected line is not shown then the
        //  we jump there
        if (SelectedLine < totNumLines - 1)
        { // we a an even greater line there
            int old = SelectedLine;
            int olds = old - lnPageStart;
            SelectedLine++;
            if (olds >= -1 && olds < numLines - 1)
            {
                // no need for shifts
                drawLine(old);
                drawLine(SelectedLine);
            }
            else if (olds == numLines - 1)
            {
                // Shift 1 line
                lnPageStart++;
                if (lnPageStart > totNumLines - numLines)
                {
                    lnPageStart = totNumLines - numLines;
                    break;
                }
                CClientDC dc(this);
                CRect     rc;
                GetClientRect(&rc);
                dc.BitBlt(0, SideMargin, rc.right, CharHeight * (numLines - 2), (CDC *) &dc, 0, SideMargin + CharHeight,
                          SRCCOPY);
                drawLine(SelectedLine);
                drawLine(old);
            }
            else
            {
                // old was not shown, so we need a jump
                lnPageStart = SelectedLine;
                RedrawWindow();
            }
        }
    }
    break;
    case VK_TAB:
        CWnd *wnd = GetNextWindow();
        if (wnd)
            wnd->SetFocus();
        break;
    }
}

// calculates num chars per line & num lines
void CBaseWnd::UpdateMetrics()
{
    TEXTMETRIC tm;
    CRect      rc;
    CClientDC  dc(this);
    dc.GetTextMetrics(&tm);
    GetClientRect(&rc);
    // GetWindowRect(rc);
    //	numCharsPerLine = (rc.right - rc.left - 2 * SideMargin) / tm.tmMaxCharWidth;
    CharWidth = tm.tmMaxCharWidth;
    SideMargin = CharWidth / 2;
    LineWidth = rc.right - CharWidth;
    numLines = (LINENUMBER) ((rc.bottom - rc.top - 2 * SideMargin) / tm.tmHeight);
    CharHeight = tm.tmHeight;
}

void CBaseWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
    CClientDC dc(this);
    switch (nSBCode)
    {
    case SB_PAGEDOWN:
        OnKeyDown(VK_NEXT, 0, 0);
        break;
    case SB_PAGEUP:
        OnKeyDown(VK_PRIOR, 0, 0);
        break;
    case SB_LINEDOWN:
        if (lnPageStart + numLines < totNumLines)
        {
            CRect rc;
            GetClientRect(&rc);
            dc.BitBlt(0, SideMargin, rc.right, CharHeight * (numLines - 1), (CDC *) &dc, 0, SideMargin + CharHeight,
                      SRCCOPY);
            lnPageStart++;
            drawLine(lnPageStart + numLines - 1);
        }
        break;
    case SB_LINEUP:
        if (lnPageStart)
        {
            CRect rc;
            GetClientRect(&rc);
            dc.BitBlt(0, SideMargin + CharHeight, rc.right, CharHeight * (numLines - 1), (CDC *) &dc, 0, SideMargin,
                      SRCCOPY);
            lnPageStart--;
            drawLine(lnPageStart);
        }
        break;
    case SB_THUMBPOSITION:
        nPos &= 0xFFFF;
        lnPageStart = ((int) nPos > totNumLines - numLines) ? totNumLines - numLines : nPos;
        SetScrollPos(SB_VERT, nPos);
        RedrawWindow();
        break;
    case SB_THUMBTRACK:
        nPos &= 0xFFFF;
        lnPageStart = ((int) nPos > totNumLines - numLines) ? totNumLines - numLines : nPos;
        SetScrollPos(SB_VERT, nPos);
        RedrawWindow();
        break;
    }
}

CBaseWnd::CBaseWnd(CWnd *pParentWnd, CRect &rcPos, LPCTSTR szWindowName)
{

    const char *p = AfxRegisterWndClass(CS_OWNDC | CS_SAVEBITS, LoadCursor(NULL, IDC_ARROW), NULL, 0);
    Create(p, "Base Window", WS_CHILD | WS_TABSTOP | WS_VSCROLL, rcPos, pParentWnd, 0);

    Font.CreateFont(12,                       // nHeight
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

    this->SetFont(&Font, false);
    CClientDC dc(this);
    brSelected.CreateSolidBrush(RGB(180, 180, 180));
    brActive.CreateSolidBrush(RGB(200, 200, 0));
    brNormal.CreateSolidBrush(RGB(255, 255, 255));

    defFont = dc.SelectObject(&Font);
    dc.SetBkMode(TRANSPARENT);
    SelectedLine = 0xEE;
    UpdateMetrics();
}

CBaseWnd::~CBaseWnd()
{
    brActive.DeleteObject();
    brNormal.DeleteObject();
    brSelected.DeleteObject();
    Font.DeleteObject();
}