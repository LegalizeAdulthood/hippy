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

#ifndef HIPPY_STACKWND_H
#define HIPPY_STACKWND_H

#include "basewnd.h"
#include "hippy.h"

#include <afxwin.h>

#include <vector>

enum RegName
{
    _ACCA,
    _ACCB,
    _PCH,
    _PCL,
    _CCR,
    _XH,
    _XL,
    _NONE
};

struct StackInfo
{
    Word    addr;
    RegName reg;
};

class CAddressManager;
struct Registers;

class CStackWnd : public CBaseWnd
{
public:
    CStackWnd(CWnd *pParentWnd, CRect &rcPos);
    ~CStackWnd() override = default;

    void SetContinuous(bool continuous)
    {
        m_continuous = continuous;
    }
    void SetRegisters(Registers *regs)
    {
        m_regs = regs;
    }
    void SetMem(CAddressManager *memory)
    {
        m_memory = memory;
    }

    void            CheckStack(BYTE code, bool paint = true);
    void            ScrollUp(int nl);
    void            ScrollDown(int nl = 1);
    afx_msg bool    OnEraseBkgnd(CDC *pDC);
    afx_msg LRESULT OnRedrawAll(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

private:
    bool                   m_continuous{};
    std::vector<StackInfo> m_codes;
    Registers             *m_regs{};
    CAddressManager       *m_memory{};

    void drawLine(LINENUMBER lnActualLine) override;
    void paintBkgnd(LPCRECT lpcRect) override;
    void PushEx(RegName rn, Word addr);
    void PopEx(int numPop);
    void UpdateScroll();
    void Push(BYTE code, bool paint);
    void Pop(BYTE code, bool paint);
    void ClearLine(int line);
};

#endif
