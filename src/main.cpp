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
#include "dialogs.h"
#include "environment.h"
#include "m6800.h"
#include "resource.h"

#include <afx.h>
#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxext.h>
#include <afxmt.h>
#include <afxwin.h>

#include <wx/wx.h>

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

class CMainFrame : public CMDIFrameWnd
{
public:
    CMainFrame();
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg int  OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext) override;
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

private:
    CDebugWnd   *m_debugWnd{};
    CEnvironment m_env;
    CToolBar     m_toolbar;
    CToolBar     m_fileToolbar;
    CToolBar     m_intToolbar;
    CSemaphore  *m_irq{};
    CSemaphore  *m_nmi{};
    CSemaphore  *m_reset{};

    CAsmEditorWnd *GetCurrentEditor();
    void           SendThroughCom(int PortNo, const wxString &fileName);
    void           GetExecutablePath(wxString &path);
    void           CreateMRUMenu();
    wxString       GetDeviceFile() const;
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
    int InitInstance() override
    {
        SetRegistryKey(_T("Hippy 6800"));
        LoadStdProfileSettings(10);

        m_pMainWnd = new CMainFrame();
        return TRUE;
    }
};

CMainApp mapp;

void CMainFrame::SendThroughCom(int PortNo, const wxString &fileName)
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    CString             bat;
    CString             batb;

    // port = "COM" + PortNo;
    bat.Format(_T("send%d.bat %s"), PortNo, LPCTSTR(fileName));
    batb.Format(_T("bsend%d.bat %s"), PortNo, LPCTSTR(fileName));

    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = nullptr;
    si.cb = sizeof(si);

    CreateProcess(nullptr, batb.GetBuffer(1), nullptr, nullptr, true, CREATE_NEW_CONSOLE /*creation flags*/,
                  nullptr /*envirn*/, nullptr /*cur dir*/, &si, &pi);

    WaitForSingleObject(pi.hProcess, INFINITE);

    CreateProcess(nullptr, bat.GetBuffer(1), nullptr, nullptr, true, CREATE_NEW_CONSOLE /*creation flags*/,
                  nullptr /*envirn*/, nullptr /*cur dir*/, &si, &pi);

    WaitForSingleObject(pi.hProcess, INFINITE);
}

void CMainFrame::OnLoadSFile()
{
    CFileDialog fd(true, _T(".asm"), nullptr, OFN_FILEMUSTEXIST);
    if (fd.DoModal() == IDOK)
    {
        m_debugWnd->LoadSFile(wxString(LPCTSTR(fd.GetFileName())));
        m_debugWnd->SendMessage(WM_UPDATEDBGWND);
    }
}

void CMainFrame::OnWriteSFile()
{
    CSFileDialog fd;
    if (fd.ShowModal() == IDOK)
    {
        Word    wBegin, wEnd;
        wxString value;
        fd.GetValues(wBegin, wEnd, value);
        value.MakeUpper();
        int port = 0;
        if (value == _T("COM1"))
        {
            port = 1;
        }
        else if (value == _T("COM2"))
        {
            port = 2;
        }

        if (port)
        {
            wxString fname;
            GetExecutablePath(fname);
            fname += _T("1921abbxx231.hex");
            m_debugWnd->WriteSFile(fname, wBegin, wEnd);
            SendThroughCom(port, fname);
        }
        else
        {
            m_debugWnd->WriteSFile(value, wBegin, wEnd);
        }
    }
}

void CMainFrame::OnNormalPriority()
{
    m_debugWnd->SetThreadPriority(THREAD_PRIORITY_NORMAL);
    GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_NORMAL, MF_CHECKED);
    GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_HIGH, MF_UNCHECKED);
}

void CMainFrame::OnHighPriority()
{
    m_debugWnd->SetThreadPriority(THREAD_PRIORITY_HIGHEST);
    GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_HIGH, MF_CHECKED);
    GetMenu()->CheckMenuItem(ID_MICROCOMPUTER_EXECUTIONPRIORITY_NORMAL, MF_UNCHECKED);
}

void CMainFrame::OnIntReset()
{
    m_reset->Unlock();
}

