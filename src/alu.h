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
#ifndef HIPPY_ALU_H
#define HIPPY_ALU_H

#include "hippy.h"

class ALU
{
public:
    ALU(CCR *pccr) :
        m_pccr(pccr),
        m_result{}
    {
        ResetCCR();
    }
    BYTE Add(BYTE v1, BYTE v2);
    BYTE Addc(BYTE v1, BYTE v2);
    BYTE And(BYTE v1, BYTE v2);
    BYTE Asl(BYTE v1);
    BYTE Asr(BYTE v1);
    BYTE Com(BYTE v1);
    BYTE Lsr(BYTE v1);
    BYTE Neg(BYTE v1);
    BYTE Or(BYTE v1, BYTE v2);
    BYTE Rol(BYTE v1);
    BYTE Ror(BYTE v1);
    BYTE Sub(BYTE v1, BYTE v2);
    Word SubExt(Word v1, Word v2);
    BYTE Subc(BYTE v1, BYTE v2);
    BYTE Tst(BYTE v1);
    BYTE Xor(BYTE v1, BYTE v2);
    BYTE Clr();
    BYTE Daa(BYTE v1);
    BYTE Dec(BYTE v1);
    BYTE Eor(BYTE v1, BYTE v2);
    BYTE Inc(BYTE v1);

private:
    CCR *m_pccr;
    BYTE m_result;
    void ResetCCR();
    void CheckRESULT();
};

#endif
