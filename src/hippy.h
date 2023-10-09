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
#ifndef HIPPY_H
#define HIPPY_H

#define WM_UPDATEDBGWND WM_USER + 100
#define WM_MEMLOCCHANGE WM_USER + 101
#define WM_JUMPTOLINE WM_USER + 102
#define WM_HIDEBUILDWND WM_USER + 103
#define WM_REDRAWALL WM_USER + 104

#define INT_IRQ wxT("HIPPY_IRQ_INT_SEM")
#define INT_NMI wxT("HIPPY_NMI_INT_SEM")
#define INT_RST wxT("HIPPY_RST_INT_SEM")

using BYTE = unsigned char;
using ADDRESS = unsigned short;
using LINENUMBER = unsigned short;
using Word = unsigned short;

union CCR
{
    BYTE all;
    struct
    {
        BYTE c : 1;
        BYTE v : 1;
        BYTE z : 1;
        BYTE n : 1;
        BYTE i : 1;
        BYTE h : 1;
        BYTE rsv : 2;
    };
};

struct Registers
{
    BYTE    a;
    BYTE    b;
    ADDRESS pc;
    ADDRESS x;
    ADDRESS sp;
    CCR     ccr;
};

enum Operator
{
    bitOR = 1,
    bitXOR,
    bitAND,
    bitNOT,
    leftPrnt
};

#endif
