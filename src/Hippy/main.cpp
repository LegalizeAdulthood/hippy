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

#include <afx.h>
#include <afxwin.h>
#include <afxdlgs.h>
#include <afxext.h>
#include <afxcmn.h>
#include <afxadv.h>
#include "m6800.h"
#include "addrmng.h"
#include "resource.h"
#include "asmeditor.h"
#include "xmlparser.h"
#include "debugwnd.h"


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////	
/////////////////////////////////////////////////////

class CMainFrame : public CMDIFrameWnd{
private:
	CDebugWnd		*	pDbgWnd;
	CEnvironment		env;
	CToolBar			Toolbar;
	CToolBar			FileToolbar;
	CToolBar			IntToolbar;
	CSemaphore	*		psem_irq;
	CSemaphore  *		psem_nmi;
	CSemaphore	*		psem_reset;
	//
	CAsmEditorWnd	*	GetCurrentEditor();
	void				SendThroughCom(int PortNo, CString fname);
	void				GetExecutablePath(CString & str);
	void				CreateMRUMenu();
	CString				GetDeviceFile();
public:
	CMainFrame();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg int OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	afx_msg int OnDestroy();
	afx_msg int OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg int OnFileOpenClick();
	afx_msg int OnFileCloseClick();
	afx_msg int OnFileNewClick();
	afx_msg int OnFileExitClick();
	afx_msg int OnFileSaveClick();
	afx_msg int OnFileSaveAsClick();
	afx_msg int OnEditCompile();
	afx_msg int OnStepInto();
	afx_msg int OnStepOver();
	afx_msg int OnStepOut();
	afx_msg int OnGo();
	afx_msg int OnPause();
	afx_msg int OnInsBkPt();
	afx_msg int OnWindowMenu();
	afx_msg int OnSendCom1();
	afx_msg int OnSendCom2();
	afx_msg int OnRunToCursor();
	afx_msg int OnLoadSFile();
	afx_msg int OnWriteSFile();
	afx_msg int OnCompileNLoad();
	afx_msg int OnIntReset();
	afx_msg int OnIntNmi();
	afx_msg int OnIntIrq();
	afx_msg int OnNormalPriority();
	afx_msg int OnHighPriority();
	DECLARE_MESSAGE_MAP()
};



BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpenClick)
	ON_COMMAND(ID_FILE_CLOSE, OnFileCloseClick)
	ON_COMMAND(ID_FILE_EXIT, OnFileExitClick)
	ON_COMMAND(ID_FILE_SAVE, OnFileSaveClick)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveAsClick)
	ON_COMMAND(ID_FILE_NEW, OnFileNewClick)
	ON_COMMAND(ID_BTNSTEPINTO, OnStepInto)
	ON_COMMAND(ID_BTNSTEPOVER, OnStepOver)
	ON_COMMAND(ID_BTNSTEPOUT, OnStepOut)
	ON_COMMAND(ID_BTNRUNCURSOR, OnRunToCursor)
	ON_COMMAND(ID_BTNGO, OnGo)
	ON_COMMAND(ID_BTNPAUSE, OnPause)
	ON_COMMAND(ID_INSBRKPT, OnInsBkPt)
	ON_COMMAND(ID_EDIT_COMPILE, OnEditCompile)
	ON_COMMAND(ID_EDIT_SENDPROGRAMTOMP_COM1, OnSendCom1)
	ON_COMMAND(ID_EDIT_SENDPROGRAMTOMP_COM2, OnSendCom2)
	ON_COMMAND(ID_FILE_LOADSFILE, OnLoadSFile)
	ON_COMMAND(ID_MICROCOMPUTER_SRECORDFILE, OnWriteSFile)
	ON_COMMAND(ID_MICROCOMPUTER_GENERATEINTERRUPT_RESET, OnIntReset)
	ON_COMMAND(ID_MICROCOMPUTER_GENERATEINTERRUPT_NMI, OnIntNmi)
	ON_COMMAND(ID_MICROCOMPUTER_GENERATEINTERRUPT_IRQ, OnIntIrq)
	ON_COMMAND(ID_MICROCOMPUTER_EXECUTIONPRIORITY_HIGH, OnHighPriority)
	ON_COMMAND(ID_MICROCOMPUTER_EXECUTIONPRIORITY_NORMAL, OnNormalPriority)

	ON_COMMAND(ID_EDIT_COMPILEANDLOAD, OnCompileNLoad)