void CMainFrame::OnIntNmi()
{
    m_nmi->Unlock();
}

void CMainFrame::OnIntIrq()
{
    m_irq->Unlock();
}

void CMainFrame::OnSendCom1()
{
    CAsmEditorWnd *aw = GetCurrentEditor();
    if (aw)
    {
        if (aw->CompileCode())
        {
            SendThroughCom(1, aw->GetHexFileName());
        }
    }
    else
    {
        MessageBeep(1);
    }
}

void CMainFrame::OnSendCom2()
{
    CAsmEditorWnd *aw = GetCurrentEditor();
    if (aw)
    {
        if (aw->CompileCode())
        {
            SendThroughCom(2, aw->GetHexFileName());
        }
    }
    else
    {
        MessageBeep(1);
    }
}

void CMainFrame::OnCompileNLoad()
{
    CAsmEditorWnd *aw = GetCurrentEditor();
    if (aw)
    {
        if (aw->CompileCode())
        {
            wxString str = aw->GetHexFileName();
            m_debugWnd->LoadSFile(str);
        }
    }
    else
    {
        MessageBeep(1);
    }
}

void CMainFrame::OnEditCompile()
{
    CAsmEditorWnd *aw = GetCurrentEditor();
    if (aw)
    {
        aw->CompileCode();
    }
    else
    {
        MessageBeep(1);
    }
}

void CMainFrame::OnInsBkPt()
{
    m_debugWnd->InsBkPt();
}

void CMainFrame::OnPause()
{
    m_debugWnd->Stop();
}

void CMainFrame::OnGo()
{
    m_debugWnd->Run();
}

void CMainFrame::OnStepInto()
{
    m_debugWnd->StepIn();
}

void CMainFrame::OnStepOver()
{
    m_debugWnd->StepOver();
}

void CMainFrame::OnStepOut()
{
    m_debugWnd->StepOut();
}

void CMainFrame::OnRunToCursor()
{
    m_debugWnd->RunToCursor();
}

void CMainFrame::OnFileSaveClick()
{
    CAsmEditorWnd *pAsm = GetCurrentEditor();

    if (pAsm)
    {
        if (pAsm->IsNewFile())
        {
            OnFileSaveAsClick();
        }
        else
        {
            pAsm->Save();
        }
    }
    else
    {
        MessageBeep(1);
    }
}

void CMainFrame::OnFileSaveAsClick()
{
    CAsmEditorWnd *pAsm = GetCurrentEditor();

    if (pAsm)
    {
        wxString str;
        pAsm->GetFileName(str);
        CFileDialog fd(false, _T(".asm"), str, OFN_OVERWRITEPROMPT);
        if (fd.DoModal() == IDOK)
        {
            mapp.AddToRecentFileList(fd.GetPathName());
            pAsm->SaveAs(wxString(LPCTSTR(fd.GetPathName())));
        }
    }
    else
    {
        MessageBeep(1);
    }
}

void CMainFrame::OnFileOpenClick()
{
    CFileDialog fd(true, _T(".asm"), nullptr, OFN_FILEMUSTEXIST);
    if (fd.DoModal() == IDOK)
    {
        CAsmEditorWnd *pAsm = new CAsmEditorWnd(this, fd.GetPathName());
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
        {
            cw->DestroyWindow();
        }
        else
        {
            MessageBeep(0);
        }
    }
    else
    {
        MessageBeep(1);
    }
}

void CMainFrame::OnFileNewClick()
{
    CAsmEditorWnd *pAsm = new CAsmEditorWnd(this, _T(""));
}

void CMainFrame::OnFileExitClick()
{
    DestroyWindow();
}

int CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext)
{
    return CreateClient(lpcs, nullptr);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    return CMDIFrameWnd::OnCreate(lpCreateStruct) == -1 ? -1 : 0;
}

void CMainFrame::OnDestroy()
{
    WINDOWPLACEMENT wp;
    GetWindowPlacement(&wp);
    AfxGetApp()->WriteProfileBinary(_T("Smart IDE"), _T("WP MainFrame"), (LPBYTE) &wp, sizeof(wp));
    SaveBarState(_T("ToolBarState"));
    delete m_reset;
    delete m_nmi;
    delete m_irq;
}

