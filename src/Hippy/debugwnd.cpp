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

BEGIN_MESSAGE_MAP(CDebugWnd, CMDIChildWnd)
// ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_UPDATEDBGWND, OnUpdateDbgWnd)
END_MESSAGE_MAP()

bool CDebugWnd::ExecuteNext(){
	BYTE opcode = memory->Read(regs->pc);

	//stop if the flag is changed (maybe a pause)
	if(!Running) return false;	

	//deal with return stack 
	DoReturnStack(opcode);

	pm6800->Step();
	pDasm->add_codePt(regs->pc);
	//DoStack(opcode);

	collectedCycles +=(InstDescTbl[opcode] & 0xf0) >> 4;

	if(collectedCycles>=50000){
		DWORD passed;
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		passed = (ft.dwLowDateTime-lastTimeRecorded.dwLowDateTime);
		passed = passed/10;
		collectedCycles -=passed;
		if(collectedCycles>0){
			Sleep(collectedCycles/1000);
			collectedCycles = collectedCycles%1000;
		}
		GetSystemTimeAsFileTime(&lastTimeRecorded);
	}

	Word w;
	if(pm6800->MemLocChanged(w)){
		pMemDump->SendMessage(WM_MEMLOCCHANGE, w);
	}

	switch(stopMode){
	case NONSTOP:
		break;
	case STOP_MEMLOC:
		if(regs->pc == StopAt){
			Running = false;
			return false;
		}
		break;
	}

	//check for a breakpoint
	if(pDasm->IsBrkPoint(regs->pc)){
		Running = false;
		return false;
	}

	return true;
}

void CDebugWnd::DoStack(BYTE opcode){
	//pStackWnd->CheckStack(opcode, false);
}

void CDebugWnd::DoReturnStack(BYTE opcode){
	if(opcode == 0x8d || opcode == 0xad || opcode == 0xbd){//BSR || JSR(IND) || JSR(EXT) ?
		pwRetStack = (Word*)realloc(pwRetStack, (iRetStackSize+1) * sizeof(Word));
		pwRetStack[iRetStackSize] = (opcode==0x8d)?regs->pc+2:regs->pc+3;
		iRetStackSize++;
	}
	else if(opcode == 0x39){
		pwRetStack = (Word*) realloc(pwRetStack, (iRetStackSize-1)*sizeof(Word));
		iRetStackSize--;
	}
	
}

void CDebugWnd::InsBkPt(){
	pDasm->InsertBrkPtHere();
}

UINT ThreadedRun(LPVOID pvParam){
	CDebugWnd  * dbg = (CDebugWnd*) pvParam;
	int i = 0;
	
	while(dbg->ExecuteNext());
	dbg->SendMessage(WM_UPDATEDBGWND);
	return 0;
}


void CDebugWnd::BlockRun(){
	pm6800->SetRunMode(0);
	Running = true;
	pStackWnd->toggleContinuous(true);
	GetSystemTimeAsFileTime(&lastTimeRecorded);
	AfxBeginThread((AFX_THREADPROC)ThreadedRun,(LPVOID) this,threadPri);
	pDasm->LoadProgram(0x0100);
}

void CDebugWnd::Run(){
	if(!Running){
		stopMode = NONSTOP;
		pDasm->add_codePt(regs->pc);
		BlockRun();
	}
	else 
		MessageBeep(1);
}

void CDebugWnd::RunToCursor(){
	stopMode = STOP_MEMLOC;
	StopAt = (Word)pDasm->GetActiveMemLoc();
	BlockRun();
}

void CDebugWnd::StepOver(){
	BYTE opcode = memory->Read(regs->pc);
	if(opcode == 0x8d || opcode == 0xad || opcode == 0xbd){
		stopMode = STOP_MEMLOC;
		int instLen = InstDescTbl[opcode] & 0x0F;
		StopAt = regs->pc + instLen;
		BlockRun();
	}
	else{ 
		pm6800->Step();
		//pStackWnd->CheckStack(opcode);
		UpdateAll();
	}
}

void CDebugWnd::StepIn(){
	if(!Running){
		BYTE opcode = memory->Read(regs->pc);
		//deal with return stack 
		DoReturnStack(opcode);
		pm6800->Step();
		pDasm->add_codePt(regs->pc);
		//pStackWnd->CheckStack(opcode, true);
		UpdateAll();
	}
}

void CDebugWnd::StepOut(){
	if(Running){
		MessageBeep(1);
		return;
	}
	if(iRetStackSize){
		stopMode = STOP_MEMLOC;
		StopAt = pwRetStack[iRetStackSize-1];
		BlockRun();
	}
}

void CDebugWnd::Stop(){
	Running = false;
}

void CDebugWnd::OnUpdateDbgWnd(WPARAM wParam, LPARAM lParam){
	pStackWnd->toggleContinuous(false);
	UpdateAll();
}

BOOL CDebugWnd::OnEraseBkgnd(CDC* pDC){
	CRect rc;
	GetClientRect(&rc);
	HRGN hRgn = CreateRectRgn(0,0,rc.right,rc.top);
	return FALSE;
}

