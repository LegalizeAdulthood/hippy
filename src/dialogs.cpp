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

#include <wx/wx.h>

Word StrToWord(CString &str)
{
    if (str[0] == '$')
    {
        Word w = 0;
        BYTE b;
        char ch;
        str.MakeUpper();
        int size = str.GetLength();
        for (int i = 1; i < size; i++)
        {
            ch = str[i];
            if (ch >= '0' && ch <= '9')
            {
                b = ch - '0';
            }
            else if (ch >= 'A' && ch <= 'F')
            {
                b = ch - 'A' + 10;
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
    SendMessage(WM_SETTEXT, 0, (LPARAM) m_title.GetBuffer(1));
    DDX_Text(pDX, IDC_EDIT, m_entry);
    DDX_Text(pDX, IDC_PROMPT, m_prompt);
}

int CInputBox::ShowModal(CString *pstrPrompt, CString *pstrTitle)
{
    m_lpszTemplateName = MAKEINTRESOURCE(IDD_INPUTBOX);
    m_title = pstrTitle ? *pstrTitle : _T("User input");
    m_prompt = pstrPrompt ? *pstrPrompt : _T("Please enter a value");
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
        m_begin = app->GetProfileString(_T("SRec"), _T("SRecBeging"), _T("$0000"));
        m_end = app->GetProfileString(_T("SRec"), _T("SRecEnd"), _T("$FFFF"));
        m_file = app->GetProfileString(_T("SRec"), _T("SRecFile"), _T("Output.Hex"));
    }
    DDX_Text(pDX, IDC_EDITBADDR, m_begin);
    DDX_Text(pDX, IDC_EDITEADDR, m_end);
    DDX_Text(pDX, IDC_EDITOUTPUT, m_file);
}

void CSFileDialog::GetValues(Word &begin, Word &end, CString &str)
{
    begin = StrToWord(m_begin);
    end = StrToWord(m_end);
    str = m_file;
}
