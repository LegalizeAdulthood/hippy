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

#include "debugwnd.h"

#include "addrmng.h"
#include "dasmwnd.h"
#include "environment.h"
#include "m6800.h"
#include "memdump.h"
#include "registerwnd.h"

// clang-format off
BEGIN_MESSAGE_MAP(CDebugWnd, CMDIChildWnd)
    ON_WM_KEYDOWN()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_SHOWWINDOW()
    ON_MESSAGE(WM_UPDATEDBGWND, OnUpdateDbgWnd)
END_MESSAGE_MAP()
// clang-format on

bool CDebugWnd::ExecuteNext()
{
    BYTE opcode = m_memory->Read(m_regs->pc);

    // stop if the flag is changed (maybe a pause)
    if (!m_running)
    {
        return false;
    }

    // deal with return stack
    DoReturnStack(opcode);

    m_m6800->Step();
    m_dasm->add_codePt(m_regs->pc);
    // DoStack(m_opcode);

    m_collectedCycles += (g_instDescTbl[opcode] & 0xf0) >> 4;

    if (m_collectedCycles >= 50000)
    {
        DWORD    passed;
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        passed = (ft.dwLowDateTime - m_lastTimeRecorded.dwLowDateTime);
        passed = passed / 10;
        m_collectedCycles -= passed;
        if (m_collectedCycles > 0)
        {
            Sleep(m_collectedCycles / 1000);
            m_collectedCycles = m_collectedCycles % 1000;
        }
        GetSystemTimeAsFileTime(&m_lastTimeRecorded);
    }

    Word w;
    if (m_m6800->MemLocChanged(w))
    {
        m_memDump->SendMessage(WM_MEMLOCCHANGE, w);
    }

    switch (m_stopMode)
    {
    case NONSTOP:
        break;
    case STOP_MEMLOC:
        if (m_regs->pc == m_stopAt)
        {
            m_running = false;
            return false;
        }
        break;
    }

    // check for a breakpoint
    if (m_dasm->IsBrkPoint(m_regs->pc))
    {
        m_running = false;
        return false;
    }

    return true;
}

void CDebugWnd::DoStack(BYTE opcode)
{
    // m_stackWnd->CheckStack(m_opcode, false);
}

void CDebugWnd::DoReturnStack(BYTE opcode)
{
    if (opcode == 0x8d || opcode == 0xad || opcode == 0xbd)
    { // BSR || JSR(IND) || JSR(EXT) ?
        m_retStack = (Word *) realloc(m_retStack, (m_retStackSize + 1) * sizeof(Word));
        m_retStack[m_retStackSize] = (opcode == 0x8d) ? m_regs->pc + 2 : m_regs->pc + 3;
        m_retStackSize++;
    }
    else if (opcode == 0x39)
    {
        m_retStack = (Word *) realloc(m_retStack, (m_retStackSize - 1) * sizeof(Word));
        m_retStackSize--;
    }
}

void CDebugWnd::InsBkPt()
{
    m_dasm->InsertBrkPtHere();
}

UINT ThreadedRun(LPVOID pvParam)
{
    CDebugWnd *dbg = (CDebugWnd *) pvParam;
    int        i = 0;

    while (dbg->ExecuteNext())
    {
    }
    dbg->SendMessage(WM_UPDATEDBGWND);
    return 0;
}

void CDebugWnd::BlockRun()
{
    m_m6800->SetRunMode(0);
    m_running = true;
    m_stackWnd->toggleContinuous(true);
    GetSystemTimeAsFileTime(&m_lastTimeRecorded);
    AfxBeginThread((AFX_THREADPROC) ThreadedRun, (LPVOID) this, m_threadPri);
    m_dasm->LoadProgram(0x0100);
}

void CDebugWnd::Run()
{
    if (!m_running)
    {
        m_stopMode = NONSTOP;
        m_dasm->add_codePt(m_regs->pc);
        BlockRun();
    }
    else
    {
        MessageBeep(1);
    }
}

void CDebugWnd::RunToCursor()
{
    m_stopMode = STOP_MEMLOC;
    m_stopAt = (Word) m_dasm->GetActiveMemLoc();
    BlockRun();
}

void CDebugWnd::StepOver()
{
    BYTE opcode = m_memory->Read(m_regs->pc);
    if (opcode == 0x8d || opcode == 0xad || opcode == 0xbd)
    {
        m_stopMode = STOP_MEMLOC;
        int instLen = g_instDescTbl[opcode] & 0x0F;
        m_stopAt = m_regs->pc + instLen;
        BlockRun();
    }
    else
    {
        m_m6800->Step();
        UpdateAll();
    }
}

void CDebugWnd::StepIn()
{
    if (!m_running)
    {
        BYTE opcode = m_memory->Read(m_regs->pc);
        // deal with return stack
        DoReturnStack(opcode);
        m_m6800->Step();
        m_dasm->add_codePt(m_regs->pc);
        UpdateAll();
    }
}

void CDebugWnd::StepOut()
{
    if (m_running)
    {
        MessageBeep(1);
        return;
    }
    if (m_retStackSize)
    {
        m_stopMode = STOP_MEMLOC;
        m_stopAt = m_retStack[m_retStackSize - 1];
        BlockRun();
    }
}

void CDebugWnd::Stop()
{
    m_running = false;
}

LRESULT CDebugWnd::OnUpdateDbgWnd(WPARAM wParam, LPARAM lParam)
{
    m_stackWnd->toggleContinuous(false);
    UpdateAll();
    return TRUE;
}

BOOL CDebugWnd::OnEraseBkgnd(CDC *pDC)
{
    CRect rc;
    GetClientRect(&rc);
    HRGN hRgn = CreateRectRgn(0, 0, rc.right, rc.top);
    return FALSE;
}

