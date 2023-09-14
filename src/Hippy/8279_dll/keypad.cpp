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

#include "keypad.h"

CFifo::CFifo(){
	ClearFifo();
}

bool CFifo::IsFifoFull(){
	return GetNumChar() == 7;
}

bool CFifo::IsFifoEmpty(){
	return fifoHead == fifoTail;
}

void CFifo::ClearFifo(){
	fifoHead = fifoTail = 0;
}

BYTE CFifo::RemoveFromFifo(){
	BYTE ret = 0;
	if(!IsFifoEmpty()){
		ret = buffer[fifoHead];
		fifoHead = (fifoHead  + 1) % 8;
	}
	else 
		underflow = true;
	return ret;
}

int  CFifo::GetNumChar(){ 
	int i = (fifoTail - fifoHead);
	if(i<0) i = i+8;
	return (i % 8);
}
	
void CFifo::InsertIntoFifo(BYTE val){
	if(IsFifoFull())
		overflow = true;
	buffer[fifoTail] = val;
	fifoTail = (1+fifoTail)%8;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CKeyPad, CFrameWnd)
  ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void CKeyPad::OnPaint(){
	PAINTSTRUCT ps;
	BeginPaint(&ps);
		CDC BmpDC;
		CClientDC dc(this);
		CRect rc;

		GetClientRect(&rc);
		dc.FillRect(rc, &brBoard);
		DrawDisplay();
		BmpDC.CreateCompatibleDC(NULL);
		BmpDC.SelectObject(&bmp);
		dc.BitBlt( rcKeyPad.left,rcKeyPad.top,rcKeyPad.Width(),rcKeyPad.Height(), &BmpDC, 0,0,SRCCOPY);
		BmpDC.DeleteDC();
	EndPaint(&ps);
}

void CKeyPad::OnLButtonDown(UINT nFlags, CPoint point){
	
	
	if(PtInRect(&rcKeyPad, point)){
		point.x -= rcKeyPad.left;
		point.y -= rcKeyPad.top;
		int row  = point.x / 35;
		int col  = point.y / 35;
		if(row>5) return;
		if(col>3) return;
		int code;
		code = col * 10 + row;
		if(code==20 || code == 30) return;
		PushKey(row, col);
		ptDownKey.x = row;
		ptDownKey.y = col;
		if(KeyMap[code]==0xff);
		else if(KeyMap[code] ==0xbb){
			switch(code){
			case 0x00:/*reset*/
				pdevParent->Interrupt(RESET);
				break;
			case 0x0a:/*irq*/
				pdevParent->Interrupt(IRQ);
				break;
			}
		}
		else{
			/*num pad key*/
			if(FifoRAM.IsFifoFull()){
				//buffer overflow
			}
			FifoRAM.InsertIntoFifo(KeyMap[code]);
		}
		
		DrawDisplay();
	}
}

void CKeyPad::OnLButtonUp(UINT nFlags, CPoint point){
	if(ptDownKey.x != -1){
		RestoreKey(ptDownKey.x, ptDownKey.y);
		ptDownKey.x = -1;
	}
}

void CKeyPad::GetKeyRect(CPoint point, CRect & rc){
	rc.left   = (key_spacing + key_size)* point.x + key_spacing;
	rc.top    = (key_spacing + key_size)* point.y + key_spacing;
	rc.right  = rc.left + key_size;
	rc.bottom = rc.top + key_size;
	
}

void CKeyPad::PushKey(int row, int col){
	CRect rc;
	CClientDC dc(this);

	GetKeyRect(CPoint(row, col), rc);
	rc.OffsetRect(CPoint(rcKeyPad.left, rcKeyPad.top));
	CPen pen,*oldp;
	pen.CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
	oldp = dc.SelectObject(&pen);
	dc.MoveTo(rc.right, rc.top);
	dc.LineTo(rc.right, rc.bottom);
	dc.LineTo(rc.left, rc.bottom);
	pen.DeleteObject();
	pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	dc.SelectObject(&pen);
	dc.LineTo(rc.left, rc.top);
	dc.LineTo(rc.right, rc.top);
	dc.SelectObject(oldp);
	pen.DeleteObject();
}

