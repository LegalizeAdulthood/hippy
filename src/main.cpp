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

#include "addrmng.h"
#include "asmeditor.h"
#include "debugwnd.h"
#include "environment.h"
#include "m6800.h"
#include "resource.h"
#include "xmlparser.h"
#include <afx.h>
#include <afxadv.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxext.h>
#include <afxwin.h>

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

class CMainFrame : public CMDIFrameWnd
{
private:
    CDebugWnd   *pDbgWnd;
    CEnvironment env;
    CToolBar     Toolbar;
    CToolBar     FileToolbar;
    CToolBar     IntToolbar;
    CSemaphore  *psem_irq;
    CSemaphore  *psem_nmi;
    CSemaphore  *psem_reset;
    //
    CAsmEditorWnd *GetCurrentEditor();
    void           SendThroughCom(int PortNo, CString fname);
    void           GetExecutablePath(CString &str);
    void           CreateMRUMenu();
    CString        GetDeviceFile();

public:
    CMainFrame();
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg int  OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
    afx_msg void OnDestroy();
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnFileOpenClick();
    afx_msg void OnFileCloseClick();
    afx_msg void OnFileNewClick();
    afx_msg void OnFileExitClick();
    afx_msg void OnFileSaveClick();
    afx_msg void OnFileSaveAsClick();
    afx_msg void OnEditCompile();
    afx_msg void OnStepInto();
    afx_msg void OnStepOver();
    afx_msg void OnStepOut();
    afx_msg void OnGo();
    afx_msg void OnPause();
    afx_msg void OnInsBkPt();
    afx_msg int  OnWindowMenu();
    afx_msg void OnSendCom1();
    afx_msg void OnSendCom2();
    afx_msg void OnRunToCursor();
    afx_msg void OnLoadSFile();
    afx_msg void OnWriteSFile();
    afx_msg void OnCompileNLoad();
    afx_msg void OnIntReset();
    afx_msg void OnIntNmi();
    afx_msg void OnIntIrq();
    afx_msg void OnNormalPriority();
    afx_msg void OnHighPriority();
    DECLARE_MESSAGE_MAP()
};

// clang-format off
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
// clang-format on

class CMainApp : public CWinApp
{
public:
    int InitInstance()
    {
        SetRegistryKey(_T("Hippy 6800"));
        LoadStdProfileSettings(10);

        m_pMainWnd = new CMainFrame();
        return TRUE;
    }
};

CMainApp mapp;

void CMainFrame::SendThroughCom(int PortNo, CString fname)
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    CString             bat, batb;

    // port = "COM" + PortNo;
    bat.Format("send%d.bat %s", PortNo, fname);
    batb.Format("bsend%d.bat %s", PortNo, fname);

    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = NULL;
    si.cb = sizeof(si);

    CreateProcess(NULL, batb.GetBuffer(1), NULL, NULL, true, CREATE_NEW_CONSOLE /*creation flags*/, NULL /*envirn*/,
                  NULL /*cur dir*/, &si, &pi);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CreateProcess(NULL, bat.GetBuffer(1), NULL, NULL, true, CREATE_NEW_CONSOLE /*creation flags*/, NULL /*envirn*/,
                  NULL /*cur dir*/, &si, &pi);

    WaitForSingleObject(pi.hProcess, INFINITE);
}

void CMainFrame::OnLoadSFile()
{
    CFileDialog fd(true, ".asm", NULL, OFN_FILEMUSTEXIST);
    if (fd.DoModal() == IDOK)
    {
        pDbgWnd->LoadSFile(fd.GetFileName());
        pDbgWnd->SendMessage(WM_UPDATEDBGWND);
    }
}

void CMainFrame::OnWriteSFile()
{
    CSFileDialog fd;
    if (fd.ShowModal() == IDOK)
    {
        Word    wBegin, wEnd;
        CString str;
        fd.GetValues(wBegin, wEnd, str);
        int port = 0;
        if (str == "COM1")
            port = 1;
        else if (str == "COM2")
            port = 2;

        if (port)
        {
            CString fname;
            GetExecutablePath(fname);
            fname += "1921abbxx231.hex";
            pDbgWnd->WriteSFile(wBegin, wEnd, fname);
            SendThroughCom(port, fname);
        }
        else
            pDbgWnd->WriteSFile(wBegin, wEnd, str);
    }
}

