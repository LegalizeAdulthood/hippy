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
#ifndef HIPPY_ASMEDITOR_H
#define HIPPY_ASMEDITOR_H

#include <afx.h>
#include <afxcmn.h>
#include <afxwin.h>

#include <wx/string.h>
#include <wx/textctrl.h>

class AsmEditorWindow
{
public:
    virtual ~AsmEditorWindow() = default;

    virtual void ShowBuildWindow() = 0;
    virtual void HideBuildWindow() = 0;
    virtual void JumpToLine(int line) = 0;
    virtual void ErrorBeep() = 0;
};

class CAsmEdit : public CRichEditCtrl
{
public:
    ~CAsmEdit() override = default;

    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    DECLARE_MESSAGE_MAP()
};

class CBuildEdit : public CEdit
{
public:
    CBuildEdit(AsmEditorWindow *editorWindow) :
        m_editorWindow(editorWindow)
    {
    }
    ~CBuildEdit() override = default;

    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

    DECLARE_MESSAGE_MAP()

private:
    AsmEditorWindow *m_editorWindow;
};

class wxBuildEdit : public wxTextCtrl
{
public:
    wxBuildEdit(AsmEditorWindow *editorWindow) :
        m_editorWindow(editorWindow)
    {
    }
    ~wxBuildEdit() override = default;

private:
    AsmEditorWindow *m_editorWindow;

    void OnChar(wxKeyEvent &event);
    void OnLButtonDblClk(wxMouseEvent &event);

    wxDECLARE_EVENT_TABLE();
};

class CAsmEditorWnd : public CMDIChildWnd, public AsmEditorWindow
{
public:
    CAsmEditorWnd(CMDIFrameWnd *pParent, LPCTSTR lpcFileName);
    ~CAsmEditorWnd() override;

    DECLARE_DYNAMIC(CAsmEditorWnd)

    bool            IsNewFile() const;
    void            GetFileName(wxString &fileName);
    int             CompileCode();
    int             Save();
    int             SaveAs(const wxString &fileName);
    wxString        GetHexFileName() const;
    afx_msg void    OnSize(UINT nType, int cx, int cy);
    afx_msg bool    OnEraseBkgnd(CDC *pDC);
    afx_msg void    OnClose();
    afx_msg LRESULT OnJumpToLine(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnHideBuildWnd(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

    void ShowBuildWindow() override;
    void HideBuildWindow() override;
    void JumpToLine(int line) override;
    void ErrorBeep() override;

private:
    bool OpenFile();
    int  SaveFile();

    CAsmEdit    m_editor;
    wxString    m_fileName;
    CFont       m_font;
    bool        m_newFile{};
    CBuildEdit  m_buildWnd;
    wxBuildEdit m_buildWndWx;
    int         m_buildWndHeight{};
};

#endif
