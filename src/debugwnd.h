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

#ifndef HIPPY_DEBUGWND_H
#define HIPPY_DEBUGWND_H

#include "hippy.h"

#include <Winsock2.h>
#include <afxwin.h>

class CAddressManager;
class CDisasmWnd;
class CEnvironment;
class CM6800;
class CMemDumpWnd;
class CRegisterWnd;
class CStackWnd;

struct Registers;

enum StopMode
{
    NONSTOP,
    STOP_MEMLOC,
    STOP_OPCODE
};

enum TimeMode
{
    TM_ACTUAL,
    TM_BURST,
    TM_IGNORE
};

class CDebugWnd : public CMDIChildWnd
{
public:
    CDebugWnd(CEnvironment *pEnv);
    ~CDebugWnd() override = default;

    bool            ExecuteNext();
    void            Run();
    void            StepOver();
    void            StepIn();
    void            StepOut();
    void            RunToCursor();
    void            UpdateAll();
    void            Stop();
    void            InsBkPt();
    int             LoadSFile(CString &str);
    int             WriteSFile(Word wBegin, Word wEnd, CString &str);
    void            SetThreadPriority(int priority);
    afx_msg void    OnDestroy();
    afx_msg void    OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg BOOL    OnEraseBkgnd(CDC *pDC);
    afx_msg void    OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    afx_msg LRESULT OnUpdateDbgWnd(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

private:
    FILETIME         m_lastTimeRecorded{};
    int              m_collectedCycles{};
    int              m_threadPri{};
    CDisasmWnd      *m_dasm{};
    CMemDumpWnd     *m_memDump{};
    CRegisterWnd    *m_regWnd{};
    CStackWnd       *m_stackWnd{};
    Registers       *m_regs{};
    CM6800          *m_m6800{};
    CEnvironment    *m_env{};
    Word            *m_retStack{};
    int              m_retStackSize{};
    CFont            m_font;
    TimeMode         m_timing{};
    CAddressManager *m_memory{};
    int              m_leftPercent{};
    int              m_topPercent{};
    Word             m_execMode{};
    Word             m_stopAt{};
    StopMode         m_stopMode{};
    bool             m_running{};

    void DoReturnStack(BYTE opcode);
    void DoStack(BYTE opcode);
    void BlockRun();
};

#endif
