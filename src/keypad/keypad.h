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
#ifndef HIPPY_KEYPAD_H
#define HIPPY_KEYPAD_H

#include <device.h>
#include <hippy.h>

#include <afxwin.h>

#include <wx/wx.h>

/*-----------------------------------
 * FIFO
 *-----------------------------------*/
class CFifo
{
public:
    CFifo();
    void Reset()
    {
        m_overflow = m_underflow = false;
    }
    int  GetNumChar() const;
    bool IsUnderFlow() const
    {
        return m_underflow;
    }
    bool IsOverFlow() const
    {
        return m_overflow;
    }
    bool IsFifoFull() const;
    bool IsFifoEmpty() const;
    void ClearFifo();
    BYTE RemoveFromFifo();
    void InsertIntoFifo(BYTE val);

private:
    BYTE m_buffer[8]{};
    int  m_fifoHead{};
    int  m_fifoTail{};
    bool m_overflow{};
    bool m_underflow{};
};

/*-----------------------------------
 * KEYPAD
 *-----------------------------------*/

#define led_width 12
#define led_height 2
#define led_spacing 0
#define disp_spacing 10
#define disp_width (led_width + 2 * led_spacing + 2 * led_height)
#define key_size 30
#define key_spacing 5
#define keyboard_width 6 * key_size + 7 * key_spacing
#define keyboard_height 4 * key_size + 5 * key_spacing

class CKeyPad : public CFrameWnd
{
public:
    CKeyPad(CDevice *pdevParent, CWnd *parent);
    ~CKeyPad() override = default;

    int GetFifoNumChar() const
    {
        return m_fifoRAM.GetNumChar();
    }
    bool IsFifoFull() const
    {
        return m_fifoRAM.IsFifoFull();
    }
    bool IsFifoOverFlow() const
    {
        return m_fifoRAM.IsOverFlow();
    }
    bool IsFifoUnderFlow() const
    {
        return m_fifoRAM.IsUnderFlow();
    }
    BYTE GetDispRAM(Word word) const
    {
        return m_dispRAM[word];
    }
    void SetDispRAM(int i, BYTE value)
    {
        m_dispRAM[i] = value;
    }
    BYTE RemoveFromFifo()
    {
        return m_fifoRAM.RemoveFromFifo();
    }

    void         ClearDisplay();
    void         ClearFifoRAM();
    void         SetDisplayValue(int segment, BYTE wVal);
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

    DECLARE_MESSAGE_MAP()

private:
    CFifo    m_fifoRAM;
    BYTE     m_dispRAM[8]{};
    CBrush   m_light;
    CBrush   m_dark;
    CBrush   m_back;
    CBrush   m_board;
    CBitmap  m_bmp;
    int      m_padWidth{};
    int      m_padHeight{};
    CRect    m_displayRect;
    CRect    m_keyPadRect;
    CPoint   m_downKeyPoint;
    CWnd    *m_parent{};
    CDevice *m_devParent{};

    void     DrawDisplay();
    void     Draw7Segment(int x, int y, BYTE val);
    void     PushKey(int row, int col);
    void     RestoreKey(int row, int col);
    void     GetKeyRect(CPoint point, CRect &rc);
};

enum IODevice
{
    FIFO,
    DISPLAY
};

enum DisplayMode
{
    dm8x8bitLeft,
    dm16x8bitLeft,
    dm8x8bitRight,
    dm16x8bitRight
};

enum KeyboardMode
{
    kmEnc2KeyLock,
    kmDec2KeyLock,
    kmEncNKeyRoll,
    kmDecNKeyRoll,
    kmEncMatrix,
    kmDecMatrix,
    kmStrobeEncScan,
    kmStrobeDecScan
};

class CIntel8279 : public CDevice
{
protected:
    void Reset() override;
    void OnInitialize() override;
    BYTE OnRead(Word addr) override;
    void OnWrite(Word addr, BYTE bVal) override;

private:
    BYTE         m_status{};
    KeyboardMode m_keyMode{};
    DisplayMode  m_dispMode{};
    bool         m_errMode{};
    IODevice     m_writeTo{};
    IODevice     m_readFrom{};
    Word         m_addrRW{};
    bool         m_autoInc{};
    CKeyPad     *m_keyPad{};
};

#endif