void CMainFrame::OnNormalPriority()
{
    pDbgWnd->SetThreadPriority(THREAD_PRIORITY_NORMAL);
    GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_NORMAL, MF_CHECKED);
    GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_HIGH, MF_UNCHECKED);
}

void CMainFrame::OnHighPriority()
{
    pDbgWnd->SetThreadPriority(THREAD_PRIORITY_HIGHEST);
    GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_HIGH, MF_CHECKED);
    GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_NORMAL, MF_UNCHECKED);
}

void CMainFrame::OnIntReset()
{
    psem_reset->Unlock();
}

void CMainFrame::OnIntNmi()
{
    psem_nmi->Unlock();
}

void CMainFrame::OnIntIrq()
{
    psem_irq->Unlock();
}

void CMainFrame::OnSendCom1()
{
    CAsmEditorWnd *aw = GetCurrentEditor();
    if (aw)
    {
        if (aw->CompileCode())
        {
            CString str = aw->GetHexFileName();
            SendThroughCom(1, str);
        }
    }
    else
        MessageBeep(1);
}

void CMainFrame::OnSendCom2()
{
    CAsmEditorWnd *aw = GetCurrentEditor();
    if (aw)
    {
        if (aw->CompileCode())
        {
            CString str = aw->GetHexFileName();
            SendThroughCom(2, str);
        }
    }
    else
        MessageBeep(1);
}

void CMainFrame::OnCompileNLoad()
{
    CAsmEditorWnd *aw = GetCurrentEditor();
    if (aw)
    {
        if (aw->CompileCode())
        {
            CString str = aw->GetHexFileName();
            pDbgWnd->LoadSFile(str);
        }
    }
    else
        MessageBeep(1);
}

void CMainFrame::OnEditCompile()
{
    CAsmEditorWnd *aw = GetCurrentEditor();
    if (aw)
    {
        aw->CompileCode();
    }
    else
        MessageBeep(1);
}

void CMainFrame::OnInsBkPt()
{
    pDbgWnd->InsBkPt();
}

void CMainFrame::OnPause()
{
    pDbgWnd->Stop();
}

void CMainFrame::OnGo()
{
    pDbgWnd->Run();
}

void CMainFrame::OnStepInto()
{
    pDbgWnd->StepIn();
}

void CMainFrame::OnStepOver()
{
    pDbgWnd->StepOver();
}

void CMainFrame::OnStepOut()
{
    pDbgWnd->StepOut();
}

void CMainFrame::OnRunToCursor()
{
    pDbgWnd->RunToCursor();
}

void CMainFrame::OnFileSaveClick()
{
    CAsmEditorWnd *pAsm = GetCurrentEditor();

    if (pAsm)
    {
        if (pAsm->IsNewFile())
            OnFileSaveAsClick();
        else
            pAsm->Save();
    }
    else
        MessageBeep(1);
}

void CMainFrame::OnFileSaveAsClick()
{
    CAsmEditorWnd *pAsm = GetCurrentEditor();

    if (pAsm)
    {
        CString str;
        pAsm->GetFileName(str);
        CFileDialog fd(false, ".asm", str.GetBuffer(1), OFN_OVERWRITEPROMPT);
        if (fd.DoModal() == IDOK)
        {
            mapp.AddToRecentFileList(fd.GetPathName());
            pAsm->SaveAs(fd.GetPathName());
        }
    }
    else
        MessageBeep(1);
}

void CMainFrame::OnFileOpenClick()
{
    CFileDialog fd(true, ".asm", NULL, OFN_FILEMUSTEXIST);
    if (fd.DoModal() == IDOK)
    {
        CAsmEditorWnd *pAsm;
        pAsm = new CAsmEditorWnd(this, fd.GetPathName());
        mapp.AddToRecentFileList(fd.GetFileName());
    }
}

void CMainFrame::OnFileCloseClick()
{
    CMDIChildWnd *cw = this->MDIGetActive();
    if (cw)
    {
        CRuntimeClass *pRtc = cw->GetRuntimeClass();
        CString        str = "CAsmEditorWnd";
        if (str == pRtc->m_lpszClassName)
            cw->DestroyWindow();
        else
            MessageBeep(0);
    }
    else
        MessageBeep(1);
}

