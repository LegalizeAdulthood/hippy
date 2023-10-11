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

#include "asmeditor.h"

#include "hippy.h"
#include "utils.h"

#include <wx/nativewin.h>

#include <fstream>
#include <string>

static int s_fileNumber = 0;

// clang-format off
BEGIN_MESSAGE_MAP(CAsmEditorWnd, CMDIChildWnd)
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_MESSAGE(WM_JUMPTOLINE, OnJumpToLine)
    ON_MESSAGE(WM_HIDEBUILDWND, OnHideBuildWnd)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CAsmEdit, CRichEditCtrl)
    ON_WM_CHAR()
END_MESSAGE_MAP()

wxBEGIN_EVENT_TABLE(wxBuildEdit, wxTextCtrl)
    EVT_CHAR(OnChar)
    EVT_LEFT_DCLICK(OnLButtonDblClk)
wxEND_EVENT_TABLE()

; // workaround to get clang-format to stop indenting after these macros.
// clang-format on

IMPLEMENT_DYNAMIC(CAsmEditorWnd, CMDIChildWnd)

void wxBuildEdit::OnChar(wxKeyEvent &event)
{
    if (event.GetKeyCode() == WXK_ESCAPE)
    {
        m_editorWindow->HideBuildWindow();
    }
}

void wxBuildEdit::OnLButtonDblClk(wxMouseEvent &event)
{
    long begin;
    long end;
    // retrieve selected chars
    GetSelection(&begin, &end);
    // now get selected line
    long line;
    // receive the contents of the line, if no problems yet
    if (PositionToXY(begin, nullptr, &line))
    {
        wxString text = GetLineText(line);
        // strip off the line number
        const int errorLine = hippy::GetErrorLineNumber(text);
        if (errorLine >= 0)
        {
            m_editorWindow->JumpToLine(errorLine);
        }
        else
        {
            m_editorWindow->ErrorBeep();
        }
    }
}

LRESULT CAsmEditorWnd::OnHideBuildWnd(WPARAM wParam, LPARAM lParam)
{
    HideBuildWindow();
    return TRUE;
}

void CAsmEditorWnd::HideBuildWindow()
{
    if (!m_buildWndWx->IsShown())
    {
        return;
    }

    m_buildWndWx->Show(false);
    CRect rc;
    GetClientRect(&rc);
    OnSize(0, rc.right, rc.bottom);
}

LRESULT CAsmEditorWnd::OnJumpToLine(WPARAM wParam, LPARAM lParam)
{
    int line = wParam - 1;
    JumpToLine(line);
    return TRUE;
}

void CAsmEditorWnd::JumpToLine(int line)
{
    const int nBegin = m_editor.LineIndex(line);
    if (nBegin != -1)
    {
        TCHAR buffer[1024];
        m_editor.GetLine(line, buffer, 1024);
        const int nEnd = _tcslen(buffer) + nBegin;
        m_editor.SetSel(nBegin, nEnd);
        m_editor.SetFocus();
        m_editor.LineScroll(line - m_editor.GetFirstVisibleLine());
    }
    else
    {
        wxBell();
    }
}

void CAsmEditorWnd::ErrorBeep()
{
    wxBell();
}

bool CAsmEditorWnd::IsNewFile() const
{
    return m_newFile;
}

void CAsmEditorWnd::GetFileName(wxString &fileName)
{
    fileName = m_fileName;
}

bool CAsmEditorWnd::OnEraseBkgnd(CDC *pDC)
{
    return true;
}

void CAsmEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);

    SetModify();
    switch (nChar)
    {
    case VK_RETURN:
    {
        ReplaceSel(wxT("\t\t"), false);
    }
    break;
    case ':':
    {
        long curs, cure;
        GetSel(curs, cure);
        long start = LineIndex();
        long end = start + 2;
        SetSel(start, end);
        CString str = GetSelText();
        if (str[0] != wxT(' ') && str[0] != wxT('\t'))
        {
            SetSel(curs, cure);
            break;
        }

        ReplaceSel(wxT(""), false);
        curs -= 2;
        cure -= 2;
        SetSel(cure, cure);
        ReplaceSel(wxT("\t\t"), false);
        SetSel(curs + 1, cure + 1);
    }
    break;
    case VK_ESCAPE:
        GetParent()->SendMessage(WM_HIDEBUILDWND);
        break;
    }
}

