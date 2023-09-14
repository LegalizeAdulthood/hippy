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
#include "registerwnd.h"

BEGIN_MESSAGE_MAP(CRegisterWnd, CWnd)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_SETFOCUS()
  ON_WM_KILLFOCUS()
  ON_WM_KEYDOWN()
  ON_WM_ERASEBKGND()
  ON_MESSAGE(WM_REDRAWALL, OnRedrawAll)
END_MESSAGE_MAP()

//calculate the metrics related to selected font
void CRegisterWnd::UpdateMetrics(){
	TEXTMETRIC tm;
	CRect rc;
	CClientDC dc(this);
	dc.GetTextMetrics(&tm);
	GetClientRect(&rc);
	CharWidth = tm.tmMaxCharWidth;
	SideMargin = CharWidth/2;
	CharHeight = tm.tmHeight;
}

void CRegisterWnd::drawRegister(bool bActive, LPCRECT lprc, RegEnum reReg, int regsize, int regval){
	CClientDC dc(this);
	COLORREF cr;
	char buffer[100];
	CRect rc(lprc);
	rc.left -= SideMargin;
	rc.right+= SideMargin;
	

	//set the color of text (red for recently changed registers)
	if(bActive)
		cr = RGB(200,10,20);
	else 
		cr = RGB(0, 0, 0);

	dc.SetTextColor(cr);
	

	//if the register is selected then draw selection rectangle
	if(selReg==reReg){
		dc.FillRect(&rc, &brSelected);
		dc.DrawFocusRect(&rc);
		
	}
	else 
		dc.FillRect(&rc, &brNormal);

	if(regsize==1){//condition code bit
		itoa(regval, buffer, 2);
		dc.TextOut(lprc->left, lprc->top, buffer, 1);
	}
	else if(regsize==2){
		BYTE b = (BYTE) regval;
		Hexer.ByteToHex(b, buffer);
		dc.TextOut(lprc->left, lprc->top, buffer, 2);
	}
	else{
		BYTE b[2];
		ADDRESS ad = (ADDRESS) regval;
		b[0] = ((BYTE*)&ad)[1];
		b[1] = ((BYTE*)&ad)[0];
		Hexer.ByteArrayToHexArray(b,2,buffer);
		dc.TextOut(lprc->left, lprc->top, buffer, 4);
	}
}

void CRegisterWnd::OnRedrawAll(WPARAM wParam, LPARAM lParam){
	Update();
}

