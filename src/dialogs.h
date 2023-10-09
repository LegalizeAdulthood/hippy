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
#ifndef HIPPY_DIALOGS_H
#define HIPPY_DIALOGS_H

#include "hippy.h"
#include "resource.h"

#include <afxwin.h>

#include <wx/wx.h>

class CInputBox : public CDialog
{
public:
    int ShowModal(CString *pstrPrompt = nullptr, CString *pstrTitle = nullptr);

    void AsString(CString &str)
    {
        str = m_entry;
    }
    Word AsWord();
    void DoDataExchange(CDataExchange *pDX) override;

private:
    CString m_entry;
    CString m_title;
    CString m_prompt;
};

class CSFileDialog : public CDialog
{
public:
    int  ShowModal();
    void GetValues(Word &begin, Word &end, CString &str);
    void DoDataExchange(CDataExchange *pDX) override;

protected:
    enum MYIDD
    {
        IDD = IDD_SRECORD
    };

private:
    CString m_begin;
    CString m_file;
    CString m_end;
};

#endif