END_MESSAGE_MAP()

class CMainApp : public CWinApp{
public:
	int InitInstance(){
		SetRegistryKey(_T("Hippy 6800"));
		LoadStdProfileSettings(10);

		m_pMainWnd = new CMainFrame();
		return TRUE;
	}
};


CMainApp mapp;

void CMainFrame::SendThroughCom(int PortNo, CString fname){
	STARTUPINFO			si;
	PROCESS_INFORMATION pi;
	CString bat, batb;
	
	//port = "COM" + PortNo;
	bat.Format("send%d.bat %s", PortNo, fname);
	batb.Format("bsend%d.bat %s", PortNo, fname);
	
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;
	si.hStdInput = NULL;
	si.cb = sizeof(si);

	CreateProcess( NULL, batb.GetBuffer(1), NULL, NULL, true, CREATE_NEW_CONSOLE /*creation flags*/,
		NULL/*envirn*/, NULL/*cur dir*/, &si, &pi); 

	WaitForSingleObject(pi.hProcess, INFINITE);

	CreateProcess( NULL, bat.GetBuffer(1), NULL, NULL, true, CREATE_NEW_CONSOLE /*creation flags*/,
		NULL/*envirn*/, NULL/*cur dir*/, &si, &pi); 

	WaitForSingleObject(pi.hProcess, INFINITE);
}

int CMainFrame::OnLoadSFile(){
	CFileDialog fd(true,".asm", NULL, OFN_FILEMUSTEXIST);
	if(fd.DoModal()==IDOK){
		pDbgWnd->LoadSFile(fd.GetFileName());
		pDbgWnd->SendMessage(WM_UPDATEDBGWND);
	}
	return 0;
}

int CMainFrame::OnWriteSFile(){
	CSFileDialog fd;
	if(fd.ShowModal()==IDOK){
		Word wBegin, wEnd;
		CString str;
		fd.GetValues(wBegin, wEnd, str);
		int port=0;
		if(str == "COM1") port = 1;
		else if(str == "COM2") port = 2;
		
		if(port){
			CString fname;
			GetExecutablePath(fname);
			fname += "1921abbxx231.hex";
			pDbgWnd->WriteSFile(wBegin, wEnd, fname);
			SendThroughCom(port,fname);
		}
		else
			pDbgWnd->WriteSFile(wBegin, wEnd, str);
	}
	return 0;
}

int CMainFrame::OnNormalPriority(){
	pDbgWnd->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_NORMAL, MF_CHECKED);
	GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_HIGH, MF_UNCHECKED);
	return 0;

}

int CMainFrame::OnHighPriority(){
	pDbgWnd->SetThreadPriority(THREAD_PRIORITY_HIGHEST);
	GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_HIGH, MF_CHECKED);
	GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_NORMAL, MF_UNCHECKED);
	return 0;
}

int CMainFrame::OnIntReset(){
	psem_reset->Unlock();
	return 0;
}

int CMainFrame::OnIntNmi(){
	psem_nmi->Unlock();
	return 0;
}

int CMainFrame::OnIntIrq(){
	psem_irq->Unlock();
	return 0;
}
	

int CMainFrame::OnSendCom1(){
	CAsmEditorWnd * aw = GetCurrentEditor();
	if(aw){
		if(aw->CompileCode()){
			CString str = aw->GetHexFileName();
			SendThroughCom(1, str);
		}
	}
	else MessageBeep(1);

	return 0;
}

