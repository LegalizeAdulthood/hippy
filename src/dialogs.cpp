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
                b = ch - '0';
            else if (ch >= 'A' && ch <= 'F')
                b = ch - 'A' + 10;
            else
                b = 0;
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
    SendMessage(WM_SETTEXT, 0, (LPARAM) strTitle.GetBuffer(1));
    DDX_Text(pDX, IDC_EDIT, strEntry);
    DDX_Text(pDX, IDC_PROMPT, strPrompt);
    // DDV_Text(pDX, IDD_INPUTBOX, strTitle);
}

int CInputBox::ShowModal(CString *pstrPrompt, CString *pstrTitle)
{
    m_lpszTemplateName = MAKEINTRESOURCE(IDD_INPUTBOX);
    if (pstrTitle)
        strTitle = *pstrTitle;
    else
        strTitle = "User input";

    if (pstrPrompt)
        strPrompt = *pstrPrompt;
    else
        strPrompt = "Please enter a value";

    return DoModal();
}

Word CInputBox::AsWord()
{
    return StrToWord(strEntry);
}

/////////////////////////////////////
// S-RECORD DUMP DIALOG
/////////////////////////////////////

int CSFileDialog::ShowModal()
{
    int ret;
    m_lpszTemplateName = MAKEINTRESOURCE(CSFileDialog::IDD);

    ret = DoModal();
    if (ret == IDOK)
    {
        CWinApp *app = AfxGetApp();
        app->WriteProfileString("SRec", "SRecBeging", strBegin);
        app->WriteProfileString("SRec", "SRecEnd", strEnd);
        app->WriteProfileString("SRec", "SRecFile", strFile);
    }

    return ret;
}

void CSFileDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);

    { // read latest write locations from registry
        CWinApp *app = AfxGetApp();
        strBegin = app->GetProfileString("SRec", "SRecBeging", "$0000");
        strEnd = app->GetProfileString("SRec", "SRecEnd", "$FFFF");
        strFile = app->GetProfileString("SRec", "SRecFile", "Output.Hex");
    }
    DDX_Text(pDX, IDC_EDITBADDR, strBegin);
    DDX_Text(pDX, IDC_EDITEADDR, strEnd);
    DDX_Text(pDX, IDC_EDITOUTPUT, strFile);
}

void CSFileDialog::GetValues(Word &wBegin, Word &wEnd, CString &str)
{
    wBegin = StrToWord(strBegin);
    wEnd = StrToWord(strEnd);
    str = strFile;
}