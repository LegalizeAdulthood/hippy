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

#include "keypad.h"

#include "resource.h"

CFifo::CFifo()
{
    ClearFifo();
}

bool CFifo::IsFifoFull() const
{
    return GetNumChar() == 7;
}

bool CFifo::IsFifoEmpty() const
{
    return m_fifoHead == m_fifoTail;
}

void CFifo::ClearFifo()
{
    m_fifoHead = m_fifoTail = 0;
}

BYTE CFifo::RemoveFromFifo()
{
    BYTE ret = 0;
    if (!IsFifoEmpty())
    {
        ret = m_buffer[m_fifoHead];
        m_fifoHead = (m_fifoHead + 1) % 8;
    }
    else
    {
        m_underflow = true;
    }
    return ret;
}

int CFifo::GetNumChar() const
{
    int i = (m_fifoTail - m_fifoHead);
    if (i < 0)
    {
        i = i + 8;
    }
    return (i % 8);
}

void CFifo::InsertIntoFifo(BYTE val)
{
    if (IsFifoFull())
    {
        m_overflow = true;
    }
    m_buffer[m_fifoTail] = val;
    m_fifoTail = (1 + m_fifoTail) % 8;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

// clang-format off
BEGIN_MESSAGE_MAP(CKeyPad, CFrameWnd)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
END_MESSAGE_MAP()
// clang-format on

void CKeyPad::OnPaint()
{
    PAINTSTRUCT ps;
    BeginPaint(&ps);
    CDC       BmpDC;
    CClientDC dc(this);
    CRect     rc;

    GetClientRect(&rc);
    dc.FillRect(rc, &m_board);
    DrawDisplay();
    BmpDC.CreateCompatibleDC(nullptr);
    BmpDC.SelectObject(&m_bmp);
    dc.BitBlt(m_keyPadRect.left, m_keyPadRect.top, m_keyPadRect.Width(), m_keyPadRect.Height(), &BmpDC, 0, 0, SRCCOPY);
    BmpDC.DeleteDC();
    EndPaint(&ps);
}

void CKeyPad::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (PtInRect(&m_keyPadRect, point))
    {
        point.x -= m_keyPadRect.left;
        point.y -= m_keyPadRect.top;
        int row = point.x / 35;
        int col = point.y / 35;
        if (row > 5)
        {
            return;
        }
        if (col > 3)
        {
            return;
        }
        int code;
        code = col * 10 + row;
        if (code == 20 || code == 30)
        {
            return;
        }
        PushKey(row, col);
        m_downKeyPoint.x = row;
        m_downKeyPoint.y = col;
        if (KeyMap[code] == 0xff)
        {
        }
        else if (KeyMap[code] == 0xbb)
        {
            switch (code)
            {
            case 0x00: /*reset*/
                m_devParent->Interrupt(RESET);
                break;
            case 0x0a: /*irq*/
                m_devParent->Interrupt(IRQ);
                break;
            }
        }
        else
        {
            /*num pad key*/
            if (m_fifoRAM.IsFifoFull())
            {
                // buffer overflow
            }
            m_fifoRAM.InsertIntoFifo(KeyMap[code]);
        }

        DrawDisplay();
    }
}

void CKeyPad::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_downKeyPoint.x != -1)
    {
        RestoreKey(m_downKeyPoint.x, m_downKeyPoint.y);
        m_downKeyPoint.x = -1;
    }
}

void CKeyPad::GetKeyRect(CPoint point, CRect &rc)
{
    rc.left = (key_spacing + key_size) * point.x + key_spacing;
    rc.top = (key_spacing + key_size) * point.y + key_spacing;
    rc.right = rc.left + key_size;
    rc.bottom = rc.top + key_size;
}

void CKeyPad::PushKey(int row, int col)
{
    CRect     rc;
    CClientDC dc(this);

    GetKeyRect(CPoint(row, col), rc);
    rc.OffsetRect(CPoint(m_keyPadRect.left, m_keyPadRect.top));
    CPen pen, *oldp;
    pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
    oldp = dc.SelectObject(&pen);
    dc.MoveTo(rc.right, rc.top);
    dc.LineTo(rc.right, rc.bottom);
    dc.LineTo(rc.left, rc.bottom);
    pen.DeleteObject();
    pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    dc.SelectObject(&pen);
    dc.LineTo(rc.left, rc.top);
    dc.LineTo(rc.right, rc.top);
    dc.SelectObject(oldp);
    pen.DeleteObject();
}

