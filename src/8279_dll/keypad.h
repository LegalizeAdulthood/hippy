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
#ifndef _KEYPAD_H__
#define _KEYPAD_H__

#include "resource.h"
#include <afxwin.h>
#include <device.h>
#include <hippy.h>

/*-----------------------------------
 * FIFO
 *-----------------------------------*/
class CFifo
{
private:
    BYTE buffer[8];
    int  fifoHead, fifoTail;
    bool overflow;
    bool underflow;

public:
    CFifo();
    void Reset()
    {
        overflow = underflow = false;
    }
    int  GetNumChar();
    bool IsUnderFlow()
    {
        return underflow;
    }
    bool IsOverFlow()
    {
        return overflow;
    }
    bool IsFifoFull();
    bool IsFifoEmpty();
    void ClearFifo();
    BYTE RemoveFromFifo();
    void InsertIntoFifo(BYTE val);
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

const BYTE DispArr[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                          0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

const BYTE KeyMap[36] = {0xbb, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0xff, 0xff, 0xff, 0xbb, 0xff, 0x08,
                         0x09, 0x0a, 0x0b, 0x11, 0xff, 0xff, 0xff, 0xff, 0xff, 0x04, 0x05, 0x06,
                         0x07, 0x12, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x13};

class CKeyPad : public CFrameWnd
{
private:
    CBrush   brLight;
    CBrush   brDark;
    CBrush   brBack;
    CBrush   brBoard;
    CBitmap  bmp;
    int      padwidth;
    int      padheight;
    CRect    rcDisplay;
    CRect    rcKeyPad;
    CPoint   ptDownKey;
    CWnd    *pParent;
    CDevice *pdevParent;
    void     DrawDisplay();
    void     Draw7Segment(int x, int y, BYTE val);
    void     PushKey(int row, int col);
    void     RestoreKey(int row, int col);
    void     GetKeyRect(CPoint point, CRect &rc);

public:
    CKeyPad(CDevice *pdevParent, CWnd *parent);
    CFifo        FifoRAM;
    BYTE         DispRAM[8];
    void         ClearDisplay();
    void         ClearFifoRAM();
    void         SetDisplayValue(int segment, BYTE wVal);
    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
};

typedef enum
{
    FIFO,
    DISPLAY
} IODevice;

typedef enum
{
    dm8x8bitLeft,
    dm16x8bitLeft,
    dm8x8bitRight,
    dm16x8bitRight
} DisplayMode;

typedef enum
{
    kmEnc2KeyLock,
    kmDec2KeyLock,
    kmEncNKeyRoll,
    kmDecNKeyRoll,
    kmEncMatrix,
    kmDecMatrix,
    kmStrobeEncScan,
    kmStrobeDecScan
} KeyboardMode;

class CIntel8279 : public CDevice
{
private:
    BYTE         status;
    KeyboardMode keyMode;
    DisplayMode  dispMode;
    bool         errMode;
    IODevice     writeTo;
    IODevice     readFrom;
    Word         addrRW;
    bool         AutoInc;
    CKeyPad     *pKeyPad;

protected:
    void Reset() override;
    void OnInitialize() override;
    BYTE OnRead(Word addr) override;
    void OnWrite(Word addr, BYTE bVal) override;

public:
};

#endif