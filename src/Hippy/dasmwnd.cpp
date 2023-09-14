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

#include "dasmwnd.h"

BEGIN_MESSAGE_MAP(CDisasmWnd, CBaseWnd)
  ON_WM_KEYDOWN()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_ERASEBKGND()
  ON_COMMAND(ID_DASM_GOTO, OnGoto)
  ON_COMMAND(ID_DASM_DISASSEMBLE, OnDasmHere)
  ON_MESSAGE(WM_REDRAWALL, OnRedrawAll)
END_MESSAGE_MAP()

int CDisasmWnd::add_codePt(Word w){
	
	if(code_listing.insert(w)){
	/*	Word m = InstructionPos[w];
		Word t = MemoryAddressLocator[m];
		if(m!=t) code_listing.clearall();
		code_listing.insert(w);
		LoadProgram(0);*/
	}

	return 0;	
}

int CDisasmWnd::OnDasmHere(){
	CInputBox ib;
	if(ib.ShowModal(&CString("Enter address to disassemble from:"))==IDOK){
		Word w = ib.AsWord();
		DasmHere(w);
		SelectedLine = InstructionPos[w];
		lnPageStart = InstructionPos[w];
		RedrawWindow();
	}
	return 0;
	
}

int CDisasmWnd::OnGoto(){
	CInputBox ib;
	
	if(ib.ShowModal()==IDOK){
		CString str;
		Word w = ib.AsWord();
		SelectedLine = InstructionPos[w];
		lnPageStart = InstructionPos[w];
		RedrawWindow();
	}

	return 0;
}

void CDisasmWnd::OnRButtonUp(UINT nFlags, CPoint point){
	CMenu popup;
	CMenu * sub;
	popup.LoadMenu(IDR_DASM_POPUP);
	sub = popup.GetSubMenu(0);
	CRect rc;
	GetWindowRect(&rc);
	sub->TrackPopupMenu( 0, rc.left+point.x, rc.top+point.y,this, CRect(0,0,200,200));
	CBaseWnd::OnRButtonUp(nFlags, point);
}

// returns the screen line number of a point on the client dc
int  CDisasmWnd::GetLineFromPt(int x, int y){
	return (y - SideMargin) / CharHeight;
}

/*
 * Returns the memory location corresponding to
 * selected line in the disassambly view.
 */
int  CDisasmWnd::GetActiveMemLoc(){
	return MemoryAddressLocator[SelectedLine];
}

/*
 * Toggles a breakpoint to the selected line
 * removes if any set, sets if no breakpoint here
 */
void CDisasmWnd::InsertBrkPtHere(){
	Word addr = MemoryAddressLocator[SelectedLine];
	BYTE b = BreakPoints[addr];
	b = ! (bool)b;
	BreakPoints[addr] = b;
	drawLine(SelectedLine);
}

void CDisasmWnd::drawLine(LINENUMBER lnActualNum){
	char buffer[20];
	int ret;
	int x,y;
	CClientDC dc(this);
	CBrush br;
	int screenNum = lnActualNum - lnPageStart;
	Word addr = MemoryAddressLocator[lnActualNum];
	BYTE bp = BreakPoints[addr];
	BYTE b[2];

	//do nothing if line is not visible
	if(screenNum < 0 && screenNum >= numLines) return;

	// if line is selected or active (next inst. to be executed)
	// then handle them here (note that selected overrides active)
	CRect rc;
	GetLineRect(screenNum, &rc);

	if(SelectedLine == lnActualNum && GetFocus() == this){
		dc.FillRect(&rc, &brSelected);
		dc.DrawFocusRect(&rc);
	}
	else if(ActiveLine == lnActualNum)	dc.FillRect(&rc, &brActive);
	else								dc.FillRect(&rc, &brNormal);

	//if there is a breakpoint on this line, draw a small rectangle
	if(bp){
		rc.right = 2 * SideMargin + 4 * CharWidth;
		dc.FillRect(&rc, &brBrkpt);

	}

	dc.MoveTo(SideMargin*2+4*CharWidth, rc.top);
	dc.LineTo(SideMargin*2+4*CharWidth, rc.bottom);
	dc.MoveTo(SideMargin*4+10*CharWidth, rc.top);
	dc.LineTo(SideMargin*4+10*CharWidth, rc.bottom);
	dc.MoveTo(SideMargin*5+26*CharWidth, rc.top);
	dc.LineTo(SideMargin*5+26*CharWidth, rc.bottom);


	y = CharHeight * screenNum + SideMargin;
	
	BYTE buff[3];

	int length = InstructionLength[lnActualNum]&0x0f;
	for(int i=0;i<length;i++)
		buff[i] = pbMemoryBase->Read(MemoryAddressLocator[lnActualNum]+i, true);
	ret = Dasm.Dasm((BYTE*)buff,
					InstructionLength[lnActualNum],
					buffer
					);
	if(ret == INVALID_CODE) ret = 1;
	x = CharWidth*10+5*SideMargin;
	dc.TextOut(x,y, buffer);

	Hexer.ByteArrayToHexArray(buff,
							  ret, buffer
							  );
	x = CharWidth*4+3*SideMargin;
	dc.TextOut(x,y, buffer);
	
	b[0] = (BYTE) (MemoryAddressLocator[lnActualNum] >> 8);
	b[1] = (BYTE) (MemoryAddressLocator[lnActualNum] & 0x00FF); 
	Hexer.ByteArrayToHexArray(b,
							  2, buffer
							  );
	x = SideMargin;
	dc.TextOut(x,y, buffer);
}