void CKeyPad::RestoreKey(int row, int col){
	CRect rc;
	CClientDC dc(this);
	CDC BmpDC;

	GetKeyRect(CPoint(row, col), rc);
	BmpDC.CreateCompatibleDC(NULL);
	BmpDC.SelectObject(&bmp);
	dc.BitBlt( rcKeyPad.left + rc.left,rcKeyPad.top + rc.top,
		key_size+3,key_size+3, &BmpDC, rc.left,rc.top,SRCCOPY);
	BmpDC.DeleteDC();
}

void CKeyPad::DrawDisplay(){
	if(IsWindow(this->m_hWnd)){
		CClientDC dc(this);
		dc.FillRect(&rcDisplay, &brBack);
		for(int i=0;i<8;i++)
			Draw7Segment(
				rcDisplay.left + disp_spacing + i*(disp_width + disp_spacing),
				rcDisplay.top + disp_spacing,DispRAM[i]);  
	}
}

void CKeyPad::Draw7Segment(int x, int y, BYTE val){
	int offsety = led_height + led_width + 2 * led_spacing;
	CClientDC dc(this);
	//BYTE	den = 0x80;
	CRect rc;
	CBrush * br;

	if(val & 0x01) br = &brLight; else br = &brDark;
	rc.left   = x+led_height;
	rc.right  = rc.left + led_width;
	rc.top    = y;
	rc.bottom = y + led_height;
	dc.FillRect(&rc, br);

	if(val & 0x40) br = &brLight; else br = &brDark;
	rc.OffsetRect(CPoint(0, offsety));
	dc.FillRect(&rc, br);

	if(val & 0x08) br = &brLight; else br = &brDark;
	rc.OffsetRect(CPoint(0, offsety));
	dc.FillRect(&rc, br);

	if(val & 0x20) br = &brLight; else br = &brDark;
	rc.left   = x;
	rc.top    = y + led_height + led_spacing;
	rc.right  = x + led_height;
	rc.bottom = rc.top + led_width;
	dc.FillRect(&rc, br);

	if(val & 0x10) br = &brLight; else br = &brDark;
	rc.OffsetRect(CPoint(0, offsety));
	dc.FillRect(&rc, br);

	if(val & 0x04) br = &brLight; else br = &brDark;
	rc.OffsetRect(CPoint(led_width + led_height, 0));
	dc.FillRect(&rc, br);

	if(val & 0x02) br = &brLight; else br = &brDark;
	rc.OffsetRect(CPoint(0, - offsety));
	dc.FillRect(&rc, br);	
}
	
void CKeyPad::ClearDisplay(){
	for(int i=0;i<8;i++)
		DispRAM[i] = 0;
}

void CKeyPad::ClearFifoRAM(){
	FifoRAM.ClearFifo();
}
void CKeyPad::SetDisplayValue(int segment, BYTE wVal){
	DispRAM[segment] = wVal;
	DrawDisplay();
}

CKeyPad::CKeyPad(CDevice * pdevParent, CWnd * parent){
 	 
	brLight.CreateSolidBrush(RGB(255, 0, 0));
	brDark.CreateSolidBrush(RGB(160,0,0));
	brBack.CreateSolidBrush(RGB(170,0,0));
	brBoard.CreateSolidBrush(RGB(50,140, 50));
	rcDisplay.left   = 5;
	rcDisplay.top    = 5;
	rcDisplay.right  = rcDisplay.left + 9 * disp_spacing + 8 * disp_width;
	rcDisplay.bottom = rcDisplay.top + 2 * disp_spacing + 3 * led_spacing +
		2 * led_width + 3 * led_height;
	
	rcKeyPad.left = rcDisplay.left;
	rcKeyPad.top  = rcDisplay.bottom + disp_spacing;
	rcKeyPad.bottom = rcKeyPad.top + keyboard_height;
	rcKeyPad.right = rcKeyPad.left + keyboard_width;

	int x,y;
	x = y = 200;

	pParent = parent;
	
	Create(NULL, "KeyPad", WS_SYSMENU|WS_VISIBLE, 
		CRect(x,y, x + rcDisplay.right + 20 , y + rcKeyPad.bottom + disp_spacing + 35), 
		pParent, NULL, WS_EX_TOOLWINDOW);
	
	HANDLE hBmp = LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BMP_KEYPAD), IMAGE_BITMAP, 0, 0, 
		LR_CREATEDIBSECTION|LR_DEFAULTSIZE);
	bmp.Attach((HBITMAP)hBmp);
	

	ClearDisplay();
	
	ClearFifoRAM();
	ptDownKey.x = -1;
	this->pdevParent = pdevParent;
}

