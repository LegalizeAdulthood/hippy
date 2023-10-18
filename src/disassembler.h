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
#ifndef HIPPY_DISASSEMBLER_H
#define HIPPY_DISASSEMBLER_H

#include "hippy.h"

/* class for producing hexadecimal representation of
 * given byte arrays.
 */

class HexDumper
{
public:
    static int  ByteArrayToHexArray(BYTE *pbByteArray, int szByteArray, char *pcStr);
    static void ByteArrayToHexArrayEx(BYTE *pbByteArray, int szByteArray, char *pcStr);
    static void ByteToHex(BYTE b, char *pcStr);
};

enum AddressingMode
{
    amNon,
    amRel,
    amImp,
    amInd,
    amExt,
    amDir,
    amImm
};

extern const BYTE           g_instDescTbl[0x100];
extern const AddressingMode g_instModes[0x100];

#define SZ_CODELEN 10   // A diseassembled code is this much bytes (0x32 stuffing is used)

/* class for disassembling given byte array
 */
class Disassembler
{
public:
    int Dasm(BYTE *pbByteArray, int szByteArray, char *pcStr);
};

#endif