void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CMDIFrameWnd::OnShowWindow(bShow, nStatus);

    if (bShow && !IsWindowVisible())
    {
        WINDOWPLACEMENT *lwp;
        UINT             nl;
        if (AfxGetApp()->GetProfileBinary(_T("Smart IDE"), _T("WP MainFrame"), (LPBYTE *) &lwp, &nl))
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

void CMainFrame::GetExecutablePath(wxString &path)
{
    TCHAR buffer[256];
    GetModuleFileName(GetModuleHandle(nullptr), buffer, 256);
    buffer[_tcslen(buffer) - 9] = 0;
    path = buffer;
}

CAsmEditorWnd *CMainFrame::GetCurrentEditor()
{
    CAsmEditorWnd *ret = nullptr;

    CMDIChildWnd *cw = this->MDIGetActive();
    if (cw)
    {
        CRuntimeClass *pRtc = cw->GetRuntimeClass();
        CString        str = "CAsmEditorWnd";
        if (str == pRtc->m_lpszClassName)
        {
            ret = (CAsmEditorWnd *) (CAsmEditorWnd *) cw;
        }
    }
    return ret;
}

wxString CMainFrame::GetDeviceFile() const
{
    TCHAR buf[256];
    TCHAR *p;
    GetModuleFileName(nullptr, buf, 256);

    p = _tcsrchr(buf, _T('\\'));
    *p = 0;
    p = _tcsrchr(buf, _T('\\'));
    *p = 0;
    wxString str(buf);
    str += _T("\\devices\\device.xml");
    return str;
}

CMainFrame::CMainFrame()
{
    CFrameWnd::Create(nullptr, _T("Hippy - Motorola 6800 Studio"), WS_OVERLAPPEDWINDOW, CRect(100, 100, 800, 800),
                      GetDesktopWindow(), MAKEINTRESOURCE(IDR_MENU1));

    m_env.SetMainWnd(this);
    m_env.SetDeviceFile(GetDeviceFile());
    m_debugWnd = new CDebugWnd(&m_env);

    // toolbar
    m_toolbar.Create(this);
    m_toolbar.LoadToolBar(IDR_TOOLBAR1);
    m_toolbar.EnableDocking(CBRS_ALIGN_ANY);

    /**/
    EnableToolTips();
    m_toolbar.EnableToolTips();
    m_toolbar.SetBarStyle(m_toolbar.GetBarStyle() | TBSTYLE_FLAT | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    /**/

    m_fileToolbar.Create(this);
    m_fileToolbar.LoadToolBar(IDR_TOOLBAR2);
    m_fileToolbar.EnableDocking(CBRS_ALIGN_ANY);
    m_fileToolbar.SetBarStyle(m_fileToolbar.GetBarStyle() | TBSTYLE_FLAT | CBRS_TOOLTIPS | CBRS_FLYBY |
                              CBRS_SIZE_DYNAMIC);

    m_intToolbar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS);
    m_intToolbar.LoadToolBar(IDR_TOOLBAR_INT);
    m_intToolbar.EnableDocking(CBRS_ALIGN_ANY);
    m_intToolbar.SetBarStyle(m_intToolbar.GetBarStyle() | TBSTYLE_FLAT | CBRS_TOOLTIPS | CBRS_FLYBY |
                             CBRS_SIZE_DYNAMIC);

    EnableDocking(CBRS_ALIGN_ANY);
    DockControlBar(&m_fileToolbar);
    DockControlBar(&m_toolbar);
    DockControlBar(&m_intToolbar);
    LoadAccelTable(MAKEINTRESOURCE(IDR_ACCELERATOR1));
    LoadBarState(_T("ToolBarState"));
    UpdateWindow();
    ShowWindow(SW_SHOW);

    m_irq = new CSemaphore(0, 1000, INT_IRQ);
    m_nmi = new CSemaphore(0, 1000, INT_NMI);
    m_reset = new CSemaphore(0, 1000, INT_RST);
    CreateMRUMenu();
}