//called after each inst. if drawAll is true then (called from paint) all of the registers
//are redrawn in the prevState, else the new states are calculated and only changed ones are
//drawn
void CRegisterWnd::Update(bool drawAll){
	Registers regValChanged;
//	Registers regStChanged;
	COLORREF crAct = RGB(200, 10, 20);
	COLORREF crNorm = RGB(0,0,0);
	CClientDC dc(this);

	if(drawAll){
		//find changed registers
		regValChanged = prevRegsAct;
	}
	else{
		//find the registers that are modified
		regValChanged.a = (prevRegs.a ^ pRegs->a);
		regValChanged.b = (prevRegs.b ^ pRegs->b);
		regValChanged.x = (prevRegs.x ^ pRegs->x);
		regValChanged.sp = (prevRegs.sp ^ pRegs->sp);
		regValChanged.pc = (prevRegs.pc ^ pRegs->pc);
		regValChanged.ccr.c = (prevRegs.ccr.c ^ pRegs->ccr.c);
		regValChanged.ccr.v = (prevRegs.ccr.v ^ pRegs->ccr.v);
		regValChanged.ccr.i = (prevRegs.ccr.i ^ pRegs->ccr.i);
		regValChanged.ccr.h = (prevRegs.ccr.h ^ pRegs->ccr.h);
		regValChanged.ccr.z = (prevRegs.ccr.z ^ pRegs->ccr.z);
		regValChanged.ccr.n = (prevRegs.ccr.n ^ pRegs->ccr.n);
		
		//
	}

	int y = SideMargin * 3 + CharHeight;
	int x = SideMargin * 4 + 15 * CharWidth;
	CPoint pt(0, SideMargin * 2 + CharHeight);

	CRect rc(x, y, x + 2*CharWidth, y + CharHeight);

	if(regValChanged.a || drawAll || prevRegsAct.a){
		drawRegister(regValChanged.a>0, &rc, rgA, 2, pRegs->a);
	}
	rc.OffsetRect(pt);
	if(regValChanged.b || drawAll || prevRegsAct.b){
		drawRegister(regValChanged.b>0, &rc, rgB, 2, pRegs->b);
	}
	rc.OffsetRect(pt);
	rc.right += 2*CharWidth;
	if(regValChanged.pc || drawAll || prevRegsAct.pc){
		drawRegister(regValChanged.pc>0, &rc, rgPC, 4, pRegs->pc);
	}
	rc.OffsetRect(pt);
	if(regValChanged.x || drawAll || prevRegsAct.x){
		drawRegister(regValChanged.x>0, &rc, rgX, 4, pRegs->x);
	}
	rc.OffsetRect(pt);
	if(regValChanged.sp || drawAll || prevRegsAct.sp){
		drawRegister(regValChanged.sp>0, &rc, rgSP, 4, pRegs->sp);
	}
	rc.OffsetRect(pt);
	dc.SetTextColor(RGB(0,0,0));
	dc.TextOut(rc.left, rc.top, "H I N Z V C", 11); 
	rc.OffsetRect(pt);
	rc.right = rc.left + CharWidth;
	if(regValChanged.ccr.h || drawAll || prevRegsAct.ccr.h){
		drawRegister(regValChanged.ccr.h, &rc, rgH, 1, pRegs->ccr.h);
	}
	pt.x = 2 * CharWidth;
	pt.y = 0;

	rc.OffsetRect(pt);
	if(regValChanged.ccr.i || drawAll || prevRegsAct.ccr.i){
		drawRegister(regValChanged.ccr.i, &rc, rgI, 1, pRegs->ccr.i);
	}
	rc.OffsetRect(pt);
	if(regValChanged.ccr.n || drawAll || prevRegsAct.ccr.n){
		drawRegister(regValChanged.ccr.n, &rc, rgN, 1, pRegs->ccr.n);
	}
	rc.OffsetRect(pt);
	if(regValChanged.ccr.z || drawAll || prevRegsAct.ccr.z){
		drawRegister(regValChanged.ccr.z, &rc, rgZ, 1, pRegs->ccr.z);
	}
	rc.OffsetRect(pt);
	if(regValChanged.ccr.v || drawAll || prevRegsAct.ccr.v){
		drawRegister(regValChanged.ccr.v, &rc, rgV, 1, pRegs->ccr.v);
	}
	rc.OffsetRect(pt);
	if(regValChanged.ccr.c || drawAll || prevRegsAct.ccr.c){
		drawRegister(regValChanged.ccr.c, &rc, rgC, 1, pRegs->ccr.c);
	}
	//if this is not a paint call then store the new state
	if(!drawAll){
		prevRegs = *pRegs;
		prevRegsAct = regValChanged;
	}

}

