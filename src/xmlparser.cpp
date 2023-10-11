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

#include "xmlparser.h"

#include "device.h"

#include <wx/filename.h>

#include <vector>

inline char stack_pop(std::vector<char> &stack)
{
    const char result = stack.back();
    stack.pop_back();
    return result;
}

enum XmlTagType
{
    ttHdf,
    ttDevice,
    ttWordSelect,
    ttChipSelect,
    ttWordSelectCld,
    ttChipSelectCld,
    ttAI,
    ttCS
};

struct XmlTag
{
    XmlTagType type;
    double     version;
    int        id;
    wxString   data;
    wxString   name;
    wxString   chip;
    bool       close;
};

class CDeviceFile
{
public:
    int ParseFile(CWnd *parent, const wxString &fileName, CDeviceArray &devArr, AddrResEntry *AddrResTbl);

private:
    FILE  *m_file{};
    bool   m_opened{};
    XmlTag m_xmlTag{};

    void PutCharBack();
    bool FindChar(char c);
    bool FindPattern(LPSTR lpStr);
    char GetString(wxString &value);
    bool GetField(LPCSTR lpcField, wxString &value);
    bool GetUntilChar(char chr, wxString &value);
    bool FindNextTag();
    bool CompileData();
    char GetNextToken();
    bool EvalEqn(Word addr, const wxString &eqn);
};

char CDeviceFile::GetNextToken()
{
    int      state{};
    char     c[3]{};

    while (m_xmlTag.data.Length())
    {
        switch (state)
        {
        case 0:
        {
            if (m_xmlTag.data[0] == wxT('P'))
            {
                state = 1;
            }
            else if (m_xmlTag.data[0] != wxT(' '))
            {
                char ret = static_cast<char>(m_xmlTag.data[0]);
                switch (ret)
                {
                case '~':
                    ret = (char) bitNOT;
                    break;
                case '(':
                    ret = (char) leftPrnt;
                    break;
                case '.':
                    ret = (char) bitAND;
                    break;
                case '^':
                    ret = (char) bitXOR;
                    break;
                case '|':
                    ret = (char) bitOR;
                    break;
                }
                m_xmlTag.data.erase(0, 1);
                return ret;
            }
            m_xmlTag.data.erase(0, 1);
        }
        break;

        case 1:
            if (m_xmlTag.data[0] != wxT('A'))
            {
                return 0;
            }
            state = 2;
            m_xmlTag.data.erase(0, 1);
            break;
        case 2:
            if (m_xmlTag.data[0] > wxT('9') || m_xmlTag.data[0] < wxT('0'))
                return 0;
            c[0] = static_cast<char>(m_xmlTag.data[0]);
            m_xmlTag.data.erase(0, 1);
            state = 3;
            break;
        case 3:
            if (m_xmlTag.data[0] <= wxT('9') && m_xmlTag.data[0] >= wxT('0'))
            {
                c[1] = static_cast<char>(m_xmlTag.data[0]);
                m_xmlTag.data.erase(0, 1);
            }
            else
            {
                c[1] = c[0];
                c[0] = '0';
            }
            {
                const int k = atoi(c);
                return (0xF0 + (char) k);
            }
        }
    }

    return 0;
}

// modifies the pin equations for easy handling :
//
bool CDeviceFile::CompileData()
{
    // assume : m_xmlTag.data is valid
    wxString          buffer;
    unsigned char     c;
    std::vector<char> stack;

    while (c = GetNextToken())
    {
        if (c >= 0xf0)
        {
            // operand
            buffer += c;
            if (!stack.empty() && stack.back() == bitNOT)
            {
                buffer += stack_pop(stack);
            }
        }
        else // operator
            if (c == ')')
            {
                while (!stack.empty() && stack.back() != leftPrnt)
                {
                    buffer += stack_pop(stack);
                }
                stack_pop(stack); // pop (
                if (!stack.empty() && stack.back() == bitNOT)
                {
                    buffer += stack_pop(stack);
                }
            }
            else if (c < 6)
            {
                while (!stack.empty() && stack.back() >= c && stack.back() != leftPrnt)
                {
                    buffer += stack_pop(stack);
                }
                stack.push_back(static_cast<char>(c));
            }
    }

    while (!stack.empty())
    {
        buffer += stack_pop(stack);
    }
    m_xmlTag.data = buffer;
    return true;
}

// decrements file pointer by one
void CDeviceFile::PutCharBack()
{
    fseek(m_file, ftell(m_file) - 1, SEEK_SET);
}

// find the first occurrence of a character in the file
// file pointer positions right after the char.
bool CDeviceFile::FindChar(char c)
{
    while (fgetc(m_file) != c && !feof(m_file))
    {
    }
    return !feof(m_file);
}