void CKeyPad::RestoreKey(int row, int col)
{
    CRect     rc;
    CClientDC dc(this);
    CDC       BmpDC;

    GetKeyRect(CPoint(row, col), rc);
    BmpDC.CreateCompatibleDC(nullptr);
    BmpDC.SelectObject(&m_bmp);
    dc.BitBlt(m_keyPadRect.left + rc.left, m_keyPadRect.top + rc.top, key_size + 3, key_size + 3, &BmpDC, rc.left,
              rc.top, SRCCOPY);
    BmpDC.DeleteDC();
}

void CKeyPad::DrawDisplay()
{
    if (IsWindow(this->m_hWnd))
    {
        CClientDC dc(this);
        dc.FillRect(&m_displayRect, &m_back);
        for (int i = 0; i < 8; i++)
        {
            Draw7Segment(m_displayRect.left + disp_spacing + i * (disp_width + disp_spacing),
                         m_displayRect.top + disp_spacing, m_dispRAM[i]);
        }
    }
}

void CKeyPad::Draw7Segment(int x, int y, BYTE val)
{
    int       offsety = led_height + led_width + 2 * led_spacing;
    CClientDC dc(this);
    // BYTE	den = 0x80;
    CRect   rc;
    CBrush *br;

    br = val & 0x01 ? &m_light : &m_dark;
    rc.left = x + led_height;
    rc.right = rc.left + led_width;
    rc.top = y;
    rc.bottom = y + led_height;
    dc.FillRect(&rc, br);

    br = val & 0x40 ? &m_light : &m_dark;
    rc.OffsetRect(CPoint(0, offsety));
    dc.FillRect(&rc, br);

    br = val & 0x08 ? &m_light : &m_dark;
    rc.OffsetRect(CPoint(0, offsety));
    dc.FillRect(&rc, br);

    br = val & 0x20 ? &m_light : &m_dark;
    rc.left = x;
    rc.top = y + led_height + led_spacing;
    rc.right = x + led_height;
    rc.bottom = rc.top + led_width;
    dc.FillRect(&rc, br);

    br = val & 0x10 ? &m_light : &m_dark;
    rc.OffsetRect(CPoint(0, offsety));
    dc.FillRect(&rc, br);

    br = val & 0x04 ? &m_light : &m_dark;
    rc.OffsetRect(CPoint(led_width + led_height, 0));
    dc.FillRect(&rc, br);

    br = val & 0x02 ? &m_light : &m_dark;
    rc.OffsetRect(CPoint(0, -offsety));
    dc.FillRect(&rc, br);
}

void CKeyPad::ClearDisplay()
{
    for (int i = 0; i < 8; i++)
    {
        m_dispRAM[i] = 0;
    }
}

void CKeyPad::ClearFifoRAM()
{
    m_fifoRAM.ClearFifo();
}
void CKeyPad::SetDisplayValue(int segment, BYTE wVal)
{
    m_dispRAM[segment] = wVal;
    DrawDisplay();
}