//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////

void CIntel8279::OnWrite(Word addr, BYTE bVal){
	int i;
	//if A0 input is high then this is a command
	if(addr){ //COMMAND
		//Most significant 3 bits specifies the command
		switch( (bVal & 0xe0) >> 5){
		case 0:	//MODE SET
			dispMode = (DisplayMode) (bVal & 0x18);
			keyMode  = (KeyboardMode) (bVal & 0x07);
			break;
		case 1:
			break;
		case 2:	//READ FIFO
			readFrom = FIFO;
			break;
		case 3://READ DISPLAY
			readFrom = DISPLAY;
			addrRW =   bVal & 0x0f;
			AutoInc = (bVal & 0x10) != 0;
			break;
		case 4://WRITE DISPLAY
			writeTo = DISPLAY;
			addrRW =   bVal & 0x0f;
			AutoInc = (bVal & 0x10) != 0;	
			break;
		case 5:	//BLANKING
			break;
		case 6://CLEAR
			if(bVal & 0x01){	//CLEAR ALL
				pKeyPad->ClearDisplay();
				pKeyPad->ClearFifoRAM();
				//& clear fifo status
				status = 0;
			}
			else{
				if(bVal & 0x02)
					pKeyPad->ClearFifoRAM();
				if(bVal & 0x10)
					switch(bVal & 0x0c){
					case 0x08:
						for(i=0;i<8;i++) pKeyPad->DispRAM[i] = 0xff;
						break;
					case 0x0c:
						for(i=0;i<8;i++) pKeyPad->DispRAM[i] = 0x20;
						break;
					default:
						pKeyPad->ClearDisplay();
						break;
					}
			}

			break;
		case 7:
			if(bVal & 0x10){
				errMode = true;
			}
			break;
		}
	}
	else{//DATA WRITE -- TO DISPLAY RAM
		pKeyPad->SetDisplayValue(addrRW, bVal);
		if(AutoInc)
			if(dispMode == dm8x8bitLeft)
				addrRW = (addrRW - 1) % 8;
			else
				addrRW = (addrRW + 1) % 8;
	}
}

BYTE CIntel8279::OnRead(Word addr){
	if(bDbg) return 0;
	BYTE res=0;
	if(addr==0x00){//read data
		if(readFrom == FIFO){
			res = pKeyPad->FifoRAM.RemoveFromFifo();
		}
		else{
			res = 	pKeyPad->DispRAM[addrRW];
			if(AutoInc){
				if(dispMode == dm8x8bitLeft)
					addrRW = (addrRW - 1) % 8;
				else
					addrRW = (addrRW + 1) % 8;
			}
		}
	}
	else{//status
		status = 0;
		//last 3 bits number of chars in fifo
		res = (status & 0xf0) + pKeyPad->FifoRAM.GetNumChar();
		//4th bit --> fifo full?
		if(pKeyPad->FifoRAM.IsFifoFull())
			res |= 0x080;
		if(pKeyPad->FifoRAM.IsOverFlow())
			res |= 0x20;
		if(pKeyPad->FifoRAM.IsUnderFlow())
			res |= 0x10;
	}
	return res;
}

void CIntel8279::Reset(){
	dispMode = dm8x8bitLeft;
	keyMode = kmEncNKeyRoll;
	errMode = false;
	AutoInc = false;
}

void CIntel8279::OnInitialize(){
	TRACE0("Generated KeyPad.\n");
	pKeyPad = new CKeyPad(this, m_pParentWnd);
}