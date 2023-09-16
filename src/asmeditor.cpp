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

// clang-format off
BEGIN_MESSAGE_MAP(CAsmEditorWnd, CMDIChildWnd)
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_MESSAGE(WM_JUMPTOLINE, OnJumpToLine)
    ON_MESSAGE(WM_HIDEBUILDWND, OnHideBuildWnd)
    //  ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CAsmEdit, CRichEditCtrl)
    ON_WM_CHAR()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CBuildEdit, CEdit)
    ON_WM_CHAR()
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()
// clang-format on

IMPLEMENT_DYNAMIC(CAsmEditorWnd, CMDIChildWnd)

void CBuildEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    switch (nChar)
    {
    case VK_ESCAPE:
        GetParent()->SendMessage(WM_HIDEBUILDWND);
        break;
    }
}

void CBuildEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    int  nBegin, nEnd, line, d;
    char buffer[1024];
    // retreive selected chars
    GetSel(nBegin, nEnd);
    // now get selected line
    line = LineFromChar(nBegin);
    // receive the contents of the line, if no problems yet
    if (line >= 0)
    {
        GetLine(line, buffer, sizeof(buffer));
        // printf("Line %d : Error: %s\n", line, msg);
        // strip off the line number
        d = -1;
        sscanf(buffer, "error: line %d: ", &d);
        if (d >= 0)
        {
            // send jump message to parent
            GetParent()->SendMessage(WM_JUMPTOLINE, (WPARAM) d);
        }
        else
            MessageBeep(1);
    }
}

LRESULT CAsmEditorWnd::OnHideBuildWnd(WPARAM wParam, LPARAM lParam)
{
    buildWnd.ShowWindow(SW_HIDE);
    CRect rc;
    GetClientRect(&rc);
    OnSize(0, rc.right, rc.bottom);
    return TRUE;
}

LRESULT CAsmEditorWnd::OnJumpToLine(WPARAM wParam, LPARAM lParam)
{
    int nBegin, nEnd;

    if ((nBegin = Editor.LineIndex(wParam - 1)) != -1)
    {
        char buffer[1024];
        Editor.GetLine(wParam - 1, buffer, 1024);
        nEnd = strlen(buffer) + nBegin;
        Editor.SetSel(nBegin, nEnd);
        Editor.SetFocus();
        Editor.LineScroll((wParam - 1) - Editor.GetFirstVisibleLine());
    }
    else
        MessageBeep(1);

    return TRUE;
}

bool CAsmEditorWnd::IsNewFile()
{
    return newFile;
}

void CAsmEditorWnd::GetFileName(CString &str)
{
    str = szFileName;
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
        ReplaceSel("\t\t", false);
    }
    break;
    case ':':
    {
        long start, end, curs, cure;

        /*	char buffer[1024];
                GetLine(-1, buffer, sizeof(buffer));
                CString str(buffer);
                str.TrimLeft("\t ");
                str = str + "\t\t";
                SetSel(LineIndex(), LineIndex()+ LineLength(LineIndex()));
                ReplaceSel(str.GetBuffer(1), false);
                */
        {
            GetSel(curs, cure);
            start = LineIndex();
            end = start + 2;
            SetSel(start, end);
            CString str = GetSelText();
            if (str[0] != ' ' && str[0] != '\t')
            {
                SetSel(curs, cure);
                break;
            }

            ReplaceSel("", false);
            curs -= 2;
            cure -= 2;
            SetSel(cure, cure);
            ReplaceSel("\t\t", false);
            SetSel(curs + 1, cure + 1);
        }
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
    if (!IsWindow(Editor.m_hWnd))
        return;
    // if build output window is visible then it needs care
    if (buildWnd.IsWindowVisible())
    {
        WINDOWPLACEMENT wp;
        buildWnd.GetWindowPlacement(&wp);
        buildWnd.SetWindowPos(NULL, 5, cy - 5 - buildWndHeight, cx - 10, buildWndHeight, 0);
        Editor.SetWindowPos(NULL, 5, 5, cx - 10, cy - buildWndHeight - 15, 0);
    }
    else
    {
        Editor.SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, 0);
    }
}

// saves current file with the name szFileName, assumes that
// it is a valid filename
int CAsmEditorWnd::SaveFile()
{
    CStdioFile     file;
    CFileException fe;
    char           ln[3] = {0x0d, 0x0a, 0};
    char           buffer[2048];
    if (!file.Open(szFileName, CFile::modeWrite | CFile::typeText | CFile::modeCreate, &fe))
    {
        buffer[1000];
        fe.GetErrorMessage(buffer, sizeof(buffer));
        MessageBox(buffer, "File open error");
        return 0;
    }

    file.SetLength(0);

    int c = Editor.GetLineCount();
    int size;
    for (int i = 0; i < c; i++)
    {
        size = Editor.LineLength(Editor.LineIndex(i));
        Editor.GetLine(i, buffer, size);
        buffer[size] = '\n';
        buffer[size + 1] = 0;
        file.WriteString(buffer);
    }
    file.Close();
    Editor.SetModify(false);
    SetWindowText(szFileName);
    newFile = false;
    return 1;
}

// saves the files returns 0 on success, if file is a new file
// returns -1, then we have to call Save As
int CAsmEditorWnd::Save()
{
    if (szFileName == "")
        return false;
    else
        SaveFile();
    return true;
}

int CAsmEditorWnd::SaveAs(CString szFileName)
{
    this->szFileName = szFileName;
    return SaveFile();
}

