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
#ifndef __DASMWND_H__
#define __DASMWND_H__

#include <afxwin.h>
#include "disassembler.h"
#include "basewnd.h"
#include "hippy.h"
#include "resource.h"
#include "dialogs.h"

typedef struct Listnode{
	struct Listnode *	prev;
	struct Listnode *	next;
	Word				loc;	
} Listnode, *PListnode;

class LinkedList{
private:
	Listnode	head;
	PListnode	current;
public:
	LinkedList(){ head.prev = head.next = NULL; head.loc = 0;}
	bool	search(Word loc, PListnode & pnode){
		PListnode p = &head;
		while(p)
			if(p->loc==loc) return true;
			else if(loc>p->loc){
					if(!p->next){
					pnode = p;
					return false;
				}
				p = p->next;
			}
			else{ 
				pnode = p->prev; 
				return false;
			}
			return true;
	}
	bool	insert(Word loc){
		PListnode p;
		if(!search(loc, p)){
			ASSERT(p);
			PListnode n = new Listnode;
			n->loc = loc;
			n->prev = p;
			n->next = p->next;
			p->next = n;
			if(n->next) n->next->prev = n;
			return true;
		}
		return false;
	}
	void clearall(){
		PListnode t,p = head.next;
		while(p){
			t = p;
			p = p->next;
			delete t;
		}
	}
	bool GetNextLocation(Word & wLoc, bool restart = false){
		if(restart){
			current = &head;
		}
		if(!current){
			wLoc = 0xFFFF;
			return false;
		}
		wLoc = current->loc;
		current = current->next;
		return true;
	}
};

class CDisasmWnd : public CBaseWnd{
private:
	LinkedList		code_listing;
	Disassembler	Dasm;
	CMenu *			menu;

	int				ActiveLine;

	CBrush			brBrkpt;
	BYTE			BreakPoints[0x10000];			
	BYTE			InstructionLength[0x10000];		//index is line number, value is the length of corresponding instr.
	Word			InstructionPos[0x10000];		//index is the memory address, value is the line where that address is dasm'ed
	Word			MemoryAddressLocator[0x10000];	//index is the line number, value is the memory address of the 													//first byte disassembled in that line
	
	void paintBkgnd(LPCRECT lpcRect);
	void drawLine(LINENUMBER lnActualNum);

public:
	CDisasmWnd(CWnd * pParentWnd,CRect &rcPos, LPCTSTR szWindowName=NULL);
	~CDisasmWnd();
	
	void InsertBreakpoint(ADDRESS addr, bool soft=false);
	void InsertBrkPtHere();
	void RemoveBreakpoint(ADDRESS addr);
	int  IsBrkPoint(Word addr);

	void DasmHere(Word addr);
	void Update(Word pc);	//pc is updated, active line changed

	int  GetActiveMemLoc();
	int  GetLineFromPt(int x, int y);
	int add_codePt(Word w);
	void LoadProgram(Word wStart);
	afx_msg int OnGoto();
	afx_msg int OnDasmHere();
	afx_msg void OnRedrawAll(WPARAM wParam, LPARAM lParam);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#endif
