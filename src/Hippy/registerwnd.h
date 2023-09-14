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
#ifndef _REGISTERWND_H_
#define _REGISTERWND_H_

#include <afxwin.h>
#include "hippy.h"
#include "disassembler.h"

typedef enum RegEnum{
	rgNone, rgA, rgB, rgPC, rgX, rgSP, rgH, rgI, rgN, rgZ, rgV, rgC
} RegEnum; 

class CRegisterWnd : public CWnd{
private:
	CFont		Font, *defFont;
	CBrush		brNormal, brSelected, brRecent;
	int			CharWidth, CharHeight,SideMargin;
	PRegisters  pRegs;			//points to actual processor registers
	Registers	prevRegs;		//previous register states
	Registers	prevRegsAct;	//used as boolean type, true for each element if
								//that element was modified in previous instruction.
	RegEnum		selReg;			//shows the selected reg
	HexDumper	Hexer;
	
								// editing-----------
	Word		editMask;		// mask showing last quadrant edited (like 0x0f)
	
	void drawRegister(bool bActive, LPCRECT lprc, RegEnum reReg, int regsize, int regval);
	void UpdateMetrics();
public:
	//constructor & destructor
	CRegisterWnd(CWnd * pParentWnd,CRect &rcPos, PRegisters pRegs,  LPCTSTR szWindowName=NULL);
	~CRegisterWnd();

	void	Update(bool drawAll=false);
	afx_msg void OnRedrawAll(WPARAM wParam, LPARAM lParam);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnKillFocus(CWnd * pNewWnd);
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg bool OnEraseBkgnd(CDC *pDC);
	DECLARE_MESSAGE_MAP()
};

#endif