void CAsmEditorWnd::OnClose()
{
    if (Editor.GetModify())
        MessageBeep(1);
    else
        DestroyWindow();
}

// tries to open the file specified by lpFileName returns true on success
bool CAsmEditorWnd::OpenFile()
{
    CStdioFile file;
    CString    str;
    CString    szBuffer;

    if (file.Open(szFileName.GetBuffer(1), CFile::modeRead | CFile::typeText))
    {
        while (file.ReadString(str))
        {
            szBuffer += str + '\13';
        }
        Editor.SendMessage(WM_SETTEXT, (WPARAM) 0, (LPARAM) szBuffer.GetBuffer(1));
        file.Close();
        return true;
    }
    else
    {
        LPVOID lpMsgBuf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                      GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                      (LPTSTR) &lpMsgBuf, 0, NULL);
        MessageBox((LPCSTR) lpMsgBuf);
        LocalFree(lpMsgBuf);
        return false;
    }
}

CString CAsmEditorWnd::GetHexFileName()
{
    char buffer[1024];
    strcpy(buffer, szFileName.GetBuffer(1));

    char *p = strrchr(buffer, '.');
    if (p)
    {
        p[1] = 'h';
        p[2] = 'e';
        p[3] = 'x';
        p[4] = 0;
    }
    else
    {
        strcat(buffer, ".hex");
    }
    return CString(buffer);
}

// assemble the source file, generate source.HEX file within the same
// directory.
int CAsmEditorWnd::CompileCode()
{
    char buffer[0x50000];

    STARTUPINFO         si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES sa;

    sa.nLength = sizeof(sa);
    sa.bInheritHandle = true;
    sa.lpSecurityDescriptor = NULL;
    HANDLE rd, wr;
    DWORD  bavail = 0, bread = 0;

    if (!CreatePipe(&rd, &wr, &sa, 0))
    {
        TRACE0(_T("Stdout pipe creation failed\n"));
        return 0;
    }
    else
    {
        // code below prepares the startup structure and forks a process for assembler
        GetStartupInfo(&si);
        si.hStdOutput = wr;
        // si.hStdInput = st_out;
        si.hStdError = wr;
        si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        si.hStdInput = NULL;
        si.cb = sizeof(si);
        sprintf(buffer, "%s %s", "assembler.exe", szFileName.GetBuffer(1));
        CreateProcess(NULL, buffer, NULL, NULL, true, CREATE_NEW_CONSOLE /*creation flags*/, NULL /*envirn*/,
                      NULL /*cur dir*/, &si, &pi);

        if (pi.hProcess)
        {
            WaitForSingleObject(pi.hProcess, INFINITE);
            while (!bread)
                PeekNamedPipe(rd, buffer, 0x50000, &bread, &bavail, NULL);
            ReadFile(rd, buffer, 0x50000, &bread, NULL);
            buffer[bread] = 0;
            buildWnd.SetWindowText(buffer);
            // show the window if hidden
            if (!buildWnd.IsWindowVisible())
            {
                buildWnd.ShowWindow(SW_SHOW);
                // then the window size needs to be calculated
                CRect rc;
                GetClientRect(&rc);
                OnSize(0, rc.right, rc.bottom);
            }
        }
        else
        {
            TRACE(_T("CREATING NEW PROCESS FAILED : GetLatError -->  0x%8.8X\n"), ::GetLastError());

            LPVOID lpMsgBuf;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                          (LPTSTR) &lpMsgBuf, 0, NULL);
            TRACE((char *) lpMsgBuf);
            // Free the buffer.
            LocalFree(lpMsgBuf);
        }
    }

    return 1;
}

// constructor & destructor

// pParent     : Parent MDIFrame window (the main application window
// lpcFileName : the filename of the file to be edited in this editor
// bNewFile    : if true then the editor will not open the file (its a new file)
//				 otherwise it will attempt to open the file.
CAsmEditorWnd::CAsmEditorWnd(CMDIFrameWnd *pParent, LPCSTR lpcFileName)
{
    Create(NULL, "Editor", WS_VISIBLE | WS_CHILD | WS_OVERLAPPEDWINDOW, rectDefault, pParent);
    Editor.Create(WS_CHILD | ES_MULTILINE | WS_VSCROLL, rectDefault, this, 101);
    buildWnd.Create(WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_TABSTOP | WS_BORDER, rectDefault, this, 102);
    buildWndHeight = 100;
    buildWnd.SetReadOnly();
    szFileName = lpcFileName;
    newFile = (szFileName == "");
    if (newFile)
    {
        char buffer[100];
        filenumber++;
        sprintf(buffer, "%03d", filenumber);
        szFileName = CString("NewFile") + CString(buffer) + CString(".asm");
    }

    CRect rc;
    Font.CreateFont(12,                       // nHeight
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
                    _T("FixedSys"));          // lpszFacename
    Editor.SetFont(&Font, false);

    GetClientRect(&rc);
    OnSize(0, rc.right, rc.bottom);
    this->SetWindowText(szFileName);
    if (!newFile)
        OpenFile();
    Editor.LimitText(100 * 1024);
    Editor.ShowWindow(SW_SHOW);
    Editor.SetFocus();
    Editor.SetModify(false);
}

CAsmEditorWnd::~CAsmEditorWnd()
{
    Editor.DestroyWindow();
    buildWnd.DestroyWindow();
}