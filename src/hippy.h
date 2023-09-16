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
#ifndef _HIPPY_H__
#define _HIPPY_H__

#define WM_UPDATEDBGWND WM_USER + 100
#define WM_MEMLOCCHANGE WM_USER + 101
#define WM_JUMPTOLINE WM_USER + 102
#define WM_HIDEBUILDWND WM_USER + 103
#define WM_REDRAWALL WM_USER + 104

#define INT_IRQ "HIPPY_IRQ_INT_SEM"
#define INT_NMI "HIPPY_NMI_INT_SEM"
#define INT_RST "HIPPY_RST_INT_SEM"

typedef unsigned char  BYTE;
typedef unsigned short ADDRESS;
typedef unsigned short LINENUMBER;
typedef unsigned short Word;

typedef union CCR
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
} CCR;

typedef struct Registers
{
    BYTE    a;
    BYTE    b;
    ADDRESS pc;
    ADDRESS x;
    ADDRESS sp;
    CCR     ccr;
} Registers, *PRegisters;

#define IMPLEMENT_STACK() \
    int  indStack = 0;    \
    char stack[30]
#define POP() stack[--indStack]
#define PUSH(x) stack[indStack++] = (x)

typedef enum
{
    bitOR = 1,
    bitXOR,
    bitAND,
    bitNOT,
    leftPrnt
} Operator;

#endif