CKeyPad::CKeyPad(CDevice *pdevParent, CWnd *parent) :
    m_parent(parent),
    m_devParent(pdevParent)
{
    m_light.CreateSolidBrush(RGB(255, 0, 0));
    m_dark.CreateSolidBrush(RGB(160, 0, 0));
    m_back.CreateSolidBrush(RGB(170, 0, 0));
    m_board.CreateSolidBrush(RGB(50, 140, 50));
    m_displayRect.left = 5;
    m_displayRect.top = 5;
    m_displayRect.right = m_displayRect.left + 9 * disp_spacing + 8 * disp_width;
    m_displayRect.bottom = m_displayRect.top + 2 * disp_spacing + 3 * led_spacing + 2 * led_width + 3 * led_height;

    m_keyPadRect.left = m_displayRect.left;
    m_keyPadRect.top = m_displayRect.bottom + disp_spacing;
    m_keyPadRect.bottom = m_keyPadRect.top + keyboard_height;
    m_keyPadRect.right = m_keyPadRect.left + keyboard_width;

    int y = 200;
    int x = 200;

    CFrameWnd::Create(nullptr, wxT("KeyPad"), WS_SYSMENU | WS_VISIBLE,
                      CRect(x, y, x + m_displayRect.right + 20, y + m_keyPadRect.bottom + disp_spacing + 35), m_parent,
                      nullptr, WS_EX_TOOLWINDOW);

    wxString libName;
    m_devParent->GetLibraryName(libName);
    HANDLE hBmp = LoadImage(GetModuleHandle(libName), MAKEINTRESOURCE(IDB_BMP_KEYPAD), IMAGE_BITMAP, 0, 0,
                            LR_CREATEDIBSECTION | LR_DEFAULTSIZE);
    m_bmp.Attach((HBITMAP) hBmp);

    ClearDisplay();
    ClearFifoRAM();
    m_downKeyPoint.x = -1;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

void CIntel8279::OnWrite(Word addr, BYTE bVal)
{
    int i;
    // if A0 input is high then this is a command
    if (addr)
    { // COMMAND
        // Most significant 3 bits specifies the command
        switch ((bVal & 0xe0) >> 5)
        {
        case 0: // MODE SET
            m_dispMode = (DisplayMode) (bVal & 0x18);
            m_keyMode = (KeyboardMode) (bVal & 0x07);
            break;
        case 1:
            break;
        case 2: // READ FIFO
            m_readFrom = FIFO;
            break;
        case 3: // READ DISPLAY
            m_readFrom = DISPLAY;
            m_addrRW = bVal & 0x0f;
            m_autoInc = (bVal & 0x10) != 0;
            break;
        case 4: // WRITE DISPLAY
            m_writeTo = DISPLAY;
            m_addrRW = bVal & 0x0f;
            m_autoInc = (bVal & 0x10) != 0;
            break;
        case 5: // BLANKING
            break;
        case 6: // CLEAR
            if (bVal & 0x01)
            { // CLEAR ALL
                m_keyPad->ClearDisplay();
                m_keyPad->ClearFifoRAM();
                //& clear fifo status
                m_status = 0;
            }
            else
            {
                if (bVal & 0x02)
                {
                    m_keyPad->ClearFifoRAM();
                }
                if (bVal & 0x10)
                {
                    switch (bVal & 0x0c)
                    {
                    case 0x08:
                        for (i = 0; i < 8; i++)
                        {
                            m_keyPad->SetDispRAM(i, 0xff);
                        }
                        break;
                    case 0x0c:
                        for (i = 0; i < 8; i++)
                        {
                            m_keyPad->SetDispRAM(i, 0x20);
                        }
                        break;
                    default:
                        m_keyPad->ClearDisplay();
                        break;
                    }
                }
            }

            break;
        case 7:
            if (bVal & 0x10)
            {
                m_errMode = true;
            }
            break;
        }
    }
    else
    { // DATA WRITE -- TO DISPLAY RAM
        m_keyPad->SetDisplayValue(m_addrRW, bVal);
        if (m_autoInc)
        {
            if (m_dispMode == dm8x8bitLeft)
            {
                m_addrRW = (m_addrRW - 1) % 8;
            }
            else
            {
                m_addrRW = (m_addrRW + 1) % 8;
            }
        }
    }
}

BYTE CIntel8279::OnRead(Word addr)
{
    if (m_debug)
        return 0;
    BYTE res = 0;
    if (addr == 0x00)
    { // read data
        if (m_readFrom == FIFO)
        {
            res = m_keyPad->RemoveFromFifo();
        }
        else
        {
            res = m_keyPad->GetDispRAM(m_addrRW);
            if (m_autoInc)
            {
                if (m_dispMode == dm8x8bitLeft)
                {
                    m_addrRW = (m_addrRW - 1) % 8;
                }
                else
                {
                    m_addrRW = (m_addrRW + 1) % 8;
                }
            }
        }
    }
    else
    { // status
        m_status = 0;
        // last 3 bits number of chars in fifo
        res = (m_status & 0xf0) + m_keyPad->GetFifoNumChar();
        // 4th bit --> fifo full?
        if (m_keyPad->IsFifoFull())
        {
            res |= 0x080;
        }
        if (m_keyPad->IsFifoOverFlow())
        {
            res |= 0x20;
        }
        if (m_keyPad->IsFifoUnderFlow())
        {
            res |= 0x10;
        }
    }
    return res;
}

void CIntel8279::Reset()
{
    m_dispMode = dm8x8bitLeft;
    m_keyMode = kmEncNKeyRoll;
    m_errMode = false;
    m_autoInc = false;
}

void CIntel8279::OnInitialize()
{
    TRACE0("Generated KeyPad.\n");
    m_keyPad = new CKeyPad(this, m_parentWnd);
}
