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

///////////////////////////////////////

int HexToByte(char c)
{
    c = toupper(c);
    if (c <= 'F' && c >= 'A')
        return c - 'A' + 10;
    else
        return c - '0';
}

BYTE GetNextByte(FILE *f)
{
    int hi, lo;
    hi = fgetc(f);
    lo = fgetc(f);
    return HexToByte(hi) * 0x10 + HexToByte(lo);
}

///////////////////////////////////////////////

bool CAddressManager::LoadFile(char *fname, CArray<Word, Word &> &adr_arr)
{
    int   hi, lo;
    int   lg;
    Word  addr;
    BYTE  val, ck;
    BYTE  checksum;
    Word  last_addr = 0xffff;
    int   dbg = 0;
    FILE *f = fopen(fname, "r");
    if (!f)
        return false;
    while (!feof(f))
    {
        do
        {
            if (feof(f))
                break;
            hi = fgetc(f);
        } while (hi != 'S');
        lo = fgetc(f);
        dbg++;
        if (hi == 'S')
        {
            if (lo == '0' || lo == '9')
            {
                while (fgetc(f) != '\n' && !feof(f))
                    ;
            }
            else
            {
                lg = GetNextByte(f);
                addr = GetNextByte(f);
                checksum = addr + lg;
                val = GetNextByte(f);
                checksum += val;
                addr = addr * 0x100 + val;
                lg -= 3;
                if (last_addr != addr)
                    adr_arr.Add(addr);
                for (; lg > 0; lg--)
                {
                    val = GetNextByte(f);
                    checksum += val;
                    Write(addr, val); // memory[addr] = val;
                    addr++;
                }
                last_addr = addr;

                ck = GetNextByte(f);
                if ((BYTE) ~checksum != ck)
                {
                    MessageBox(NULL, "CHECK SUM ERROR, S file seems to be badly formatted.", "ERROR",
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
int CAddressManager::SaveSFile(CString str, Word wBegin, Word wEnd)
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
    if (!file.Open(str, CFile::modeWrite | CFile::modeCreate, &fe))
        return 0;
    while (num_bytes)
    {
        cksum = 0;
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
        file.WriteString(buf_out);
    }
    file.WriteString("S9030000FC\n");
    file.Close();
    return 0;
}

void CAddressManager::Write(Word wIndex, BYTE bVal)
{
    int ind = AddrResTbl[wIndex].devIndex;
    if (ind < 255)
        devices[ind]->Write(AddrResTbl[wIndex].decodedAddr, bVal, false);
    else
        memory[wIndex] = bVal;
    wLastWrite = wIndex;
}

BYTE CAddressManager::Read(Word wIndex, bool bDbg)
{
    BYTE val;
    int  ind = AddrResTbl[wIndex].devIndex;
    if (ind < 255)
        devices[ind]->Read(AddrResTbl[wIndex].decodedAddr, val, bDbg);
    else
        val = memory[wIndex];
    wLastRead = wIndex;
    return val;
}

void CAddressManager::Create(CEnvironment *pEnv)
{
    CStrArray sa;
    memset(memory, 0, 0x10000);
    this->pEnv = pEnv;
    // read the device file
    xp.ParseFile((CWnd *) pEnv->GetMainWnd(), pEnv->GetDeviceFile(), devices, AddrResTbl);
}

CAddressManager::CAddressManager()
{
}

CAddressManager::~CAddressManager()
{
}