void CMainFrame::OnFileNewClick()
{
    CAsmEditorWnd *pAsm;
    pAsm = new CAsmEditorWnd(this, "");
}

void CMainFrame::OnFileExitClick()
{
    DestroyWindow();
}

int CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext)
{
    return CreateClient(lpcs, NULL);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    return 0;
}

void CMainFrame::OnDestroy()
{
    WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    AfxGetApp()->WriteProfileBinary("Smart IDE", "WP MainFrame", (LPBYTE) &wp, sizeof(wp));
    SaveBarState("ToolBarState");
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CMDIFrameWnd::OnShowWindow(bShow, nStatus);

    if (bShow && !IsWindowVisible())
    {
        WINDOWPLACEMENT *lwp;
        UINT             nl;
        if (AfxGetApp()->GetProfileBinary("Smart IDE", "WP MainFrame", (LPBYTE *) &lwp, &nl))
        {
            SetWindowPlacement(lwp);
            delete[] lwp;
        }
    }
}

void CMainFrame::CreateMRUMenu()
{
    CMenu *menu = GetMenu();
}

void CMainFrame::GetExecutablePath(CString &str)
{
    char buffer[256];
    GetModuleFileName(GetModuleHandle(NULL), buffer, 256);
    buffer[strlen(buffer) - 9] = 0;
    str = buffer;
}

CAsmEditorWnd *CMainFrame::GetCurrentEditor()
{
    CAsmEditorWnd *ret = NULL;

    CMDIChildWnd *cw = this->MDIGetActive();
    if (cw)
    {
        CRuntimeClass *pRtc = cw->GetRuntimeClass();
        CString        str = "CAsmEditorWnd";
        if (str == pRtc->m_lpszClassName)
            ret = (CAsmEditorWnd *) (CAsmEditorWnd *) cw;
    }
    return ret;
}

CString CMainFrame::GetDeviceFile()
{
    char  buf[256];
    char *p;
    GetModuleFileName(mapp.m_hInstance, buf, 256);

    p = strrchr(buf, '\\');
    *p = 0;
    p = strrchr(buf, '\\');
    *p = 0;
    CString str(buf);
    str += "\\devices\\device.xml";
    return str;
}

CMainFrame::CMainFrame()
{
    Create(NULL, "Hippy - Motorola 6800 Studio", WS_OVERLAPPEDWINDOW, CRect(100, 100, 800, 800), GetDesktopWindow(),
           MAKEINTRESOURCE(IDR_MENU1));

    CString deviceFile;

    GetExecutablePath(deviceFile);
    deviceFile += "device.xml";
    env.SetDebugWnd(pDbgWnd);
    env.SetMainWnd(this);
    env.SetDeviceFile(GetDeviceFile());
    pDbgWnd = new CDebugWnd(&env);

    // toolbar
    Toolbar.Create(this);
    Toolbar.LoadToolBar(IDR_TOOLBAR1);
    Toolbar.EnableDocking(CBRS_ALIGN_ANY);

    /**/
    EnableToolTips();
    Toolbar.EnableToolTips();
    Toolbar.SetBarStyle(Toolbar.GetBarStyle() | TBSTYLE_FLAT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    /**/

    FileToolbar.Create(this);
    FileToolbar.LoadToolBar(IDR_TOOLBAR2);
    FileToolbar.EnableDocking(CBRS_ALIGN_ANY);
    FileToolbar.SetBarStyle(FileToolbar.GetBarStyle() | TBSTYLE_FLAT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    IntToolbar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS);
    IntToolbar.LoadToolBar(IDR_TOOLBAR_INT);
    IntToolbar.EnableDocking(CBRS_ALIGN_ANY);
    IntToolbar.SetBarStyle(IntToolbar.GetBarStyle() | TBSTYLE_FLAT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&FileToolbar);
    DockControlBar(&Toolbar);
    DockControlBar(&IntToolbar);
    LoadAccelTable(MAKEINTRESOURCE(IDR_ACCELERATOR1));
    LoadBarState("ToolBarState");
    UpdateWindow();
    ShowWindow(SW_SHOW);

    psem_irq = new CSemaphore(0, 1000, INT_IRQ);
    psem_nmi = new CSemaphore(0, 1000, INT_NMI);
    psem_reset = new CSemaphore(0, 1000, INT_RST);
    CreateMRUMenu();
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
