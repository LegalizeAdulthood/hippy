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
#include "alu.h"

/*
 */
void SetBit(BYTE *val, bool state, int pos)
{
    if (state)
        *val = *val | (1 << pos);
    else
        *val = *val & ~(1 << pos);
}

bool GetBitb(BYTE val, int pos)
{
    BYTE b = 1 << pos;
    return b == (val & b);
}

bool GetBiti(int val, int pos)
{
    int i = 1 << pos;
    return i == (val & i);
}

bool GetBitw(Word val, int pos)
{
    Word i = 1 << pos;
    return i == (val & i);
}

/*
 */

void ALU::ResetCCR()
{
    pccr->all &= 0xD0; /* 1101 0000*/
}

void ALU::CheckRESULT()
{
    pccr->n = GetBitb(RESULT, 7);
    pccr->z = !RESULT;
}

BYTE ALU::Add(BYTE v1, BYTE v2)
{
    ResetCCR();
    RESULT = v1 + v2;

    { // HALF CARRY
        bool a3, b3, r3;

        a3 = GetBitb(v1, 3);
        b3 = GetBitb(v2, 3);
        r3 = GetBitb(RESULT, 3);

        pccr->h = (a3 && b3) || (b3 && !r3) || (!r3 && a3);
    }

    { // CARRY
        bool a7, b7, r7;

        a7 = GetBitb(v1, 7);
        b7 = GetBitb(v2, 7);
        r7 = GetBitb(RESULT, 7);

        pccr->c = (a7 && b7) || (b7 && !r7) || (!r7 && a7);
        // OVERFLOW
        pccr->v = (a7 && b7 && !r7) || (!a7 && !b7 && r7);
    }

    CheckRESULT(); /* (N), (Z) */
    return RESULT;
}

BYTE ALU::Addc(BYTE v1, BYTE v2)
{
    RESULT = v1 + v2 + pccr->c;

    { // HALF CARRY
        bool a3, b3, r3;

        a3 = GetBitb(v1, 3);
        b3 = GetBitb(v2, 3);
        r3 = GetBitb(RESULT, 3);

        pccr->h = (a3 && b3) || (b3 && !r3) || (!r3 && a3);
    }

    { // CARRY
        bool a7, b7, r7;

        a7 = GetBitb(v1, 7);
        b7 = GetBitb(v2, 7);
        r7 = GetBitb(RESULT, 7);

        pccr->c = (a7 && b7) || (b7 && !r7) || (!r7 && a7);
        // OVERFLOW
        pccr->v = (a7 && b7 && !r7) || (!a7 && !b7 && r7);
    }

    CheckRESULT(); /* (N), (Z) */
    return RESULT;
}

BYTE ALU::And(BYTE v1, BYTE v2)
{
    pccr->v = 0;
    RESULT = v1 & v2;
    CheckRESULT();
    return RESULT;
}

BYTE ALU::Asl(BYTE v1)
{
    pccr->c = GetBitb(v1, 7);
    RESULT = v1 << 1;
    CheckRESULT();
    pccr->v = pccr->c ^ pccr->n;
    return RESULT;
}

BYTE ALU::Asr(BYTE v1)
{
    pccr->c = GetBitb(v1, 0);
    RESULT = v1 >> 1;
    SetBit(&RESULT, GetBitb(v1, 7), 7);
    CheckRESULT();
    pccr->v = pccr->c ^ pccr->n;
    return RESULT;
}

BYTE ALU::Com(BYTE v1)
{
    RESULT = (BYTE) ~v1;
    CheckRESULT();
    pccr->c = 1;
    pccr->v = 0;
    return RESULT;
}

BYTE ALU::Lsr(BYTE v1)
{
    pccr->c = GetBitb(v1, 0);
    RESULT = v1 >> 1;
    CheckRESULT();
    pccr->v = pccr->c ^ pccr->n;
    return RESULT;
}

BYTE ALU::Neg(BYTE v1)
{
    RESULT = BYTE(~(v1 - 1));
    pccr->c = (RESULT == 0);
    pccr->v = (RESULT == 0x80);
    CheckRESULT();
    return RESULT;
}