void CAsmEditorWnd::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);
    if (!IsWindow(m_editor.m_hWnd))
    {
        return;
    }
    // if build output window is visible then it needs care
    if (m_buildWndWx->IsShown())
    {
        m_buildWndWx->SetPosition(wxPoint(5, cy - 5 - m_buildWndHeight));
        m_buildWndWx->SetSize(cx - 10, m_buildWndHeight);
        m_editor.SetWindowPos(nullptr, 5, 5, cx - 10, cy - m_buildWndHeight - 15, 0);
    }
    else
    {
        m_editor.SetWindowPos(nullptr, 5, 5, cx - 10, cy - 10, 0);
    }
}

// saves current file with the name m_fileName, assumes that
// it is a valid filename
int CAsmEditorWnd::SaveFile()
{
    std::ofstream file(m_fileName.c_str().AsChar(), std::ios_base::out | std::ios_base::trunc);
    if (!file)
    {
        wxMessageBox(wxString::Format(wxT("Error opening %s for writing"), m_fileName.c_str()), wxT("File open error"));
        return 0;
    }

    int   c = m_editor.GetLineCount();
    int   size;
    TCHAR buffer[1000];
    for (int i = 0; i < c; i++)
    {
        size = m_editor.LineLength(m_editor.LineIndex(i));
        if (size > 0)
        {
            m_editor.GetLine(i, buffer, size);
        }
        buffer[size] = wxT('\n');
        buffer[size + 1] = 0;
        file << CT2A(buffer);
    }
    file.close();
    m_editor.SetModify(false);
    SetWindowText(m_fileName);
    m_newFile = false;
    return 1;
}

// saves the files returns 0 on success, if file is a new file
// returns -1, then we have to call Save As
int CAsmEditorWnd::Save()
{
    if (m_fileName.IsEmpty())
    {
        return false;
    }
    SaveFile();
    return true;
}

int CAsmEditorWnd::SaveAs(const wxString &fileName)
{
    m_fileName = fileName;
    return SaveFile();
}

void CAsmEditorWnd::OnClose()
{
    if (m_editor.GetModify())
    {
        wxBell();
    }
    else
    {
        DestroyWindow();
    }
}

// tries to open the file specified by lpFileName returns true on success
bool CAsmEditorWnd::OpenFile()
{
    std::ifstream file(m_fileName.c_str().AsChar(), std::ios_base::in);
    if (!file)
    {
        LPVOID lpMsgBuf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      (LPTSTR) &lpMsgBuf, 0, nullptr);
        wxMessageBox((LPCTSTR) lpMsgBuf, wxT("Error"), wxOK | wxICON_ERROR | wxCENTER);
        LocalFree(lpMsgBuf);
        return false;
    }

    wxString    buffer;
    std::string line;
    while (std::getline(file, line))
    {
        buffer += wxString(line.c_str()) + wxT('\13');
    }
    m_editor.SendMessage(WM_SETTEXT, 0, (LPARAM) buffer.c_str().AsWChar());
    return true;
}

wxString CAsmEditorWnd::GetHexFileName() const
{
    return hippy::GetHexFileName(m_fileName);
}

