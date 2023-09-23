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
    {
        for (; zDelta; zDelta++)
        {
            OnVScroll(SB_PAGEDOWN, 0, nullptr);
        }
    }
    else
    {
        for (; zDelta; zDelta--)
        {
            OnVScroll(SB_PAGEUP, 0, nullptr);
        }
    }

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
    line -= m_pageStart;
    if (line >= 0)
    {
        line -= m_numLines;
        if (line < 0)
        {
            res = true;
        }
    }
    return res;
}

// returns the bounding rectangle of a line. no test is done.
void CBaseWnd::GetLineRect(int screenNum, LPRECT lpRect)
{
    lpRect->left = m_sideMargin;
    lpRect->right = m_lineWidth;
    lpRect->top = m_sideMargin + screenNum * m_charHeight;
    lpRect->bottom = lpRect->top + m_charHeight;
}

// paints the window
void CBaseWnd::OnPaint()
{
    // BYTE * p = pbMemoryPageStart;
    ADDRESS a = m_pageStart;
    // int ret;
    // char buffer[1024];
    PAINTSTRUCT ps;
    BeginPaint(&ps);

    // First paint the borders and grids for columns
    CRect     rc;
    CClientDC dc(this);
    CPen      newp;

    // if the window has got the focus draw a stronger border
    if (GetFocus() == this)
    {
        newp.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    }
    else
    {
        newp.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    }

    CPen *oldp = dc.SelectObject(&newp);
    GetClientRect(&rc);

    // draw the rectangle around the window
    dc.MoveTo(rc.left + 1, rc.top + 1);
    dc.LineTo(rc.right - 1, rc.top + 1);
    dc.LineTo(rc.right - 1, rc.bottom - 1);
    dc.LineTo(rc.left + 1, rc.bottom - 1);
    dc.LineTo(rc.left + 1, rc.top + 1);

    dc.SelectObject(oldp);
    newp.DeleteObject();

    paintBkgnd(&rc);

    for (int start = m_numLines - 1; start >= 0; start--)
    {
        if (m_totNumLines <= a)
        {
            break;
        }
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
        if (!m_pageStart)
        {
            break;
        }
        int i = m_pageStart;
        i -= m_numLines;
        m_pageStart = (i < 0) ? 0 : i;
        this->RedrawWindow();
    }
    break;
    case VK_NEXT:
    {
        int line = m_pageStart + m_numLines;
        if (line + m_numLines >= m_totNumLines)
        {
            m_pageStart = m_totNumLines - m_numLines;
        }
        else
        {
            m_pageStart += m_numLines;
        }
        this->RedrawWindow();
    }
    break;
    case VK_UP:
    {
        // selection shifts to the above line if possible
        // if the new selected line is not shown then the
        //  we jump there
        if (m_selectedLine > 0)
        { // we a an even smaller line there
            int old = m_selectedLine;
            int olds = old - m_pageStart;
            m_selectedLine--;
            if (olds > 0)
            {
                // no need for shifts
                drawLine(old);
                drawLine(m_selectedLine);
            }
            else if (olds)
            {
                // old was not shown, so we need a jump
                m_pageStart = m_selectedLine;
                RedrawWindow();
            }
            else
            {
                // Shift 1 line
                if (m_pageStart > 0)
                {
                    m_pageStart--;
                }
                CClientDC dc(this);
                CRect     rc;
                GetClientRect(&rc);
                dc.BitBlt(0, m_sideMargin + m_charHeight * 2, rc.right, m_charHeight * (m_numLines - 2), (CDC *) &dc, 0,
                          m_sideMargin + m_charHeight, SRCCOPY);
                drawLine(m_pageStart);
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
        if (m_selectedLine < m_totNumLines - 1)
        { // we a an even greater line there
            int old = m_selectedLine;
            int olds = old - m_pageStart;
            m_selectedLine++;
            if (olds >= -1 && olds < m_numLines - 1)
            {
                // no need for shifts
                drawLine(old);
                drawLine(m_selectedLine);
            }
            else if (olds == m_numLines - 1)
            {
                // Shift 1 line
                m_pageStart++;
                if (m_pageStart > m_totNumLines - m_numLines)
                {
                    m_pageStart = m_totNumLines - m_numLines;
                    break;
                }
                CClientDC dc(this);
                CRect     rc;
                GetClientRect(&rc);
                dc.BitBlt(0, m_sideMargin, rc.right, m_charHeight * (m_numLines - 2), (CDC *) &dc, 0, m_sideMargin + m_charHeight,
                          SRCCOPY);
                drawLine(m_selectedLine);
                drawLine(old);
            }
            else
            {
                // old was not shown, so we need a jump
                m_pageStart = m_selectedLine;
                RedrawWindow();
            }
        }
    }
    break;
    case VK_TAB:
        CWnd *wnd = GetNextWindow();
        if (wnd)
        {
            wnd->SetFocus();
        }
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
    m_charWidth = tm.tmMaxCharWidth;
    m_sideMargin = m_charWidth / 2;
    m_lineWidth = rc.right - m_charWidth;
    m_numLines = (LINENUMBER) ((rc.bottom - rc.top - 2 * m_sideMargin) / tm.tmHeight);
    m_charHeight = tm.tmHeight;
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
        if (m_pageStart + m_numLines < m_totNumLines)
        {
            CRect rc;
            GetClientRect(&rc);
            dc.BitBlt(0, m_sideMargin, rc.right, m_charHeight * (m_numLines - 1), (CDC *) &dc, 0, m_sideMargin + m_charHeight,
                      SRCCOPY);
            m_pageStart++;
            drawLine(m_pageStart + m_numLines - 1);
        }
        break;
    case SB_LINEUP:
        if (m_pageStart)
        {
            CRect rc;
            GetClientRect(&rc);
            dc.BitBlt(0, m_sideMargin + m_charHeight, rc.right, m_charHeight * (m_numLines - 1), (CDC *) &dc, 0, m_sideMargin,
                      SRCCOPY);
            m_pageStart--;
            drawLine(m_pageStart);
        }
        break;
    case SB_THUMBPOSITION:
        nPos &= 0xFFFF;
        m_pageStart = ((int) nPos > m_totNumLines - m_numLines) ? m_totNumLines - m_numLines : nPos;
        SetScrollPos(SB_VERT, nPos);
        RedrawWindow();
        break;
    case SB_THUMBTRACK:
        nPos &= 0xFFFF;
        m_pageStart = ((int) nPos > m_totNumLines - m_numLines) ? m_totNumLines - m_numLines : nPos;
        SetScrollPos(SB_VERT, nPos);
        RedrawWindow();
        break;
    }
}

CBaseWnd::CBaseWnd(CWnd *pParentWnd, CRect &rcPos, LPCTSTR szWindowName)
{
    const char *p = AfxRegisterWndClass(CS_OWNDC | CS_SAVEBITS, LoadCursor(nullptr, IDC_ARROW), nullptr, nullptr);
    Create(p, "Base Window", WS_CHILD | WS_TABSTOP | WS_VSCROLL, rcPos, pParentWnd, 0);

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

    this->SetFont(&m_font, false);
    CClientDC dc(this);
    m_selected.CreateSolidBrush(RGB(180, 180, 180));
    m_active.CreateSolidBrush(RGB(200, 200, 0));
    m_normal.CreateSolidBrush(RGB(255, 255, 255));

    m_defaultFont = dc.SelectObject(&m_font);
    dc.SetBkMode(TRANSPARENT);
    UpdateMetrics();
}

CBaseWnd::~CBaseWnd()
{
    m_active.DeleteObject();
    m_normal.DeleteObject();
    m_selected.DeleteObject();
    m_font.DeleteObject();
}
