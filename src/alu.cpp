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

void SetBit(BYTE *val, bool state, int pos)
{
    if (state)
        *val |= 1U << pos;
    else
        *val &= ~(1U << pos);
}

bool GetBitb(BYTE val, int pos)
{
    BYTE b = 1U << pos;
    return b == (val & b);
}

bool GetBiti(int val, int pos)
{
    int i = 1U << pos;
    return i == (val & i);
}

bool GetBitw(Word val, int pos)
{
    Word i = 1U << pos;
    return i == (val & i);
}

/*
 */

void ALU::ResetCCR()
{
    m_pccr->all &= 0xD0; /* 1101 0000*/
}

void ALU::CheckRESULT()
{
    m_pccr->n = GetBitb(m_result, 7);
    m_pccr->z = !m_result;
}

BYTE ALU::Add(BYTE v1, BYTE v2)
{
    ResetCCR();
    m_result = v1 + v2;

    { // HALF CARRY
        bool a3 = GetBitb(v1, 3);
        bool b3 = GetBitb(v2, 3);
        bool r3 = GetBitb(m_result, 3);

        m_pccr->h = (a3 && b3) || (b3 && !r3) || (!r3 && a3);
    }

    { // CARRY
        bool a7 = GetBitb(v1, 7);
        bool b7 = GetBitb(v2, 7);
        bool r7 = GetBitb(m_result, 7);

        m_pccr->c = (a7 && b7) || (b7 && !r7) || (!r7 && a7);
        // OVERFLOW
        m_pccr->v = (a7 && b7 && !r7) || (!a7 && !b7 && r7);
    }

    CheckRESULT(); /* (N), (Z) */
    return m_result;
}

BYTE ALU::Addc(BYTE v1, BYTE v2)
{
    m_result = v1 + v2 + m_pccr->c;

    { // HALF CARRY
        bool a3 = GetBitb(v1, 3);
        bool b3 = GetBitb(v2, 3);
        bool r3 = GetBitb(m_result, 3);

        m_pccr->h = (a3 && b3) || (b3 && !r3) || (!r3 && a3);
    }

    { // CARRY
        bool a7 = GetBitb(v1, 7);
        bool b7 = GetBitb(v2, 7);
        bool r7 = GetBitb(m_result, 7);

        m_pccr->c = (a7 && b7) || (b7 && !r7) || (!r7 && a7);
        // OVERFLOW
        m_pccr->v = (a7 && b7 && !r7) || (!a7 && !b7 && r7);
    }

    CheckRESULT(); /* (N), (Z) */
    return m_result;
}

BYTE ALU::And(BYTE v1, BYTE v2)
{
    m_pccr->v = 0;
    m_result = v1 & v2;
    CheckRESULT();
    return m_result;
}

BYTE ALU::Asl(BYTE v1)
{
    m_pccr->c = GetBitb(v1, 7);
    m_result = v1 << 1;
    CheckRESULT();
    m_pccr->v = m_pccr->c ^ m_pccr->n;
    return m_result;
}

BYTE ALU::Asr(BYTE v1)
{
    m_pccr->c = GetBitb(v1, 0);
    m_result = v1 >> 1;
    SetBit(&m_result, GetBitb(v1, 7), 7);
    CheckRESULT();
    m_pccr->v = m_pccr->c ^ m_pccr->n;
    return m_result;
}

BYTE ALU::Com(BYTE v1)
{
    m_result = (BYTE) ~v1;
    CheckRESULT();
    m_pccr->c = 1;
    m_pccr->v = 0;
    return m_result;
}

BYTE ALU::Lsr(BYTE v1)
{
    m_pccr->c = GetBitb(v1, 0);
    m_result = v1 >> 1;
    CheckRESULT();
    m_pccr->v = m_pccr->c ^ m_pccr->n;
    return m_result;
}

BYTE ALU::Neg(BYTE v1)
{
    m_result = BYTE(~(v1 - 1));
    m_pccr->c = (m_result == 0);
    m_pccr->v = (m_result == 0x80);
    CheckRESULT();
    return m_result;
}

