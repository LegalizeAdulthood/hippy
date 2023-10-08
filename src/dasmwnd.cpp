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

#include "dasmwnd.h"

#include "addrmng.h"
#include "dialogs.h"
#include "resource.h"

#include <wx/wx.h>

// clang-format off
BEGIN_MESSAGE_MAP(CDisasmWnd, CBaseWnd)
    ON_WM_KEYDOWN()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_ERASEBKGND()
    ON_COMMAND(ID_DASM_GOTO, OnGoto)
    ON_COMMAND(ID_DASM_DISASSEMBLE, OnDasmHere)
    ON_MESSAGE(WM_REDRAWALL, OnRedrawAll)
END_MESSAGE_MAP()
// clang-format on

int CDisasmWnd::add_codePt(Word w)
{
    if (m_codeListing.insert(w))
    {
        /*	Word m = m_instructionPos[w];
                Word t = m_memoryAddressLocator[m];
                if(m!=t) m_codeListing.clearall();
                m_codeListing.insert(w);
                LoadProgram(0);*/
    }

    return 0;
}

void CDisasmWnd::OnDasmHere()
{
    CInputBox ib;
    if (ib.ShowModal(&CString("Enter address to disassemble from:")) == IDOK)
    {
        Word w = ib.AsWord();
        DasmHere(w);
        m_selectedLine = m_instructionPos[w];
        m_pageStart = m_instructionPos[w];
        RedrawWindow();
    }
}

void CDisasmWnd::OnGoto()
{
    CInputBox ib;

    if (ib.ShowModal() == IDOK)
    {
        CString str;
        Word    w = ib.AsWord();
        m_selectedLine = m_instructionPos[w];
        m_pageStart = m_instructionPos[w];
        RedrawWindow();
    }
}

void CDisasmWnd::OnRButtonUp(UINT nFlags, CPoint point)
{
    CMenu  popup;
    CMenu *sub;
#ifdef UNICODE
    popup.LoadMenuW(IDR_DASM_POPUP);
#else
    popup.LoadMenuA(IDR_DASM_POPUP);
#endif
    sub = popup.GetSubMenu(0);
    CRect rc;
    GetWindowRect(&rc);
    sub->TrackPopupMenu(0, rc.left + point.x, rc.top + point.y, this, CRect(0, 0, 200, 200));
    CBaseWnd::OnRButtonUp(nFlags, point);
}

// returns the screen line number of a point on the client dc
int CDisasmWnd::GetLineFromPt(int x, int y)
{
    return (y - m_sideMargin) / m_charHeight;
}

/*
 * Returns the memory location corresponding to
 * selected line in the disassambly view.
 */
int CDisasmWnd::GetActiveMemLoc()
{
    return m_memoryAddressLocator[m_selectedLine];
}

/*
 * Toggles a breakpoint to the selected line
 * removes if any set, sets if no breakpoint here
 */
void CDisasmWnd::InsertBrkPtHere()
{
    Word addr = m_memoryAddressLocator[m_selectedLine];
    BYTE b = m_breakpoints[addr];
    b = b != 0 ? 0 : 1;
    m_breakpoints[addr] = b;
    drawLine(m_selectedLine);
}

