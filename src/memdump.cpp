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
#include "memdump.h"

#include "addrmng.h"
#include "disassembler.h"

// clang-format off
BEGIN_MESSAGE_MAP(CMemDumpWnd, CBaseWnd)
    ON_WM_LBUTTONDOWN()
    ON_WM_KEYDOWN()
    ON_MESSAGE(WM_MEMLOCCHANGE, OnMemLocChange)
END_MESSAGE_MAP()
// clang-format on

/*
 * Message received when data stored in a memory location changes
 * wParam in the memory location changes.
 */
LRESULT CMemDumpWnd::OnMemLocChange(WPARAM wParam, LPARAM lParam)
{
    Word w = (Word) wParam;
    if (isLineVisible(w / 8))
    {
        drawLine(w / 8);
    }
    return TRUE;
}

void CMemDumpWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    CBaseWnd::OnLButtonDown(nFlags, point);
    // which line is clicked?
    int line = (point.y - m_sideMargin) / m_charHeight + m_pageStart;

    // calculate the byte clicked
    int offx = point.x - (m_sideMargin * 3 + m_charWidth * 4);
    offx /= m_charWidth;

    // if the click targets to black chars btw bytes then offx mod 3 is 2
    if (offx % 3 == 2)
    {
        // in which case the selection is dismissed
        SelectedByte = -1;
    }
    else
    {
        SelectedByte = offx / 3;
    }

    if (SelectedByte > 7)
    {
        SelectedByte = -1;
    }

    // if selected line is the same or selection is dismissed
    // then all we have to update is the old(new) selected line
    if (SelectedByte == -1 || line == m_selectedLine)
    {
        drawLine(m_selectedLine);
    }
    else
    {
        int tmp = m_selectedLine;
        m_selectedLine = line;
        drawLine(m_selectedLine);
        drawLine(tmp);
    }
    editMask = 0x10;
}

void CMemDumpWnd::invertMask()
{
    editMask = editMask << 0x4;
    if (!editMask)
    {
        editMask = 1;
    }
}

void CMemDumpWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CBaseWnd::OnKeyDown(nChar, nRepCnt, nFlags);
    switch (nChar)
    {
    case VK_LEFT:
        if (SelectedByte > 0)
        {
            SelectedByte--;
        }
        drawLine(m_selectedLine);
        editMask = 0x10;
        break;
    case VK_RIGHT:
        if (SelectedByte < 7)
        {
            SelectedByte++;
        }
        drawLine(m_selectedLine);
        editMask = 0x10;
        break;
    }

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
        Word addr = m_selectedLine * 8 + SelectedByte;
        BYTE w = m_memoryBase->Read(addr, true);
        w &= editMask == 0x10 ? 0x0f : 0xf0;

        w |= val * editMask;
        m_memoryBase->Write(addr, w);
        invertMask();
        if (editMask == 0x10)
        {
            SelectedByte = (SelectedByte + 1) % 8;
            if (!SelectedByte)
            {
                m_selectedLine++;
                if (m_selectedLine == 0x2000)
                {
                    m_selectedLine--;
                    SelectedByte = 7;
                    drawLine(m_selectedLine);
                    return;
                }
            }
            drawLine(m_selectedLine - 1);
        }
        drawLine(m_selectedLine);
    }
}

// draws a single line
void CMemDumpWnd::drawLine(LINENUMBER lnActualNum)
{
    CRect     rc;
    char      buffer[100];
    BYTE      b[8];
    int       i;
    CClientDC dc(this);

    GetLineRect(lnActualNum - m_pageStart, &rc);
    dc.FillRect(&rc, &m_normal);

    dc.MoveTo(m_sideMargin * 2 + m_charWidth * 4, rc.top);
    dc.LineTo(m_sideMargin * 2 + m_charWidth * 4, rc.bottom);

    dc.MoveTo(m_sideMargin * 4 + m_charWidth * 28, rc.top);
    dc.LineTo(m_sideMargin * 4 + m_charWidth * 28, rc.bottom);

    ADDRESS addr;
    BYTE    c[2];
    // calculate starting address for the block
    addr = lnActualNum * 8;
    c[0] = ((BYTE *) &addr)[1];
    c[1] = ((BYTE *) &addr)[0];

    HexDumper::ByteArrayToHexArray(c, 2, buffer);
    dc.TextOut(m_sideMargin, rc.top, CA2T(buffer), 4);

    if (m_selectedLine == lnActualNum && SelectedByte > -1 && GetFocus() == this)
    {
        rc.left = 3 * m_sideMargin + (4 + SelectedByte * 3) * m_charWidth;
        rc.right = rc.left + 2 * m_charWidth;
        dc.FillRect(&rc, &m_selected);
        dc.DrawFocusRect(&rc);
    }

    for (i = 0; i < 8; i++)
    {
        b[i] = m_memoryBase->Read(addr + i, true);
    }
    HexDumper::ByteArrayToHexArrayEx(b, 8, buffer);
    dc.TextOut(m_sideMargin * 3 + m_charWidth * 4, rc.top, CA2T(buffer), 23);
}

// paint the background
void CMemDumpWnd::paintBkgnd(LPCRECT lpcRect)
{
    CClientDC dc(this);

    int x = m_sideMargin * 2 + m_charWidth * 4;
    dc.MoveTo(x, lpcRect->top + 1);
    dc.LineTo(x, lpcRect->bottom);

    x += m_sideMargin * 2 + m_charWidth * 24;
    dc.MoveTo(x, lpcRect->top + 1);
    dc.LineTo(x, lpcRect->bottom);
}

// constructor
CMemDumpWnd::CMemDumpWnd(CWnd *pParentWnd, CRect &rcPos, LPCTSTR szWindowName) :
    CBaseWnd(pParentWnd, rcPos, szWindowName)
{
    editMask = 0x10;
    m_pageStart = 0x0000;
    m_totNumLines = 0x10000 / 8;
    // Scrollbar finetuning
    SetScrollRange(SB_VERT, 0, m_totNumLines - 1, false);
    SetScrollPos(SB_VERT, m_pageStart);
}