BYTE ALU::Or(BYTE v1, BYTE v2)
{
    m_result = v1 | v2;
    CheckRESULT();
    m_pccr->v = 0;
    return m_result;
}

BYTE ALU::Rol(BYTE v1)
{
    BYTE c = m_pccr->c;
    m_pccr->c = GetBitb(v1, 7);
    m_result = v1 << 1;
    if (c)
    {
        m_result |= 1;
    }
    CheckRESULT();
    m_pccr->v = m_pccr->c ^ m_pccr->n;
    return m_result;
}

BYTE ALU::Ror(BYTE v1)
{
    BYTE c = m_pccr->c;
    m_pccr->c = GetBitb(v1, 0);
    m_result = v1 >> 1;
    if (c)
    {
        m_result |= 0x80; /*(1 << 7);*/
    }
    CheckRESULT();
    m_pccr->v = m_pccr->c ^ m_pccr->n;
    return m_result;
}

/*
 * Sub = v1 - v2
 */
BYTE ALU::Sub(BYTE v1, BYTE v2)
{
    bool m7 = GetBitb(v2, 7);
    bool x7 = GetBitb(v1, 7);

    v2 = ~v2 + 1; // complement

    m_result = v1 + v2;

    bool r7 = GetBitb(m_result, 7);

    m_pccr->v = (x7 & !m7 & !r7) | (!x7 & m7 & r7);
    m_pccr->c = (!x7 & m7) | (m7 & r7) | (r7 & !x7);
    m_pccr->n = r7;
    m_pccr->z = !m_result;
    return m_result;
}

Word ALU::SubExt(Word v1, Word v2)
{
    bool m7 = GetBitw(v2, 7);
    v2 = ~v2 + 1; // complement
    unsigned int i = v1 + v2;
    Word         res = i & 0xffff;
    bool         xh7 = GetBitw(v1, 7);
    bool         rh7 = GetBitw(res, 7);

    m_pccr->v = (xh7 && !m7 && !rh7) || (!xh7 && m7 && rh7);
    m_pccr->n = rh7;
    m_pccr->z = !res;

    return res;
}

BYTE ALU::Subc(BYTE v1, BYTE v2)
{
    bool x7 = GetBitb(v1, 7);
    bool m7 = GetBitb(v2, 7);
    v2 = (~v2) + 1;
    m_result = v1 + v2 - m_pccr->c;
    bool r7 = GetBitb(m_result, 7);

    m_pccr->v = (x7 && !m7 && !r7) || (!x7 && m7 && r7);
    m_pccr->c = (!x7 && m7) || (m7 && r7) || (r7 && !x7);
    CheckRESULT();
    return m_result;
}

BYTE ALU::Tst(BYTE v1)
{
    return Sub(v1, 0);
}

BYTE ALU::Xor(BYTE v1, BYTE v2)
{
    m_result = v1 ^ v2;
    CheckRESULT();
    m_pccr->v = 0;
    return m_result;
}

BYTE ALU::Clr()
{
    m_pccr->c = 0;
    m_pccr->v = 0;
    m_pccr->n = 0;
    m_pccr->z = 1;
    return 0;
}

BYTE ALU::Daa(BYTE v1)
{
    BYTE upper = (v1 & 0xF0) >> 4;
    BYTE lower = v1 & 0x0F;
    BYTE c = 0;

    if (m_pccr->h || (lower > 0x09))
        c |= 0x06;

    if (m_pccr->c || (upper > 0x09) || (upper > 0x08 && lower > 0x09))
        c |= 0x60;

    m_result = v1 + c;

    if (upper > 0x09)
        m_pccr->c = 1;
    CheckRESULT();

    return m_result;
}

BYTE ALU::Dec(BYTE v1)
{
    m_result = v1 - 1;
    CheckRESULT();
    m_pccr->v = (v1 == 0x80);

    return m_result;
}

BYTE ALU::Eor(BYTE v1, BYTE v2)
{
    m_result = v1 ^ v2;
    m_pccr->v = 0;
    CheckRESULT();
    return m_result;
}

BYTE ALU::Inc(BYTE v1)
{
    m_result = v1 + 1;
    m_pccr->v = (v1 == 0x7F);
    CheckRESULT();
    return m_result;
}