void CDisasmWnd::drawLine(LINENUMBER lnActualNum)
{
    char      buffer[20];
    int       ret;
    int       x, y;
    CClientDC dc(this);
    CBrush    br;
    int       screenNum = lnActualNum - m_pageStart;
    Word      addr = m_memoryAddressLocator[lnActualNum];
    BYTE      bp = m_breakpoints[addr];
    BYTE      b[2];

    // do nothing if line is not visible
    if (screenNum < 0 && screenNum >= m_numLines)
    {
        return;
    }

    // if line is selected or active (next inst. to be executed)
    // then handle them here (note that selected overrides active)
    CRect rc;
    GetLineRect(screenNum, &rc);

    if (m_selectedLine == lnActualNum && GetFocus() == this)
    {
        dc.FillRect(&rc, &m_selected);
        dc.DrawFocusRect(&rc);
    }
    else
    {
        dc.FillRect(&rc, m_activeLine == lnActualNum ? &m_active : &m_normal);
    }

    // if there is a breakpoint on this line, draw a small rectangle
    if (bp)
    {
        rc.right = 2 * m_sideMargin + 4 * m_charWidth;
        dc.FillRect(&rc, &m_breakpointBrush);
    }

    dc.MoveTo(m_sideMargin * 2 + 4 * m_charWidth, rc.top);
    dc.LineTo(m_sideMargin * 2 + 4 * m_charWidth, rc.bottom);
    dc.MoveTo(m_sideMargin * 4 + 10 * m_charWidth, rc.top);
    dc.LineTo(m_sideMargin * 4 + 10 * m_charWidth, rc.bottom);
    dc.MoveTo(m_sideMargin * 5 + 26 * m_charWidth, rc.top);
    dc.LineTo(m_sideMargin * 5 + 26 * m_charWidth, rc.bottom);

    y = m_charHeight * screenNum + m_sideMargin;

    BYTE buff[3];

    int length = m_instructionLength[lnActualNum] & 0x0f;
    for (int i = 0; i < length; i++)
    {
        buff[i] = m_memoryBase->Read(m_memoryAddressLocator[lnActualNum] + i, true);
    }
    ret = m_dasm.Dasm((BYTE *) buff, m_instructionLength[lnActualNum], buffer);
    if (ret == INVALID_CODE)
    {
        ret = 1;
    }
    x = m_charWidth * 10 + 5 * m_sideMargin;
    dc.TextOut(x, y, buffer);

    HexDumper::ByteArrayToHexArray(buff, ret, buffer);
    x = m_charWidth * 4 + 3 * m_sideMargin;
    dc.TextOut(x, y, buffer);

    b[0] = (BYTE) (m_memoryAddressLocator[lnActualNum] >> 8);
    b[1] = (BYTE) (m_memoryAddressLocator[lnActualNum] & 0x00FF);
    HexDumper::ByteArrayToHexArray(b, 2, buffer);
    x = m_sideMargin;
    dc.TextOut(x, y, buffer);
}

void CDisasmWnd::paintBkgnd(LPCRECT lpcRect)
{
    CClientDC dc(this);
    int       x;

    x = m_charWidth * 4 + 2 * m_sideMargin;
    dc.MoveTo(x, lpcRect->top + 1);
    dc.LineTo(x, lpcRect->bottom);

    x += m_charWidth * 6 + 2 * m_sideMargin;
    dc.MoveTo(x, lpcRect->top + 1);
    dc.LineTo(x, lpcRect->bottom);

    x += m_charWidth * 16 + m_sideMargin;
    dc.MoveTo(x, lpcRect->top + 1);
    dc.LineTo(x, lpcRect->bottom);
}

void CDisasmWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    CBaseWnd::OnLButtonDown(nFlags, point);

    // figure out the line that received the click
    int i = GetLineFromPt(point.x, point.y);
    if (i >= m_numLines)
    {
        return;
    }

    // i is a screen line, convert it to actual line
    i += m_pageStart;
    if (m_selectedLine != i)
    {
        int olda = m_selectedLine;
        m_selectedLine = i;
        // firstly, if selected line was shown, we need to redraw it
        drawLine(olda);
        drawLine(i);
    }
}

LRESULT CDisasmWnd::OnRedrawAll(WPARAM wParam, LPARAM lParam)
{
    Update((Word) wParam);
    return TRUE;
}

