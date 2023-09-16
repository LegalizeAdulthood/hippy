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

#ifndef STACK_WND_H__12
#define STACK_WND_H__12

#include "basewnd.h"
#include "hippy.h"
#include <afxwin.h>

typedef enum
{
    _ACCA,
    _ACCB,
    _PCH,
    _PCL,
    _CCR,
    _XH,
    _XL,
    _NONE
} RegName;

typedef struct StackInfo
{
    Word    addr;
    RegName reg;
} StackInfo;

class CStackWnd : public CBaseWnd
{
private:
    bool             continuous;
    StackInfo       *pCodes;
    Registers       *pRegs;
    HexDumper        hexer;
    CAddressManager *mem;

    void drawLine(LINENUMBER lnActualLine);
    void paintBkgnd(LPCRECT lpcRect);
    void PushEx(RegName rn, Word addr);
    void PopEx(int numPop);
    void UpdateScroll();
    void Push(BYTE code, bool paint);
    void Pop(BYTE code, bool paint);
    void ClearLine(int line);

public:
    void toggleContinuous(bool cont)
    {
        continuous = cont;
    }
    CStackWnd(CWnd *pParentWnd, CRect &rcPos);
    ~CStackWnd();
    void SetRegisters(Registers *pRegs)
    {
        this->pRegs = pRegs;
    }
    void SetMem(CAddressManager *mem)
    {
        this->mem = mem;
    }
    void            CheckStack(BYTE code, bool paint = true);
    void            ScrollUp(int nl);
    void            ScrollDown(int nl = 1);
    afx_msg bool    OnEraseBkgnd(CDC *pDC);
    afx_msg LRESULT OnRedrawAll(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
};

#endif