//paint event handler
void CRegisterWnd::OnPaint(){
	CPen newp, *oldp;
	CClientDC dc(this);
	CRect rc;
	PAINTSTRUCT ps;

	BeginPaint(&ps);

	//if the window has got the focus draw a stronger border
	if(GetFocus()==this) 	newp.CreatePen(PS_SOLID, 2, RGB(0,0,0));
	else					newp.CreatePen(PS_SOLID, 1, RGB(0,0,0));

	oldp = dc.SelectObject(&newp);
	GetClientRect(&rc); 

	dc.Rectangle(-1, -1, rc.right, rc.bottom);

	// draw the rectangle around the window
	{
		dc.MoveTo(rc.left+1, rc.top+1);
		dc.LineTo(rc.right-1, rc.top+1);
		dc.LineTo(rc.right-1, rc.bottom-1);
		dc.LineTo(rc.left+1, rc.bottom-1);
		dc.LineTo(rc.left+1, rc.top+1);
	}

	//draw the grids
	newp.DeleteObject();
	newp.CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
	dc.SelectObject(newp);
	int y,x;
	for(int i = 1;i<7;i++){
		y = i*(2*SideMargin+CharHeight);
		dc.MoveTo(SideMargin,  y);
		dc.LineTo(rc.right-SideMargin, y);
	}

	x = 15*CharWidth+2*SideMargin;
	dc.MoveTo(x, SideMargin);
	dc.LineTo(x, rc.bottom-SideMargin);

	//write the table header (register, value) and Reg names
	y = SideMargin;
	dc.SetTextColor(RGB(0,0,0));
	dc.TextOut(SideMargin, y, "Register", 8); 
	dc.TextOut(x+2*SideMargin, y, "Value", 5); y+= 2*SideMargin + CharHeight;
	dc.SetTextColor(RGB(100,100,100));
	dc.TextOut(SideMargin, y, "Accumulator A", 13); y+= 2*SideMargin + CharHeight;
	dc.TextOut(SideMargin, y, "Accumulator B", 13); y+= 2*SideMargin + CharHeight;
	dc.TextOut(SideMargin, y, "Program Counter", 15); y+= 2*SideMargin + CharHeight;
	dc.TextOut(SideMargin, y, "Index Reg.", 10); y+= 2*SideMargin + CharHeight;
	dc.TextOut(SideMargin, y, "Stack Pointer", 13); y+= 2*SideMargin + CharHeight;
	dc.TextOut(SideMargin, y, "Cond. Code Reg.", 15);
	
	dc.SelectObject(oldp);
	newp.DeleteObject();
	Update(true);
	EndPaint(&ps);
}

bool CRegisterWnd::OnEraseBkgnd(CDC *pDC){
	return false;
}

void CRegisterWnd::OnLButtonDown(UINT nFlags, CPoint point){
	SetFocus();
	
	//find selection
	RegEnum re = selReg;
	selReg = rgNone;
	int y = point.y / (CharHeight+SideMargin*2);
	int x = point.x - (SideMargin * 4 + 15 * CharWidth);
	if(y<8 && y>0 && x > 0){
		switch(y){
		case 1: selReg = rgA;   break;
		case 2: selReg = rgB;   break;
		case 3: selReg = rgPC;  break;
		case 4: selReg = rgX;   break;
		case 5: selReg = rgSP;  break;
		case 7:{
			x /= CharWidth * 2;
			switch(x){
			case 0 : selReg = rgH; break;
			case 1 : selReg = rgI; break;
			case 2 : selReg = rgN; break;
			case 3 : selReg = rgZ; break;
			case 4 : selReg = rgV; break;
			case 5 : selReg = rgC; break;
				
			}			   }
		};
	}
	
	if(selReg==rgNone)
		selReg=re;
	else if (re!=selReg)
		Update(true);

	editMask = 0;	//reset the mask
		
}

void CRegisterWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	switch(nChar){
	case VK_UP:
		{
			int i = (int) selReg -1;
			if(i){ 
				if(i>=6) i = 5;
				selReg = (RegEnum)i; 
				Update(true);
			}
			editMask = 0;	//reset the mask
		}
		break;
	case VK_DOWN:
		{
			int i = (int) selReg +1;
			if(i<7){ selReg = (RegEnum)i; Update(true);}
			editMask = 0;	//reset the mask
		}
		break;
	case VK_RIGHT:
		{
			int i = (int) selReg +1;
			if(i>=7 && i<=11){ selReg = (RegEnum)i; Update(true);}
			editMask = 0;	//reset the mask
		}
		break;
	case VK_LEFT:
		{	int i = (int) selReg -1;
			if(i>=6 && i<=11){ selReg = (RegEnum)i; Update(true);}
			editMask = 0;	//reset the mask
		}
	};

	// get the hexadecimal value pressed
	int val=-1;

	if( (nChar >= '0') && (nChar <= '9') ) val = nChar - '0';
	else if ( (nChar >= 'A') && (nChar <= 'F') ) val = nChar - 'A' + 10;

	if(val >= 0){
		if( selReg >= rgH && val < 2){	// bit field ? (CCR bit)
			//edit mask has no meaning, just change the value
			switch(selReg){
			case rgH: pRegs->ccr.h = val; break;
			case rgI: pRegs->ccr.i = val; break;
			case rgN: pRegs->ccr.n = val; break;
			case rgZ: pRegs->ccr.z = val; break;
			case rgV: pRegs->ccr.v = val; break;
			case rgC: pRegs->ccr.c = val; break;
			};

			Update();
		}
		else if ( (selReg < rgH) && val < 16){ // byte or word
			int editSize;
			Word * pwEdit;

			switch(selReg){
			case rgA:  editSize = 2; pwEdit = (Word*)&pRegs->a;  break;
			case rgB:  editSize = 2; pwEdit = (Word*)&pRegs->b;  break;
			case rgPC: editSize = 4; pwEdit = &pRegs->pc; break;
			case rgX:  editSize = 4; pwEdit = &pRegs->x;  break;
			case rgSP: editSize = 4; pwEdit = &pRegs->sp; break;
			
			};

			int m = editSize - editMask - 1;
			Word mask = 0x000f << (m*4);
			mask = ~mask;
			*pwEdit &= mask;			//the portion to be edited is now all zero
			mask = val << (m * 4);
			*pwEdit |= mask;
			Update();
			editMask++;
			if(editMask>=editSize) editMask = 0;


		}
	}
	
}

void CRegisterWnd::OnSetFocus(CWnd *pOldWnd){
	RedrawWindow();
}

void CRegisterWnd::OnKillFocus(CWnd * pNewWnd){
	RedrawWindow();
}

//cosntructor:
//register wnd class, create window and font
CRegisterWnd::CRegisterWnd(CWnd * pParentWnd,CRect &rcPos, PRegisters pRegs, LPCTSTR szWindowName){
	const char * p =
	AfxRegisterWndClass(CS_OWNDC|CS_SAVEBITS, LoadCursor(NULL, IDC_ARROW), NULL, 0);
	Create(p, szWindowName,WS_CHILD | WS_TABSTOP, rcPos, pParentWnd, 1);
	
	this->pRegs = pRegs;

	Font.CreateFont(
		   12,                        // nHeight
		   0,                         // nWidth
		   0,                         // nEscapement
		   0,                         // nOrientation
		   FW_NORMAL,                 // nWeight
		   FALSE,                     // bItalic
		   FALSE,                     // bUnderline
		   0,                         // cStrikeOut
		   ANSI_CHARSET,              // nCharSet
		   OUT_DEFAULT_PRECIS,        // nOutPrecision
		   CLIP_DEFAULT_PRECIS,       // nClipPrecision
		   DEFAULT_QUALITY,           // nQuality
		   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		   _T("FixedSys"));           // lpszFacename
	
	this->SetFont(&Font, false);
	CClientDC dc(this);
	brSelected.CreateSolidBrush(RGB(180,180, 180));
	brRecent.CreateSolidBrush(RGB(255,50,50));
	brNormal.CreateSolidBrush(RGB(255,255,255));
	defFont = dc.SelectObject(&Font);
	dc.SetBkMode(TRANSPARENT);
	UpdateMetrics();
}

CRegisterWnd::~CRegisterWnd(){
	CClientDC dc(this);
	dc.SelectObject(defFont);
	Font.DeleteObject();
	brNormal.DeleteObject();
	brRecent.DeleteObject();
	brSelected.DeleteObject();
}
