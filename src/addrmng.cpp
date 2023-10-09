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

#include "addrmng.h"

#include "device.h"
#include "disassembler.h"
#include "environment.h"

///////////////////////////////////////

int HexToByte(char c)
{
    c = toupper(c);
    return c >= 'A' && c <= 'F' ? c - 'A' + 10 : c - '0';
}

BYTE GetNextByte(FILE *f)
{
    int hi = fgetc(f);
    int lo = fgetc(f);
    return HexToByte(hi) * 0x10 + HexToByte(lo);
}

///////////////////////////////////////////////

CAddressManager::~CAddressManager()
{
    for (CDevice *device : m_devices)
    {
        delete device;
    }
    m_devices.clear();
}

bool CAddressManager::LoadFile(const wxString &fname, std::vector<Word> &adr_arr)
{
    int   hi{};
    Word  last_addr = 0xffff;
    int   dbg = 0;
    FILE *f = fopen(CT2A(fname), "r");
    if (!f)
    {
        return false;
    }
    while (!feof(f))
    {
        do
        {
            if (feof(f))
            {
                break;
            }
            hi = fgetc(f);
        } while (hi != 'S');
        int lo = fgetc(f);
        dbg++;
        if (hi == 'S')
        {
            if (lo == '0' || lo == '9')
            {
                while (fgetc(f) != '\n' && !feof(f))
                {
                }
            }
            else
            {
                int  lg = GetNextByte(f);
                Word addr = GetNextByte(f);
                BYTE checksum = addr + lg;
                BYTE val = GetNextByte(f);
                checksum += val;
                addr = addr * 0x100 + val;
                lg -= 3;
                if (last_addr != addr)
                {
                    adr_arr.push_back(addr);
                }
                for (; lg > 0; lg--)
                {
                    val = GetNextByte(f);
                    checksum += val;
                    Write(addr, val); // memory[addr] = val;
                    addr++;
                }
                last_addr = addr;

                BYTE ck = GetNextByte(f);
                if ((BYTE) ~checksum != ck)
                {
                    MessageBox(nullptr, wxT("CHECK SUM ERROR, S file seems to be badly formatted."), wxT("ERROR"),
                               MB_OK | MB_ICONWARNING);
                    return false;
                }
                fgetc(f); // newline
            }
        }
    }
    fclose(f);
    return true;
}

/* save the contents of the memory from wBegin to wEnd (inclusive)
 * to file specified (str). Output file is in S-Record File format
 */
int CAddressManager::SaveSFile(const wxString &fileName, Word wBegin, Word wEnd)
{
    HexDumper      hex;
    CStdioFile     file;
    CFileException fe;
    Word           i, num_bytes = wEnd - wBegin; // number of bytes to export
    BYTE           cksum, b;
    int            numWrite;
    BYTE           buffer[0x20];
    char           buf_out[40];
    buf_out[0] = 'S';
    buf_out[1] = '1';
    if (!file.Open(fileName, CFile::modeWrite | CFile::modeCreate, &fe))
    {
        return 0;
    }
    while (num_bytes)
    {
        // one line in a S-Record file contains up to 10 bytes (this may depend on the implementation)
        // of data (plus addr, size, cksum)
        numWrite = (num_bytes >= 16) ? 16 : num_bytes;
        num_bytes -= numWrite;
        cksum = buffer[0] = (numWrite + 3);                   // size of the line
        cksum += buffer[1] = (BYTE) ((wBegin & 0xff00) >> 8); // higher byte of address
        cksum += buffer[2] = (BYTE) (wBegin & 0x00ff);        // lower byte of address
        for (i = 0; i < numWrite; i++)
        {
            b = Read(wBegin, true);
            wBegin++;
            buffer[3 + i] = b;
            cksum += b;
        }
        cksum = ~cksum;
        buffer[3 + i] = cksum;
        hex.ByteArrayToHexArray(buffer, 4 + i, buf_out + 2);
        strcat(buf_out, "\n");
        file.WriteString(CA2T(buf_out));
    }
    file.WriteString(wxT("S9030000FC\n"));
    file.Close();
    return 0;
}

void CAddressManager::Write(Word wIndex, BYTE bVal)
{
    int ind = m_AddrResTbl[wIndex].devIndex;
    if (ind < 255)
    {
        m_devices[ind]->Write(m_AddrResTbl[wIndex].decodedAddr, bVal, false);
    }
    else
    {
        m_memory[wIndex] = bVal;
    }
    m_wLastWrite = wIndex;
}

BYTE CAddressManager::Read(Word wIndex, bool bDbg)
{
    BYTE val;
    int  ind = m_AddrResTbl[wIndex].devIndex;
    if (ind < 255)
    {
        m_devices[ind]->Read(m_AddrResTbl[wIndex].decodedAddr, val, bDbg);
    }
    else
    {
        val = m_memory[wIndex];
    }
    m_wLastRead = wIndex;
    return val;
}

void CAddressManager::Create(CEnvironment *pEnv)
{
    CStringArray sa;
    memset(m_memory, 0, sizeof(m_memory));
    this->m_env = pEnv;
    // read the device file
    ParseFile(pEnv->GetMainWnd(), pEnv->GetDeviceFile(), m_devices, m_AddrResTbl);
}