void CDebugWnd::OnSize(UINT nType, int cx, int cy)
{
    CMDIChildWnd::OnSize(nType, cx, cy);
    CRect rc;
    GetClientRect(&rc);
    static const int offset = 10;
    int              aw = cx - 2 * offset;
    int              ah = cy - 2 * offset;

    if (m_dasm)
    {
        int top = rc.top + offset;
        int left = rc.left + offset;
        m_dasm->SetWindowPos(nullptr, left, top, (int) (aw * (m_leftPercent / 100.0f)),
                             (int) (ah * (m_topPercent / 100.0f)), 0);

        top += (int) (ah * (m_topPercent / 100.0f));
        left += (int) (aw * (m_leftPercent / 100.0f));

        m_memDump->SetWindowPos(nullptr, rc.left + offset, top + offset, (int) (aw * (m_leftPercent / 100.0f)),
                                (int) (ah * ((100 - m_topPercent) / 100.0f) - offset), 0);

        m_regWnd->SetWindowPos(nullptr, left + offset, rc.top + offset, (int) (aw * (m_leftPercent / 100.0f) - offset),
                               (int) (ah * ((100 - m_topPercent) / 100.0f) - offset), 0);

        m_stackWnd->SetWindowPos(nullptr, left + offset, top + offset, (int) (aw * (m_leftPercent / 100.0f) - offset),
                                 (int) (ah * ((100 - m_topPercent) / 100.0f) - offset), 0);
    }
}

void CDebugWnd::UpdateAll()
{
    m_m6800->SetRunMode(1);
    m_dasm->SendMessage(WM_REDRAWALL, (WPARAM) m_m6800->GetRegsPtr()->pc);
    m_regWnd->SendMessage(WM_REDRAWALL);
    m_memDump->RedrawWindow();
    m_stackWnd->RedrawWindow();
}

void CDebugWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

int CDebugWnd::LoadSFile(CString &str)
{
    CArray<Word, Word &> arr;
    m_memory->LoadFile(str.GetBuffer(1), arr);
    m_dasm->LoadProgram(0x100);
    m_dasm->RedrawWindow();
    UpdateAll();
    return 0;
}

int CDebugWnd::WriteSFile(Word wBegin, Word wEnd, CString &str)
{
    return m_memory->SaveSFile(str, wBegin, wEnd);
}

void CDebugWnd::OnDestroy()
{
    WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    AfxGetApp()->WriteProfileBinary("Smart IDE", "WP DebugWnd", (LPBYTE) &wp, sizeof(wp));
}

void CDebugWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CMDIChildWnd::OnShowWindow(bShow, nStatus);

    if (bShow && !IsWindowVisible())
    {
        WINDOWPLACEMENT *lwp;
        UINT             nl;

        if (AfxGetApp()->GetProfileBinary("Smart IDE", "WP DebugWnd", (LPBYTE *) &lwp, &nl))
        {
            SetWindowPlacement(lwp);
            delete[] lwp;
        }
    }
}

void CDebugWnd::SetThreadPriority(int priority)
{
    m_threadPri = priority;
}

CDebugWnd::CDebugWnd(CEnvironment *pEnv)
{
    CCR ccr;
    ccr.all = 0xD0;
    CString str;
    m_dasm = nullptr;
    m_memDump = nullptr;
    m_regWnd = nullptr;
    this->m_env = pEnv;

    Create(nullptr, "CPU Window", WS_TABSTOP | WS_CHILD | WS_OVERLAPPEDWINDOW, CRect(100, 100, 820, 740),
           (CMDIFrameWnd *) pEnv->GetMainWnd());
    ShowWindow(SW_SHOW);

    m_stackWnd = new CStackWnd(this, CRect(360, 300, 700, 580));
    m_m6800 = new CM6800(m_stackWnd);
    m_stackWnd->ShowWindow(SW_SHOW);
    m_stackWnd->SetRegisters(m_m6800->GetRegsPtr());
    m_stackWnd->SetMem(m_m6800->GetMemPtr());

    m_threadPri = THREAD_PRIORITY_NORMAL;
    m_retStackSize = 0;
    m_retStack = nullptr;
    m_memory = m_m6800->GetMemPtr();
    m_memory->Create(pEnv);
    m_regs = m_m6800->GetRegsPtr();
    m_leftPercent = 50;
    m_topPercent = 50;

    m_running = false;

    m_regWnd = new CRegisterWnd(this, CRect(360, 10, 700, 290), m_m6800->GetRegsPtr());
    m_regWnd->ShowWindow(SW_SHOW);
    m_regWnd->Update();

    m_memDump = new CMemDumpWnd(this, CRect(10, 300, 350, 580));
    m_memDump->SetMemory(m_m6800->GetMemPtr());
    m_memDump->ShowWindow(SW_SHOW);

    m_dasm = new CDisasmWnd(this, CRect(10, 10, 350, 290));
    CArray<Word, Word &> arr;

    m_dasm->SetMemory(m_m6800->GetMemPtr());
    m_dasm->LoadProgram(/*arr,*/ 0x0100);
    m_dasm->ModifyStyle(0, WS_GROUP);
    m_dasm->ShowWindow(SW_SHOW);

    m_m6800->GetRegsPtr()->sp = 0x1000;
    m_m6800->GetRegsPtr()->pc = 0x0100;
    m_memory->Write(0xfffe, 0xd0);
    m_memory->Write(0xffff, 0x00);

    m_collectedCycles = 0;
    m_lastTimeRecorded.dwLowDateTime = 0;
}

CDebugWnd::~CDebugWnd()
{
    delete m_dasm;
    delete m_stackWnd;
    CMDIChildWnd::DestroyWindow();
}
