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

#ifndef _DEVICE_H__
#define _DEVICE_H__

#include "hippy.h"
#include <afxwin.h>
#include <afxmt.h>
#include <afxtempl.h>


typedef CArray<CString, CString&> CStrArray;
typedef enum TInterrupt{ IRQ, NMI, RESET};

class CDevice{
private:
	CSemaphore	*		psem_irq;
	CSemaphore  *		psem_nmi;
	CSemaphore	*		psem_reset;
protected:
	CWnd	*			m_pParentWnd;
	bool				bDbg;
	CString		*		lpszDeviceName;
	CString		*		lpszLibraryName;
	
	void				Reset(){}
	virtual void		OnInitialize(){}
	void				OnFinalize(){}
	virtual BYTE		OnRead(Word addr){return 0;}
	virtual void		OnWrite(Word addr, BYTE wVal){}
public:
	CDevice();
	~CDevice();

	int Create(CWnd * parentWnd, CString szName);
	void Interrupt(TInterrupt tint);
	bool Read(Word addr, BYTE & val, bool bDbg);
	bool Write(Word addr, BYTE val, bool bDbg);

	void GetDeviceName(CString & str){ str = *lpszDeviceName;}
	void GetLibraryName(CString & str){ str = *lpszLibraryName;}

};

typedef CDevice *(*pdevFunctv)();

#endif