// assemble the source file, generate source.HEX file within the same
// directory.
int CAsmEditorWnd::CompileCode()
{
    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;

    sa.nLength = sizeof(sa);
    sa.bInheritHandle = true;
    sa.lpSecurityDescriptor = nullptr;
    HANDLE rd, wr;
    DWORD  bavail = 0, bread = 0;

    if (!CreatePipe(&rd, &wr, &sa, 0))
    {
        TRACE0("Stdout pipe creation failed\n");
        return 0;
    }

    // code below prepares the startup structure and forks a process for assembler
    GetStartupInfo(&si);
    si.hStdOutput = wr;
    // si.hStdInput = st_out;
    si.hStdError = wr;
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = nullptr;
    si.cb = sizeof(si);
    wxString cmd = wxString::Format(wxT("assembler %s"), m_fileName.c_str());
    CreateProcess(nullptr, LPTSTR(cmd.wx_str()), nullptr, nullptr, true, CREATE_NEW_CONSOLE /*creation flags*/,
                  nullptr /*envirn*/, nullptr /*cur dir*/, &si, &pi);
    if (pi.hProcess)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        char buffer[0x50000];
        buffer[0] = 0;
        while (!bread)
        {
            PeekNamedPipe(rd, buffer, 0x50000, &bread, &bavail, nullptr);
        }
        ReadFile(rd, buffer, 0x50000, &bread, nullptr);
        buffer[bread] = 0;
        m_buildWndWx->WriteText(wxString(buffer));
        // show the window if hidden
        ShowBuildWindow();
    }
    else
    {
        wxLogDebug(wxT("CREATING NEW PROCESS FAILED : GetLatError -->  0x%8.8X\n"), ::GetLastError());
        void *msgBuffer{};
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      (LPTSTR) &msgBuffer, 0, nullptr);
        wxLogDebug(static_cast<wxChar *>(msgBuffer));
        // Free the buffer.
        LocalFree(msgBuffer);
    }

    return 1;
}

void CAsmEditorWnd::ShowBuildWindow()
{
    if (m_buildWndWx->IsShown())
    {
        return;
    }

    m_buildWndWx->Show(true);
    // then the window size needs to be calculated
    CRect rc;
    GetClientRect(&rc);
    OnSize(0, rc.right, rc.bottom);
}

// constructor & destructor

// pParent     : Parent MDIFrame window (the main application window
// lpcFileName : the filename of the file to be edited in this editor
// bNewFile    : if true then the editor will not open the file (its a new file)
//				 otherwise it will attempt to open the file.
CAsmEditorWnd::CAsmEditorWnd(CMDIFrameWnd *pParent, LPCTSTR lpcFileName) :
    m_buildWndWx(new wxBuildEdit(this))
{
    CMDIChildWnd::Create(nullptr, wxT("Editor"), WS_VISIBLE | WS_CHILD | WS_OVERLAPPEDWINDOW, rectDefault, pParent);
    m_editor.Create(WS_CHILD | ES_MULTILINE | WS_VSCROLL, rectDefault, this, 101);
    m_buildWndHeight = 100;

    m_containerWx = new wxNativeContainerWindow(m_hWnd);
    m_buildWndWx->Create(m_containerWx, 102, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_buildWndWx->Enable(false);

    m_fileName = lpcFileName;
    m_newFile = m_fileName.IsEmpty();
    if (m_newFile)
    {
        TCHAR buffer[100];
        s_fileNumber++;
        _stprintf(buffer, wxT("%03d"), s_fileNumber);
        m_fileName = wxString(wxT("NewFile")) + wxString(buffer) + wxString(wxT(".asm"));
    }

    CRect rc;
#ifdef UNICODE
    m_font.CreateFontW
#else
    m_font.CreateFontA
#endif
        (12,                       // nHeight
         0,                        // nWidth
         0,                        // nEscapement
         0,                        // nOrientation
         FW_NORMAL,                // nWeight
         FALSE,                    // bItalic
         FALSE,                    // bUnderline
         0,                        // cStrikeOut
         ANSI_CHARSET,             // nCharSet
         OUT_DEFAULT_PRECIS,       // nOutPrecision
         CLIP_DEFAULT_PRECIS,      // nClipPrecision
         DEFAULT_QUALITY,          // nQuality
         DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
         wxT("FixedSys"));         // lpszFacename
    m_editor.SetFont(&m_font, false);

    GetClientRect(&rc);
    OnSize(0, rc.right, rc.bottom);
    SetWindowText(m_fileName);
    if (!m_newFile)
    {
        OpenFile();
    }
    m_editor.LimitText(100 * 1024);
    m_editor.ShowWindow(SW_SHOW);
    m_editor.SetFocus();
    m_editor.SetModify(false);
}

CAsmEditorWnd::~CAsmEditorWnd()
{
    m_editor.DestroyWindow();
}
