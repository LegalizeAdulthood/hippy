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
#ifndef _DIALOGS_H___
#define _DIALOGS_H___

#include "hippy.h"
#include "resource.h"
#include <afxwin.h>

class CInputBox : public CDialog
{
private:
    CString strEntry;
    CString strTitle;
    CString strPrompt;

public:
    int ShowModal(CString *pstrPrompt = NULL, CString *pstrTitle = NULL);

    void AsString(CString &str)
    {
        str = strEntry;
    }
    Word AsWord();
    void DoDataExchange(CDataExchange *pDX);
};

class CSFileDialog : public CDialog
{
private:
    CString strBegin;
    CString strFile;
    CString strEnd;

protected:
    enum MYIDD
    {
        IDD = IDD_SRECORD
    };

public:
    int  ShowModal();
    void GetValues(Word &wBegin, Word &wEnd, CString &str);
    void DoDataExchange(CDataExchange *pDX);
};
#endif