void CDebugWnd::OnSize(UINT nType, int cx, int cy){
	CMDIChildWnd::OnSize(nType, cx, cy);
	CRect rc;
	GetClientRect(&rc);
	static const offset = 10;
	int aw = cx - 2*offset;
	int ah = cy - 2*offset;
	
	if(pDasm){
		int top  = rc.top+offset;
		int left = rc.left+offset;
		pDasm->SetWindowPos(NULL, left, top, 
			(int)(aw * (iLeftPercent/100.0f)),(int)(ah * (iTopPercent/100.0f)),0);
		
		top += (int)(ah * (iTopPercent/100.0f));
		left += (int)(aw * (iLeftPercent/100.0f));
		
		pMemDump->SetWindowPos(NULL, rc.left+offset, top+offset, 
			(int)(aw * (iLeftPercent/100.0f)),(int)(ah * ((100-iTopPercent)/100.0f) - offset),0);

		pRegWnd->SetWindowPos(NULL, left + offset, rc.top+offset, 
			(int)(aw * (iLeftPercent/100.0f) - offset),(int)(ah * ((100-iTopPercent)/100.0f) - offset),0);

		pStackWnd->SetWindowPos(NULL, left + offset, top + offset, 
			(int)(aw * (iLeftPercent/100.0f) - offset), (int)(ah * ((100-iTopPercent)/100.0f) - offset), 0);
	}
}

void CDebugWnd::UpdateAll(){
		pm6800->SetRunMode(1);	
		pDasm->SendMessage(WM_REDRAWALL, (WPARAM)pm6800->GetRegsPtr()->pc);
		pRegWnd->SendMessage(WM_REDRAWALL);
		pMemDump->RedrawWindow();
		pStackWnd->RedrawWindow();
}

void CDebugWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
}

int CDebugWnd::LoadSFile(CString & str){/*!*/
	CArray<Word, Word&> arr;
	memory->LoadFile(str.GetBuffer(1), arr);
	pDasm->LoadProgram(0x100);
	pDasm->RedrawWindow();
	UpdateAll();
	return 0;
}

int CDebugWnd::WriteSFile(Word wBegin, Word wEnd, CString & str){
	return memory->SaveSFile(str, wBegin, wEnd);
}

int CDebugWnd::OnDestroy(){
	WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    AfxGetApp()->WriteProfileBinary("Smart IDE", "WP DebugWnd", (LPBYTE)&wp, sizeof(wp));
    return 0;
}

int CDebugWnd::OnShowWindow(BOOL bShow, UINT nStatus){
	CMDIChildWnd::OnShowWindow(bShow, nStatus);

    if(bShow && !IsWindowVisible())
    {
        WINDOWPLACEMENT *lwp;
        UINT nl;

        if(AfxGetApp()->GetProfileBinary("Smart IDE", "WP DebugWnd", (LPBYTE*)&lwp, &nl))
        {
            SetWindowPlacement(lwp);
            delete [] lwp;
        }
    }
	return 1;
}

void CDebugWnd::SetThreadPriority(int priority) {  
	threadPri = priority;
}

CDebugWnd::CDebugWnd(CEnvironment * pEnv){
	CCR ccr;
	ccr.all = 0xD0;
	CString str;
	pDasm = NULL;
	pMemDump = NULL;
	pRegWnd = NULL;
	this->pEnv = pEnv;

	Create(NULL, "CPU Window",  WS_TABSTOP | WS_CHILD | WS_OVERLAPPEDWINDOW, CRect(100,100,820,740), (CMDIFrameWnd*) pEnv->GetMainWnd());
	ShowWindow(SW_SHOW);

	pStackWnd = new CStackWnd(this, CRect(360, 300, 700, 580));
	pm6800 = new CM6800(pStackWnd);
	pStackWnd->ShowWindow(SW_SHOW);
	pStackWnd->SetRegisters(pm6800->GetRegsPtr());
	pStackWnd->SetMem(pm6800->GetMemPtr());

	threadPri = THREAD_PRIORITY_NORMAL;
	iRetStackSize = 0;
	pwRetStack = NULL;
	memory = pm6800->GetMemPtr();
	memory->Create(pEnv);
	regs = pm6800->GetRegsPtr();
	iLeftPercent = 50;
	iTopPercent  = 50;

	
	Running = false;

	pRegWnd = new CRegisterWnd(this, CRect(360, 10, 700, 290), pm6800->GetRegsPtr());
	pRegWnd->ShowWindow(SW_SHOW);
	pRegWnd->Update();

	

	pMemDump = new CMemDumpWnd(this, CRect(10, 300, 350, 580));
	pMemDump->SetMemory(pm6800->GetMemPtr());
	pMemDump->ShowWindow(SW_SHOW);

	pDasm = new CDisasmWnd(this, CRect(10,10,350,290));
	CArray<Word, Word&> arr;

	//	memory->LoadFile("SNAKE.HEX", arr);
	pDasm->SetMemory(pm6800->GetMemPtr());
	pDasm->LoadProgram(/*arr,*/0x0100);
	pDasm->ModifyStyle(0, WS_GROUP);
	pDasm->ShowWindow(SW_SHOW);


	pm6800->GetRegsPtr()->sp = 0x1000;
	pm6800->GetRegsPtr()->pc = 0x0100;
	memory->Write(0xfffe, 0xd0);
	memory->Write(0xffff, 0x00);

	collectedCycles=0;
	lastTimeRecorded.dwLowDateTime = 0;
}

CDebugWnd::~CDebugWnd(){
	delete pDasm;
	this->DestroyWindow();
}