// find the first occurence of the pattern in the file
// file pointer positions right after the pattern
bool CDeviceFile::FindPattern(LPSTR lpStr)
{
    char *ptr = lpStr;
    int   i;
    bool  found = true;

    do
    {
        // find the first char
        if (!FindChar(lpStr[0]))
        {
            return false;
        }
        for (i = 1; (unsigned) i < strlen(lpStr); i++)
        {
            if (fgetc(m_file) != lpStr[i])
            {
                break;
                found = false;
            }
        }
    } while (!found);
    return true;
}

// returns the first string encountered. looks for spaces and tabs
// check the eof string. return the character causing return.
char CDeviceFile::GetString(wxString &value)
{
    char c = ' ';

    value.Clear();

    while (c == ' ' || c == '\t')
    {
        c = fgetc(m_file);
        if (feof(m_file))
        {
            return 0;
        }
    }
    while (c != '\n' && c != ' ' && c != '\t' && c != '>' && c != '=')
    {
        value += c;
        c = fgetc(m_file);
    }
    return c;
}

// assuming that we are in a tag, find the value of the given field.
// if > is found before the field then return false
bool CDeviceFile::GetField(LPCSTR lpcField, wxString &value)
{
    wxString szField(wxT("11"));
    char    c;
    while (szField != lpcField)
    {
        c = GetString(szField);
        if (feof(m_file))
        {
            return false;
        }
    }
    if (c != '=')
    {
        if (!FindChar('='))
        {
            return false;
        }
    }
    GetString(value);
    const size_t begin = value.find_first_not_of(wxT('"'));
    const size_t end = value.find_last_not_of(wxT('"'));
    value = value.SubString(begin, end);
    return true;
}

// returns all of the characters ntil the first appereance of the char.
bool CDeviceFile::GetUntilChar(const char chr, wxString &value)
{
    value.Clear();
    char c;
    while (!feof(m_file) && (c = fgetc(m_file)) != chr)
    {
        if (c != ' ' && c != '\t')
        {
            value += c;
        }
    }
    return feof(m_file) == 0 ? true : false;
}

// skip characters, blanklines and find a meaningfull tag,
// also skip comment.
bool CDeviceFile::FindNextTag()
{
    wxString str;
    // look for "<" sign & check if it belongs to a comment or not
label1:
    if (!FindChar('<'))
    {
        return false;
    }
    GetString(str);
    if (str == wxT("!--"))
    {
        if (!FindPattern("-->"))
            return false;
        goto label1;
    }

    m_xmlTag.close = false;

    if (str == wxT("HDF"))
    {
        m_xmlTag.type = ttHdf;
        // look for version info
        m_xmlTag.version = !GetField("version", str) ? 0 : atof(CT2A(str));
    }
    else if (str == wxT("DEVICE"))
    {
        m_xmlTag.type = ttDevice;
        if (!GetField("name", m_xmlTag.name))
        {
            return false;
        }
        if (!GetField("chip", m_xmlTag.chip))
        {
            return false;
        }
    }
    else if (str == wxT("WORD_SELECT"))
    {
        m_xmlTag.type = ttWordSelect;
    }
    else if (str == wxT("/WORD_SELECT"))
    {
        m_xmlTag.type = ttWordSelect;
        m_xmlTag.close = true;
    }
    else if (str == wxT("AI"))
    {
        m_xmlTag.type = ttAI;
        GetField("id", str);
        m_xmlTag.id = atoi(CT2A(str));
        if (!GetUntilChar('<', m_xmlTag.data))
        {
            return false;
        }
        GetUntilChar('>', str);
        if (str != wxT("/AI"))
        {
            return false;
        }
        CompileData();
    }
    else if (str == wxT("CHIP_SELECT"))
    {
        m_xmlTag.type = ttChipSelect;
        if (!GetUntilChar('<', m_xmlTag.data))
        {
            return false;
        }
        GetUntilChar('>', str);
        if (str != wxT("/CHIP_SELECT"))
        {
            return false;
        }
        CompileData();
    }
    else if (str == wxT("/DEVICE"))
    {
        m_xmlTag.type = ttDevice;
        m_xmlTag.close = true;
    }
    else if (str == wxT("/HDF"))
    {
        m_xmlTag.type = ttHdf;
        m_xmlTag.close = true;
    }

    return true;
}

