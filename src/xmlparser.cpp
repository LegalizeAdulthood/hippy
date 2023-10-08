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

char CDeviceFile::GetNextToken()
{
    int     state = 0, k;
    CString szBuffer;
    char    c[3], ret;

    while (m_xmlTag.szData.GetLength())
    {
        switch (state)
        {
        case 0:
        {
            if (m_xmlTag.szData[0] == 'P')
            {
                state = 1;
            }
            else if (m_xmlTag.szData[0] != ' ')
            {
                ret = m_xmlTag.szData[0];
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
                m_xmlTag.szData.Delete(0);
                return ret;
            }
            m_xmlTag.szData.Delete(0);
        }
        break;
        case 1:
            if (m_xmlTag.szData[0] != 'A')
            {
                return 0;
            }
            state = 2;
            m_xmlTag.szData.Delete(0);
            break;
        case 2:
            if (m_xmlTag.szData[0] > '9' || m_xmlTag.szData[0] < '0')
                return 0;
            c[0] = m_xmlTag.szData[0];
            m_xmlTag.szData.Delete(0);
            state = 3;
            break;
        case 3:
            if (m_xmlTag.szData[0] <= '9' && m_xmlTag.szData[0] >= '0')
            {
                c[1] = m_xmlTag.szData[0];
                m_xmlTag.szData.Delete(0);
            }
            else
            {
                c[1] = c[0];
                c[0] = '0';
            }
            k = atoi(c);
            ret = (0xF0 + (char) k);
            return ret;
        }
    }

    return 0;
}

// modifies the pin equations for easy handling :
//
bool CDeviceFile::CompileData()
{
    // assume : m_xmlTag.szData is valid
    CString       szBuffer = "";
    unsigned char c;
    IMPLEMENT_STACK();

    while (c = GetNextToken())
    {
        if (c >= 0xf0)
        {
            // operand
            szBuffer += c;
            if (indStack && stack[indStack - 1] == bitNOT)
            {
                szBuffer += POP();
            }
        }
        else // operator
            if (c == ')')
            {
                while (indStack && stack[indStack - 1] != leftPrnt)
                {
                    szBuffer += POP();
                }
                POP(); // pop (
                if (indStack && stack[indStack - 1] == bitNOT)
                {
                    szBuffer += POP();
                }
            }
            else if (c < 6)
            {
                while (indStack && stack[indStack - 1] >= c && stack[indStack - 1] != leftPrnt)
                {
                    szBuffer += POP();
                }
                PUSH(c);
            }
    }

    while (indStack)
    {
        szBuffer += POP();
    };
    m_xmlTag.szData = szBuffer;
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
char CDeviceFile::GetString(CString &szString)
{
    char c = ' ';

    szString = "";

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
        szString += c;
        c = fgetc(m_file);
    }
    return c;
}

// assuming that we are in a tag, find the value of the given field.
// if > is found before the field then return false
bool CDeviceFile::GetField(LPCSTR lpcField, CString &szVal)
{
    CString szField("11");
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
    GetString(szVal);
    szVal.TrimLeft("\"");
    szVal.TrimRight("\"");
    return true;
}

// returns all of the characters ntil the first appereance of the char.
bool CDeviceFile::GetUntilChar(const char chr, CString &szStr)
{
    szStr = "";
    char c;
    while (!feof(m_file) && (c = fgetc(m_file)) != chr)
    {
        if (c != ' ' && c != '\t')
        {
            szStr += c;
        }
    }
    if (feof(m_file))
    {
        return false;
    }

    return true;
}

// skip characters, blanklines and find a meaningfull tag,
// also skip comment.
bool CDeviceFile::FindNextTag()
{
    CString str;
    // look for "<" sign & check if it belongs to a comment or not
label1:
    if (!FindChar('<'))
    {
        return false;
    }
    GetString(str);
    if (str == "!--")
    {
        if (!FindPattern("-->"))
            return false;
        goto label1;
    }

    m_xmlTag.close = false;

    if (str == "HDF")
    {
        m_xmlTag.type = ttHdf;
        // look for version info
        m_xmlTag.version = !GetField("version", str) ? 0 : atof(str);
    }
    else if (str == "DEVICE")
    {
        m_xmlTag.type = ttDevice;
        if (!GetField("name", m_xmlTag.szName))
        {
            return false;
        }
        if (!GetField("chip", m_xmlTag.szChip))
        {
            return false;
        }
    }
    else if (str == "WORD_SELECT")
    {
        m_xmlTag.type = ttWordSelect;
    }
    else if (str == "/WORD_SELECT")
    {
        m_xmlTag.type = ttWordSelect;
        m_xmlTag.close = true;
    }
    else if (str == "AI")
    {
        m_xmlTag.type = ttAI;
        GetField("id", str);
        m_xmlTag.id = atoi(str);
        if (!GetUntilChar('<', m_xmlTag.szData))
        {
            return false;
        }
        GetUntilChar('>', str);
        if (str != "/AI")
        {
            return false;
        }
        CompileData();
    }
    else if (str == "CHIP_SELECT")
    {
        m_xmlTag.type = ttChipSelect;
        if (!GetUntilChar('<', m_xmlTag.szData))
        {
            return false;
        }
        GetUntilChar('>', str);
        if (str != "/CHIP_SELECT")
        {
            return false;
        }
        CompileData();
    }
    else if (str == "/DEVICE")
    {
        m_xmlTag.type = ttDevice;
        m_xmlTag.close = true;
    }
    else if (str == "/HDF")
    {
        m_xmlTag.type = ttHdf;
        m_xmlTag.close = true;
    }

    return true;
}