BYTE ALU::Or(BYTE v1, BYTE v2)
{
    RESULT = v1 | v2;
    CheckRESULT();
    pccr->v = 0;
    return RESULT;
}

BYTE ALU::Rol(BYTE v1)
{
    BYTE c = pccr->c;
    pccr->c = GetBitb(v1, 7);
    RESULT = v1 << 1;
    if (c)
        RESULT |= 1;
    CheckRESULT();
    pccr->v = pccr->c ^ pccr->n;
    return RESULT;
}

BYTE ALU::Ror(BYTE v1)
{
    BYTE c = pccr->c;
    pccr->c = GetBitb(v1, 0);
    RESULT = v1 >> 1;
    if (c)
        RESULT |= 0x80; /*(1 << 7);*/
    CheckRESULT();
    pccr->v = pccr->c ^ pccr->n;
    return RESULT;
}

/*
 * Sub = v1 - v2
 */
BYTE ALU::Sub(BYTE v1, BYTE v2)
{
    bool x7, m7, r7;

    m7 = GetBitb(v2, 7);
    x7 = GetBitb(v1, 7);

    v2 = ~v2 + 1; // complement

    RESULT = v1 + v2;

    r7 = GetBitb(RESULT, 7);

    pccr->v = (x7 & !m7 & !r7) | (!x7 & m7 & r7);
    pccr->c = (!x7 & m7) | (m7 & r7) | (r7 & !x7);
    pccr->n = r7;
    pccr->z = !RESULT;
    return RESULT;
}

Word ALU::SubExt(Word v1, Word v2)
{
    Word         res;
    unsigned int i;
    bool         xh7, m7, rh7;

    m7 = GetBitw(v2, 7);
    v2 = ~v2 + 1; // complement
    i = v1 + v2;
    res = i & 0xffff;
    xh7 = GetBitw(v1, 7);
    rh7 = GetBitw(res, 7);

    pccr->v = (xh7 && !m7 && !rh7) || (!xh7 && m7 && rh7);
    pccr->n = rh7;
    pccr->z = !res;

    return res;
}

BYTE ALU::Subc(BYTE v1, BYTE v2)
{
    bool x7, m7, r7;

    x7 = GetBitb(v1, 7);
    m7 = GetBitb(v2, 7);
    v2 = (~v2) + 1;
    RESULT = v1 + v2 - pccr->c;
    r7 = GetBitb(RESULT, 7);

    pccr->v = (x7 && !m7 && !r7) || (!x7 && m7 && r7);
    pccr->c = (!x7 && m7) || (m7 && r7) || (r7 && !x7);
    CheckRESULT();
    return RESULT;
}

BYTE ALU::Tst(BYTE v1)
{
    return Sub(v1, 0);
}

BYTE ALU::Xor(BYTE v1, BYTE v2)
{
    RESULT = v1 ^ v2;
    CheckRESULT();
    pccr->v = 0;
    return RESULT;
}

BYTE ALU::Clr()
{
    pccr->c = 0;
    pccr->v = 0;
    pccr->n = 0;
    pccr->z = 1;
    return 0;
}

BYTE ALU::Daa(BYTE v1)
{
    BYTE upper = (v1 & 0xF0) >> 4;
    BYTE lower = v1 & 0x0F;
    BYTE c = 0;

    if (pccr->h || (lower > 0x09))
        c |= 0x06;

    if (pccr->c || (upper > 0x09) || (upper > 0x08 && lower > 0x09))
        c |= 0x60;

    RESULT = v1 + c;

    if (upper > 0x09)
        pccr->c = 1;
    CheckRESULT();

    return RESULT;
}

BYTE ALU::Dec(BYTE v1)
{

    RESULT = v1 - 1;
    CheckRESULT();
    pccr->v = (v1 == 0x80);

    return RESULT;
}

BYTE ALU::Eor(BYTE v1, BYTE v2)
{
    RESULT = v1 ^ v2;
    pccr->v = 0;
    CheckRESULT();
    return RESULT;
}

BYTE ALU::Inc(BYTE v1)
{
    RESULT = v1 + 1;
    pccr->v = (v1 == 0x7F);
    CheckRESULT();
    return RESULT;
}