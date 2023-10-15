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
#ifndef HIPPY_ASM_CODES_H
#define HIPPY_ASM_CODES_H

#ifndef BYTE
using BYTE = unsigned char;
using Byte = unsigned char;
#endif

/*
 * instruction descriptor
 * "ic" means Instruction Code
 */
struct InstDesc
{
    BYTE icRel;
    BYTE icImmed;
    BYTE icDirect;
    BYTE icIndex;
    BYTE icExtend;
    BYTE icInher;
};

extern const char *const hex_conv_tbl[256];

extern const InstDesc instCodes[107];

/*
 *	INSTRUCTION DESCRIPTION TABLE (IDT)
 *	HIGHER ORDER 4BITS --> CLOCK CYCLES
 *  LOWER  ORDER 4BITS --> NUMBER OF BYTES
 */

extern const unsigned char s_instDescTbl[0x100];

#endif