int CMainFrame::OnSendCom2(){
		CAsmEditorWnd * aw = GetCurrentEditor();
	if(aw){
		if(aw->CompileCode()){
			CString str = aw->GetHexFileName();
			SendThroughCom(2, str);
		}
	}
	else MessageBeep(1);
	return 0;
}

int CMainFrame::OnCompileNLoad(){
	CAsmEditorWnd * aw = GetCurrentEditor();
	if(aw){
		if(aw->CompileCode()){
			CString str = aw->GetHexFileName();
			pDbgWnd->LoadSFile(str);
		}
	}
	else MessageBeep(1);
	return 0;
}

int CMainFrame::OnEditCompile(){
	CAsmEditorWnd * aw = GetCurrentEditor();
	if(aw){
		aw->CompileCode();
	}
	else MessageBeep(1);
	return 0;
}

int CMainFrame::OnInsBkPt(){
	pDbgWnd->InsBkPt();
	return 0;
}

int CMainFrame::OnPause(){
	pDbgWnd->Stop();
	return 0;
}

int CMainFrame::OnGo(){
	pDbgWnd->Run();
	return 0;
}

int CMainFrame::OnStepInto(){
	pDbgWnd->StepIn();
	return 0;
}

int CMainFrame::OnStepOver(){
	pDbgWnd->StepOver();
	return 0;
}

int CMainFrame::OnStepOut(){
	pDbgWnd->StepOut();
	return 0;
}

int CMainFrame::OnRunToCursor(){
	pDbgWnd->RunToCursor();
	return 0;
}

int CMainFrame::OnFileSaveClick(){
	CAsmEditorWnd *  pAsm =  GetCurrentEditor();
	
	if(pAsm){
		if(pAsm->IsNewFile()) OnFileSaveAsClick();
		else pAsm->Save();
	}
	else MessageBeep(1);
	
	return 0;
}

int CMainFrame::OnFileSaveAsClick(){
	CAsmEditorWnd *  pAsm =  GetCurrentEditor();
	
	if(pAsm){
		CString str;
		pAsm->GetFileName(str);
		CFileDialog fd(false,".asm", str.GetBuffer(1), OFN_OVERWRITEPROMPT);
		if(fd.DoModal()==IDOK){
			mapp.AddToRecentFileList( fd.GetPathName());
			pAsm->SaveAs(fd.GetPathName());
		}
	}
	else MessageBeep(1);
	
	return 0;
}

int CMainFrame::OnFileOpenClick(){
	CFileDialog fd(true,".asm", NULL, OFN_FILEMUSTEXIST);
	if(fd.DoModal()==IDOK){
		CAsmEditorWnd * pAsm;
		pAsm = new CAsmEditorWnd(this, fd.GetPathName());
		mapp.AddToRecentFileList(fd.GetFileName());
		
	}
	return 0;
}

int CMainFrame::OnFileCloseClick(){
	CMDIChildWnd *cw = this->MDIGetActive();
	if(cw){
		CRuntimeClass *pRtc = cw->GetRuntimeClass();
		CString str = "CAsmEditorWnd";
		if(str == pRtc->m_lpszClassName)
		  cw->DestroyWindow();
		else MessageBeep(0);
	}
	else MessageBeep(1);
	return 0;
}

int CMainFrame::OnFileNewClick(){
	CAsmEditorWnd * pAsm;
	pAsm = new CAsmEditorWnd(this, "");
	return 0;
}

int CMainFrame::OnFileExitClick(){
	DestroyWindow();
	return 0;
}

int CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext){
	return CreateClient(lpcs, NULL);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct){
	if(CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

int CMainFrame::OnDestroy(){
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
    AfxGetApp()->WriteProfileBinary("Smart IDE", "WP MainFrame", (LPBYTE)&wp, sizeof(wp));
	SaveBarState("ToolBarState");
	return 0;
}

int CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus){
	CMDIFrameWnd::OnShowWindow(bShow, nStatus);

    if(bShow && !IsWindowVisible())
    {
        WINDOWPLACEMENT *lwp;
        UINT nl;
		if(AfxGetApp()->GetProfileBinary("Smart IDE", "WP MainFrame", (LPBYTE*)&lwp, &nl))
        {
            SetWindowPlacement(lwp);
            delete [] lwp;
        }
    }
	return 0;
}

void CMainFrame::CreateMRUMenu(){
	CMenu * menu = GetMenu();
}

void CMainFrame::GetExecutablePath(CString & str){
	char buffer[256];
	GetModuleFileName(GetModuleHandle(NULL), buffer,256);
	buffer[strlen(buffer)-9] = 0;
	str = buffer;
}

CAsmEditorWnd * CMainFrame::GetCurrentEditor(){
	CAsmEditorWnd * ret = NULL;

	CMDIChildWnd *cw = this->MDIGetActive();
	if(cw){
		CRuntimeClass *pRtc = cw->GetRuntimeClass();
		CString str = "CAsmEditorWnd";
		if(str == pRtc->m_lpszClassName)
			ret = (CAsmEditorWnd *)(CAsmEditorWnd *)cw;
	}
	return ret;
}

	
CString CMainFrame::GetDeviceFile(){
	char buf[256];
	char * p;
	GetModuleFileName(mapp.m_hInstance, buf, 256);
	
	p = strrchr(buf, '\\');
	*p = 0;
	p = strrchr(buf, '\\');
	*p = 0;
	CString str(buf);
	str += "\\devices\\device.xml";
	return str;
}

CMainFrame::CMainFrame(){
	Create(NULL, "Hippy - Motorola 6800 Studio", WS_OVERLAPPEDWINDOW, 
		CRect(100, 100, 800, 800), GetDesktopWindow(),MAKEINTRESOURCE(IDR_MENU1));
	
	CString deviceFile;

	GetExecutablePath(deviceFile);
	deviceFile += "device.xml";
	env.SetDebugWnd(pDbgWnd);
	env.SetMainWnd(this);
	env.SetDeviceFile(GetDeviceFile());
	pDbgWnd = new CDebugWnd(&env);
	
	//toolbar
	Toolbar.Create(this);
	Toolbar.LoadToolBar(IDR_TOOLBAR1);
	Toolbar.EnableDocking(CBRS_ALIGN_ANY);
	
	/**/
	EnableToolTips();
	Toolbar.EnableToolTips();
	Toolbar.SetBarStyle(Toolbar.GetBarStyle() | TBSTYLE_FLAT | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	/**/
	
	FileToolbar.Create(this);
	FileToolbar.LoadToolBar(IDR_TOOLBAR2);
	FileToolbar.EnableDocking(CBRS_ALIGN_ANY);
	FileToolbar.SetBarStyle(FileToolbar.GetBarStyle() | TBSTYLE_FLAT | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
	IntToolbar.Create(this, WS_CHILD | WS_VISIBLE |   CBRS_TOP | CBRS_TOOLTIPS);
	IntToolbar.LoadToolBar(IDR_TOOLBAR_INT);
	IntToolbar.EnableDocking(CBRS_ALIGN_ANY);
	IntToolbar.SetBarStyle(IntToolbar.GetBarStyle() | TBSTYLE_FLAT | 
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&FileToolbar);
	DockControlBar(&Toolbar);
	DockControlBar(&IntToolbar);
	LoadAccelTable(MAKEINTRESOURCE(IDR_ACCELERATOR1));
	LoadBarState("ToolBarState");
	UpdateWindow();
	ShowWindow(SW_SHOW);

	psem_irq	= new CSemaphore(0, 1000, INT_IRQ);
	psem_nmi	= new CSemaphore(0, 1000, INT_NMI);
	psem_reset	= new CSemaphore(0, 1000, INT_RST);
	CreateMRUMenu();
}


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////	
/////////////////////////////////////////////////////