// evaluate the equation using the values in addr, return true or false
bool CDeviceFile::EvalEqn(Word addr, const wxString &eqn)
{
    std::vector<char> stack;
    int const         size = eqn.Length();
    bool              b1;
    bool              b2;

    for (int i = 0; i < size; i++)
    {
        const auto c = static_cast<unsigned char>(eqn[i]);
        if ((c & 0xf0) == 0xf0) // operand
        {
            stack.push_back((addr & 1 << (c & 0x0f)) != 0);
        }
        else
        {
            switch (c)
            {
            case bitNOT:
                stack.push_back(!stack_pop(stack));
                break;
            case bitAND:
                b1 = stack_pop(stack) != 0;
                b2 = stack_pop(stack) != 0;
                stack.push_back(b1 && b2);
                break;
            case bitOR:
                b1 = stack_pop(stack) != 0;
                b2 = stack_pop(stack) != 0;
                stack.push_back(b1 || b2);
                break;
            case bitXOR:
                b1 = stack_pop(stack) != 0;
                b2 = stack_pop(stack) != 0;
                stack.push_back(b1 ^ b2);
                break;
            case 'L':
                stack.push_back(0);
                break;
            case 'H':
                stack.push_back(1);
                break;
            default:
                break;
            }
        }
    }

    return stack_pop(stack) != 0;
}

int CDeviceFile::ParseFile(CWnd *parent, const wxString &fileName, CDeviceArray &devArr, AddrResEntry *AddrResTbl)
{
    m_file = fopen(CT2A(fileName), "r");
    if (!m_file)
    {
        return 0;
    }

    const wxString deviceDir = wxFileName(fileName).GetPath();
    std::vector<wxString> csEqn;
    std::vector<wxString> addrEqns[255];
    {
        int      num = 0;
        wxString name;
        wxString libName;
        while (FindNextTag())
        {
            switch (m_xmlTag.type)
            {
            case ttDevice:
                if (m_xmlTag.close)
                { // </DEVICE>
                    wxString libPath(wxFileName(deviceDir, libName).GetAbsolutePath());
                    HMODULE  hmod = LoadLibrary(LPCTSTR(libPath));
                    if(!hmod)
                    {
                        wxMessageBox(wxString::Format(wxT("Couldn't load device %s (%s)"), name, libPath));
                        break;
                    }
                    pdevFunctv func = pdevFunctv(GetProcAddress(hmod, "GetNewDevice"));
                    if(!func)
                    {
                        wxMessageBox(wxString::Format(wxT("Couldn't load device factory function for device %s (%s)"),
                                                      name, libPath));
                        break;
                    }
                    CDevice   *pDev = func();
                    pDev->Create(parent, name, libName);
                    devArr.push_back(pDev);
                    num++;
                }
                else
                { // <DEVICE name="xxx" chip="###">
                    libName = m_xmlTag.chip + wxT(".dll");
                    name = m_xmlTag.name;
                }
                break;
            case ttChipSelect:
                csEqn.push_back(m_xmlTag.data);
                break;
            case ttAI:
                addrEqns[num].push_back(m_xmlTag.data);
                break;
            }
        }
    }

    // build up the address resolution table
    {
        const int size = static_cast<int>(devArr.size());
        for (int w = 0x0000; w < 0x10000; w++)
        {
            // for each location look for selected chip
            bool found = false;
            int  i;
            for (i = 0; i < size; i++)
            {
                if (EvalEqn(w, csEqn[i]))
                {
                    found = true;
                    break;
                }
            }

            // for each selected chip decode this address
            // according to its address equations
            if (found)
            {
                Word dec = 0;
                AddrResTbl[w].devIndex = i;
                int k = 0;
                for (const wxString &eqn : addrEqns[i])
                {
                    dec |= EvalEqn(w, eqn) << k;
                }
                AddrResTbl[w].decodedAddr = dec;
            }
            else
            {
                AddrResTbl[w].devIndex = 255; // 255 means no device
            }
        }
    }

#if 0
    { // DEBUGGING CODE
        BYTE     last = 255;
        BYTE     cur;
        wxString str;

        FILE *f = fopen("dbg.txt", "w");
        for (int w = 0; w < 0x10000; w++)
        {
            cur = AddrResTbl[w].devIndex;
            if (cur == 255)
            {
                str = wxT("Memory");
            }
            else
            {
                devArr[cur]->GetDeviceName(str);
            }
            if (cur != last)
            {
                _ftprintf(f, wxT("[%04x] %s\n"), w, LPCTSTR(str));
                last = cur;
            }
        }
        fclose(f);
    }
#endif

    fclose(m_file);
    return 1;
}

int ParseFile(CWnd *parent, const wxString &fileName, CDeviceArray &devArr, AddrResEntry *AddrResTbl)
{
    return CDeviceFile().ParseFile(parent, fileName, devArr, AddrResTbl);
}
