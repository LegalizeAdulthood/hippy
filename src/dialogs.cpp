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

#include "dialogs.h"

#include <cctype>

Word StrToWord(const wxString &str)
{
    if (str[0] == '$')
    {
        Word w = 0;
        BYTE b;
        int  size = str.Length();
        for (int i = 1; i < size; i++)
        {
            const char ch = str[i];
            if (ch >= '0' && ch <= '9')
            {
                b = ch - '0';
            }
            else if (std::toupper(ch) >= 'A' && std::toupper(ch) <= 'F')
            {
                b = std::toupper(ch) - 'A' + 10;
            }
            else
            {
                b = 0;
            }
            w = b + w * 16;
        }
        return w;
    }
    return 0;
}

/////////////////////////////////////
//	INPUT BOX
/////////////////////////////////////
void CInputBox::DoDataExchange(CDataExchange *pDX)
{
    SendMessage(WM_SETTEXT, 0, (LPARAM) LPCTSTR(m_title));
    CString entry = LPCTSTR(m_entry);
    DDX_Text(pDX, IDC_EDIT, entry);
    m_entry = LPCTSTR(entry);
    CString prompt = LPCTSTR(m_prompt);
    DDX_Text(pDX, IDC_PROMPT, prompt);
    m_prompt = LPCTSTR(prompt);
}

int CInputBox::ShowModal(const wxString *prompt, const wxString *title)
{
    m_lpszTemplateName = MAKEINTRESOURCE(IDD_INPUTBOX);
    m_title = title ? *title : _T("User input");
    m_prompt = prompt ? *prompt : _T("Please enter a value");
    return DoModal();
}

Word CInputBox::AsWord()
{
    return StrToWord(m_entry);
}

/////////////////////////////////////
// S-RECORD DUMP DIALOG
/////////////////////////////////////

int CSFileDialog::ShowModal()
{
    m_lpszTemplateName = MAKEINTRESOURCE(CSFileDialog::IDD);

    int ret = DoModal();
    if (ret == IDOK)
    {
        CWinApp *app = AfxGetApp();
        app->WriteProfileString(_T("SRec"), _T("SRecBeging"), m_begin);
        app->WriteProfileString(_T("SRec"), _T("SRecEnd"), m_end);
        app->WriteProfileString(_T("SRec"), _T("SRecFile"), m_file);
    }

    return ret;
}

void CSFileDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);

    { // read latest write locations from registry
        CWinApp *app = AfxGetApp();
        m_begin = wxString(LPCTSTR(app->GetProfileString(_T("SRec"), _T("SRecBeging"), _T("$0000"))));
        m_end = wxString(LPCTSTR(app->GetProfileString(_T("SRec"), _T("SRecEnd"), _T("$FFFF"))));
        m_file = wxString(LPCTSTR(app->GetProfileString(_T("SRec"), _T("SRecFile"), _T("Output.Hex"))));
    }
    CString begin = LPCTSTR(m_begin);
    DDX_Text(pDX, IDC_EDITBADDR, begin);
    m_begin = LPCTSTR(begin);
    CString end = LPCTSTR(m_end);
    DDX_Text(pDX, IDC_EDITEADDR, end);
    m_end = LPCTSTR(end);
    CString file = LPCTSTR(m_file);
    DDX_Text(pDX, IDC_EDITOUTPUT, file);
    m_file = LPCTSTR(file);
}

void CSFileDialog::GetValues(Word &begin, Word &end, wxString &values)
{
    begin = StrToWord(m_begin);
    end = StrToWord(m_end);
    values = LPCTSTR(m_file);
}