void CDisasmWnd::paintBkgnd(LPCRECT lpcRect){
	CClientDC dc(this);
	int x;
	
	x = CharWidth*4+2*SideMargin;
	dc.MoveTo(x, lpcRect->top+1);
	dc.LineTo(x, lpcRect->bottom);

	x += CharWidth*6+2*SideMargin;
	dc.MoveTo(x, lpcRect->top+1);
	dc.LineTo(x, lpcRect->bottom);

	x += CharWidth*16+SideMargin;
	dc.MoveTo(x, lpcRect->top+1);
	dc.LineTo(x, lpcRect->bottom);

}

void CDisasmWnd::OnLButtonDown(UINT nFlags, CPoint point){

	CBaseWnd::OnLButtonDown(nFlags,point);

	// figure out the line that received the click
	int i = GetLineFromPt(point.x, point.y);
	if(i>=numLines) return;
	// i is a screen line, convert it to actual line
	i += lnPageStart;
	if(SelectedLine!=i){
		int olda = SelectedLine;
		SelectedLine = i;
		//firstly, if selected line was shown, we need to redraw it
		drawLine(olda); 
		drawLine(i);
	}
}

void CDisasmWnd::OnRedrawAll(WPARAM wParam, LPARAM lParam){
	Update((Word)wParam);
}

void CDisasmWnd::Update(Word pc){
	Word oldAct = ActiveLine;				// save old line (we will redraw it if it is visible)
	ActiveLine  = InstructionPos[ pc ];		// recent active line is set
	
	//if current active line is not within the screen
	//scroll window, in this case the old line is erased automatically
	if(!isLineVisible(ActiveLine)){
		lnPageStart = ActiveLine;
		//one last thing is to check the location of the line
		//if it is at the extreme end of the memory modify the first line so that 
		//the last line show 0xFFFF
		lnPageStart = (lnPageStart>(totNumLines-numLines) )?(totNumLines-numLines) : lnPageStart;
		RedrawWindow();
	}
	else{
		if(isLineVisible(oldAct)) drawLine(oldAct);
		drawLine(ActiveLine);
	}
}

void CDisasmWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	CBaseWnd::OnKeyDown(nChar, nRepCnt, nFlags);

}


/*
 * returns non-zero if the address is marked with a breakpoint.
 * returns -1 for soft break points (removed after first stop.
 * return 1 for hard break points.
 */
int  CDisasmWnd::IsBrkPoint(Word addr){
	BYTE b = BreakPoints[addr];
	return (int)b;
}

/*
 * sets the higher 4 bits of the line (corresponding the address) so that its recognized as as a breakpoint.
 */
void CDisasmWnd::InsertBreakpoint(ADDRESS addr, bool soft){
	if(soft)
		InstructionLength[ InstructionPos[addr]] |= 0x30;
	else
		InstructionLength[ InstructionPos[addr]] |= 0xF0;
}

/*
 * resets the higer four bits of the line corresponding the address
 */
void CDisasmWnd::RemoveBreakpoint(ADDRESS addr){
	InstructionLength[InstructionPos[addr] ] &= 0x0F;
}

void CDisasmWnd::DasmHere(Word addr){
	
}

/*
 *  LoadProgram
 */
void CDisasmWnd::LoadProgram(Word wStart){
	ADDRESS		addrCur, addrNext;
	int			codelength;
	BYTE		b;
	totNumLines = 0;
	code_listing.GetNextLocation(addrCur, true);
	do{
		code_listing.GetNextLocation(addrNext);
		codelength = addrNext - addrCur;

		while(codelength){
			b = InstDescTbl[pbMemoryBase->Read(addrCur, true)] & 0x0F;	//masking the cycle bits (num bytes left)
			if(b==0){									//no such instruction exists
				b = 1;									//then it's 1 byte long INVALID_INSTRUCTION
			}
			else if(codelength-b<0){					//there is such an inst. but it does not fit here
				for(;codelength;codelength--){			//so the leftover bytes are marked as INVALID
					InstructionLength[totNumLines] = 1;
					MemoryAddressLocator[totNumLines] = addrCur;
					InstructionPos[addrCur] = totNumLines;
					totNumLines++;
					addrCur++;
				}
				break;									//since codelength consumed escape while
			}

			InstructionLength[totNumLines] = b;
			//all memory locations starting from this byte following inst. length bytes,
			//correspond to this line in the window
			for(int i=0;i<b;i++)
				InstructionPos[addrCur+i] = totNumLines;
			MemoryAddressLocator[totNumLines] = addrCur;
			totNumLines++;
			addrCur += b;
			codelength -= b;
		}
		addrCur = addrNext;
	}while(addrCur!=0xFFFF);

	lnPageStart = InstructionPos[wStart];

	//Scrollbar finetuning
	SetScrollRange(SB_VERT, 0, totNumLines - numLines,false);
	SetScrollPos(SB_VERT, lnPageStart);

}


CDisasmWnd::~CDisasmWnd(){
	brBrkpt.DeleteObject();
}

CDisasmWnd::CDisasmWnd(CWnd * pParentWnd,CRect &rcPos, LPCTSTR szWindowName)
: CBaseWnd(pParentWnd, rcPos, szWindowName) {
	memset(BreakPoints, 0, 0x10000); 
	brBrkpt.CreateSolidBrush(RGB(255,0,0));
}