void CDisasmWnd::Update(Word pc)
{
    Word oldAct = m_activeLine;          // save old line (we will redraw it if it is visible)
    m_activeLine = m_instructionPos[pc]; // recent active line is set

    // if current active line is not within the screen
    // scroll window, in this case the old line is erased automatically
    if (!isLineVisible(m_activeLine))
    {
        m_pageStart = m_activeLine;
        // one last thing is to check the location of the line
        // if it is at the extreme end of the memory modify the first line so that
        // the last line show 0xFFFF
        m_pageStart = (m_pageStart > (m_totNumLines - m_numLines)) ? (m_totNumLines - m_numLines) : m_pageStart;
        RedrawWindow();
    }
    else
    {
        if (isLineVisible(oldAct))
        {
            drawLine(oldAct);
        }
        drawLine(m_activeLine);
    }
}

void CDisasmWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CBaseWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

/*
 * returns non-zero if the address is marked with a breakpoint.
 * returns -1 for soft break points (removed after first stop.
 * return 1 for hard break points.
 */
int CDisasmWnd::IsBrkPoint(Word addr)
{
    BYTE b = m_breakpoints[addr];
    return (int) b;
}

/*
 * sets the higher 4 bits of the line (corresponding the address) so that its recognized as as a breakpoint.
 */
void CDisasmWnd::InsertBreakpoint(ADDRESS addr, bool soft)
{
    m_instructionLength[m_instructionPos[addr]] |= soft ? 0x30 : 0xF0;
}

/*
 * resets the higer four bits of the line corresponding the address
 */
void CDisasmWnd::RemoveBreakpoint(ADDRESS addr)
{
    m_instructionLength[m_instructionPos[addr]] &= 0x0F;
}

void CDisasmWnd::DasmHere(Word addr)
{
}

/*
 *  LoadProgram
 */
void CDisasmWnd::LoadProgram(Word wStart)
{
    ADDRESS addrCur, addrNext;
    int     codelength;
    BYTE    b;
    m_totNumLines = 0;
    m_codeListing.GetNextLocation(addrCur, true);
    do
    {
        m_codeListing.GetNextLocation(addrNext);
        codelength = addrNext - addrCur;

        while (codelength)
        {
            b = g_instDescTbl[m_memoryBase->Read(addrCur, true)] & 0x0F; // masking the cycle bits (num bytes left)
            if (b == 0)
            {          // no such instruction exists
                b = 1; // then it's 1 byte long INVALID_INSTRUCTION
            }
            else if (codelength - b < 0)
            { // there is such an inst. but it does not fit here
                for (; codelength; codelength--)
                { // so the leftover bytes are marked as INVALID
                    m_instructionLength[m_totNumLines] = 1;
                    m_memoryAddressLocator[m_totNumLines] = addrCur;
                    m_instructionPos[addrCur] = m_totNumLines;
                    m_totNumLines++;
                    addrCur++;
                }
                break; // since codelength consumed escape while
            }

            m_instructionLength[m_totNumLines] = b;
            // all memory locations starting from this byte following inst. length bytes,
            // correspond to this line in the window
            for (int i = 0; i < b; i++)
            {
                m_instructionPos[addrCur + i] = m_totNumLines;
            }
            m_memoryAddressLocator[m_totNumLines] = addrCur;
            m_totNumLines++;
            addrCur += b;
            codelength -= b;
        }
        addrCur = addrNext;
    } while (addrCur != 0xFFFF);

    m_pageStart = m_instructionPos[wStart];

    // Scrollbar finetuning
    SetScrollRange(SB_VERT, 0, m_totNumLines - m_numLines, false);
    SetScrollPos(SB_VERT, m_pageStart);
}

CDisasmWnd::~CDisasmWnd()
{
    m_breakpointBrush.DeleteObject();
}

CDisasmWnd::CDisasmWnd(CWnd *pParentWnd, CRect &rcPos, LPCTSTR szWindowName) :
    CBaseWnd(pParentWnd, rcPos, szWindowName)
{
    memset(m_breakpoints, 0, 0x10000);
    m_breakpointBrush.CreateSolidBrush(RGB(255, 0, 0));
}