// evaluate the equation using the values in addr, return true or false
bool CDeviceFile::EvalEqn(Word addr, CString &eqn)
{
    IMPLEMENT_STACK();
    int  i, size = eqn.GetLength();
    bool b1, b2;
    char c;

    for (i = 0; i < size; i++)
    {
        c = eqn[i];
        if ((c & 0xf0) == 0xf0) // operant
        {
            PUSH((bool) ((addr & (1 << (c & 0x0f))) != 0));
        }
        else
        {
            switch (c)
            {
            case bitNOT:
                PUSH(!(POP()));
                break;
            case bitAND:
                b1 = POP() != 0;
                b2 = POP() != 0;
                PUSH(b1 && b2);
                break;
            case bitOR:
                b1 = POP() != 0;
                b2 = POP() != 0;
                PUSH(b1 || b2);
                break;
            case bitXOR:
                b1 = POP() != 0;
                b2 = POP() != 0;
                PUSH(b1 ^ b2);
                break;
            case 'L':
                PUSH(0);
                break;
            case 'H':
                PUSH(1);
                break;
            }
        }
    }

    return (POP() != 0);
}

int CDeviceFile::ParseFile(CWnd *parent, CString szFileName, CDeviceArray &devArr, AddrResEntry *AddrResTbl)
{
    CString      szName;
    CStringArray csEqn;
    CStringArray addrEqns[255];
    CString      szLibName;
    int          num = 0;
    typedef CDevice *(*pvFunctv)();
    int i = -1;
    m_file = fopen(szFileName, "r");
    if (m_file)
    {
        while (FindNextTag())
        {
            switch (m_xmlTag.type)
            {
            case ttDevice:

                if (m_xmlTag.close)
                { // </DEVICE>
                    HMODULE  hmod = LoadLibrary(CString("../devices/" + szLibName));
                    pvFunctv func = (pvFunctv) GetProcAddress(hmod, "GetNewDevice");
                    CDevice *pDev;
                    pDev = func();
                    pDev->Create(parent, szName);
                    devArr.Add(pDev);
                    num++;
                }
                else
                { // <DEVICE name="xxx" chip="###">
                    szLibName = m_xmlTag.szChip + ".dll";
                    szName = m_xmlTag.szName;
                }
                break;
            case ttChipSelect:
                csEqn.Add(m_xmlTag.szData);
                break;
            case ttAI:
                addrEqns[num].Add(m_xmlTag.szData);
                break;
            }
        }

        // build up the address resolution table
        {
            int  w;
            int  size = devArr.GetSize();
            bool found;
            int  k;
            Word dec;

            for (w = 0x0000; w < 0x10000; w++)
            {
                // for each location look for selected chip
                found = false;
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
                    dec = 0;
                    AddrResTbl[w].devIndex = i;
                    for (k = 0; k < addrEqns[i].GetSize(); k++)
                    {
                        dec |= EvalEqn(w, addrEqns[i][k]) << k;
                    }
                    AddrResTbl[w].decodedAddr = dec;
                }
                else
                {
                    AddrResTbl[w].devIndex = 255; // 255 means no device
                }
            }

#if 0
            { // DEBUGGING CODE
                BYTE    last = 255;
                BYTE    cur;
                CString str;

                FILE *f = fopen("e:/dbg.txt", "w");
                for (int w = 0; w < 0x10000; w++)
                {
                    if (w == 0xc001)
                    {
                        cur = cur;
                    }
                    cur = AddrResTbl[w].devIndex;
                    if (cur == 255)
                    {
                        str = "MEMORY";
                    }
                    else
                    {
                        devArr[cur]->GetDeviceName(str);
                    }
                    if (cur != last)
                    {
                        fprintf(f, "[%08d] %s\n", w, str.GetBuffer(1));
                        last = cur;
                    }
                }
                fclose(f);
            }
#endif
        }
    }

    return 1;
}
