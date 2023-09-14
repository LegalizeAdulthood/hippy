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

#ifndef _DEBUGWND_H__
#define _DEBUGWND_H__

#include <afxwin.h>
#include "disassembler.h"
#include "dasmwnd.h"
#include "memdump.h"
#include "registerwnd.h"
#include "stackwnd.h"
#include "hippy.h"
#include "m6800.h"
#include <Winsock2.h>

typedef enum{
	NONSTOP,
	STOP_MEMLOC,
	STOP_OPCODE
} StopMode;

typedef enum{
	TM_ACTUAL,
	TM_BURST,
	TM_IGNORE
} TimeMode;

class CDebugWnd: public CMDIChildWnd{
	FILETIME		  lastTimeRecorded;
	int				  collectedCycles;
	int				  threadPri;
	CDisasmWnd		* pDasm;
	CMemDumpWnd		* pMemDump;
	CRegisterWnd	* pRegWnd;
	CStackWnd		* pStackWnd;
	Registers		* regs;
	CM6800			* pm6800;
	CEnvironment	* pEnv;
	Word			* pwRetStack;
	int				  iRetStackSize;
	CFont Font;
	TimeMode		  timing;
	CAddressManager * memory;
	int				  iLeftPercent;
	int				  iTopPercent;
	Word			  execMode;
	Word			  StopAt;
	StopMode		  stopMode;
	bool			  Running;

	void DoReturnStack(BYTE opcode);
	void DoStack(BYTE opcode);
	void BlockRun();
public:
	CDebugWnd(CEnvironment * pEnv);
	~CDebugWnd();
	bool ExecuteNext();
	void Run();
	void StepOver();
	void StepIn();
	void StepOut();
	void RunToCursor();
	void UpdateAll();
	void Stop();
	void InsBkPt();
	int LoadSFile(CString & str);
	int WriteSFile(Word wBegin, Word wEnd, CString & str);
	void SetThreadPriority(int priority);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnUpdateDbgWnd(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif