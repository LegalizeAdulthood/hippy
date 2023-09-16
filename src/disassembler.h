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
#ifndef _DISASSEMBLER_H__
#define _DISASSEMBLER_H__

#include "hippy.h"
#include <afx.h>
#include <stdio.h>
#include <string.h>

const char HexTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

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

/*
 * Here are the mnemonics for the 6800 instructions.
 * Directly adapted from the table 4-1 from the book U.Mp. and MC by William C. Wray
 */

static char *codes[0x100] = {
    0,      "NOP",  0,      0,      0,      0,      "TAP",  "TPA",  "INX",  "DEX",  "CLV",  "SEV",  "CLC",  "SEC",
    "CLI",  "SEI",  "SBA",  "CBA",  0,      0,      0,      0,      "TAB",  "TBA",  0,      "DAA",  0,      "ABA",
    0,      0,      0,      0,      "BRA",  0,      "BHI",  "BLS",  "BCC",  "BCS",  "BNE",  "BEQ",  "BVC",  "BVS",
    "BPL",  "BMI",  "BGE",  "BLT",  "BGT",  "BLE",  "TSX",  "INS",  "PULA", "PULB", "DES",  "TXS",  "PSHA", "PSHB",
    0,      "RTS",  0,      "RTI",  0,      0,      "WAI",  "SWI",  "NEGA", 0,      0,      "COMA", "LSRA", 0,
    "RORA", "ASRA", "ASLA", "ROLA", "DECA", 0,      "INCA", "TSTA", 0,      "CLRA", "NEGB", 0,      0,      "COMB",
    "LSRB", 0,      "RORB", "ASRB", "ASLB", "ROLB", "DECB", 0,      "INCB", "TSTB", 0,      "CLRB", "NEG",  0,
    0,      "COM",  "LSR",  0,      "ROR",  "ASR",  "ASL",  "ROL",  "DEC",  0,      "INC",  "TST",  "JMP",  "CLR",
    "NEG",  0,      0,      "COM",  "LSR",  0,      "ROR",  "ASR",  "ASL",  "ROL",  "DEC",  0,      "INC",  "TST",
    "JMP",  "CLR",  "SUBA", "CMPA", "SBCA", 0,      "ANDA", "BITA", "LDAA", 0,      "EORA", "ADCA", "ORAA", "ADDA",
    "CPX",  "BSR",  "LDS",  0,      "SUBA", "CMPA", "SBCA", 0,      "ANDA", "BITA", "LDAA", "STAA", "EORA", "ADCA",
    "ORAA", "ADDA", "CPXA", 0,      "LDS",  "STS",  "SUBA", "CMPA", "SBCA", 0,      "ANDA", "BITA", "LDAA", "STAA",
    "EORA", "ADCA", "ORAA", "ADDA", "CPX",  "JSR",  "LDS",  "STS",  "SUBA", "CMPA", "SBCA", 0,      "ANDA", "BITA",
    "LDAA", "STAA", "EORA", "ADCA", "ORAA", "ADDA", "CPX",  "JSR",  "LDS",  "STS",  "SUBB", "CMPB", "SBCB", 0,
    "ANDB", "BITB", "LDAB", 0,      "EORB", "ADCB", "ORAB", "ADDB", 0,      0,      "LDX",  0,      "SUBB", "CMPB",
    "SBCB", 0,      "ANDB", "BITB", "LDAB", "STAB", "EORB", "ADCB", "ORAB", "ADDB", 0,      0,      "LDX",  "STX",
    "SUBB", "CMPB", "SBCB", 0,      "ANDB", "BITB", "LDAB", "STAB", "EORB", "ADCB", "ORAB", "ADDB", 0,      0,
    "LDX",  "STX",  "SUBB", "CMPB", "SBCB", 0,      "ANDB", "BITB", "LDAB", "STAB", "EORB", "ADCB", "ORAB", "ADDB",
    0,      0,      "LDX",  "STX"};

/*
 *	INSTRUCTION DESCRIPTION TABLE (IDT)
 *	HIGHER ORDER 4BITS --> CLOCK CYCLES
 *  LOWER  ORDER 4BITS --> NUMBER OF BYTES
 */

static BYTE InstDescTbl[0x100] = {
    0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x21, 0x21, 0x41, 0x41, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x00,
    0x00, 0x00, 0x00, 0x21, 0x21, 0x00, 0x21, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x42, 0x42, 0x42, 0x42,
    0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x00,
    0x51, 0x00, 0xA1, 0x00, 0x00, 0x91, 0xC1, 0x21, 0x00, 0x00, 0x21, 0x21, 0x00, 0x21, 0x21, 0x21, 0x21, 0x21, 0x00,
    0x21, 0x21, 0x00, 0x21, 0x21, 0x00, 0x00, 0x21, 0x21, 0x00, 0x21, 0x21, 0x21, 0x21, 0x21, 0x00, 0x21, 0x21, 0x00,
    0x21, 0x72, 0x00, 0x00, 0x72, 0x72, 0x00, 0x72, 0x72, 0x72, 0x72, 0x72, 0x00, 0x72, 0x72, 0x42, 0x72, 0x63, 0x00,
    0x00, 0x63, 0x63, 0x00, 0x63, 0x63, 0x63, 0x63, 0x63, 0x00, 0x63, 0x63, 0x33, 0x63, 0x22, 0x22, 0x22, 0x00, 0x22,
    0x22, 0x22, 0x00, 0x22, 0x22, 0x22, 0x22, 0x33, 0x82, 0x33, 0x00, 0x32, 0x32, 0x32, 0x00, 0x32, 0x32, 0x32, 0x32,
    0x32, 0x32, 0x32, 0x32, 0x42, 0x00, 0x42, 0x52, 0x52, 0x52, 0x52, 0x00, 0x52, 0x52, 0x52, 0x62, 0x52, 0x52, 0x52,
    0x52, 0x62, 0x82, 0x62, 0x72, 0x43, 0x43, 0x43, 0x00, 0x43, 0x43, 0x43, 0x53, 0x43, 0x43, 0x43, 0x43, 0x53, 0x93,
    0x53, 0x63, 0x22, 0x22, 0x22, 0x00, 0x22, 0x22, 0x22, 0x00, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x33, 0x00, 0x32,
    0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x32, 0x00, 0x00, 0x42, 0x52, 0x52, 0x52, 0x52, 0x00,
    0x52, 0x52, 0x52, 0x62, 0x52, 0x52, 0x52, 0x52, 0x00, 0x00, 0x62, 0x72, 0x43, 0x43, 0x43, 0x00, 0x43, 0x43, 0x43,
    0x53, 0x43, 0x43, 0x43, 0x43, 0x00, 0x00, 0x53, 0x63};

typedef enum
{
    amNon,
    amRel,
    amImp,
    amInd,
    amExt,
    amDir,
    amImm
} AddressingMode;

static AddressingMode InstModes[0x100] = {
    amNon, amImp, amNon, amNon, amNon, amNon, amImp, amImp, amImp, amImp, amImp, amImp, amImp, amImp, amImp, amImp,
    amImp, amImp, amNon, amNon, amNon, amNon, amImp, amImp, amNon, amImp, amNon, amImp, amNon, amNon, amNon, amNon,
    amRel, amNon, amRel, amRel, amRel, amRel, amRel, amRel, amRel, amRel, amRel, amRel, amRel, amRel, amRel, amRel,
    amImp, amImp, amImp, amImp, amImp, amImp, amImp, amImp, amNon, amImp, amNon, amImp, amNon, amNon, amImp, amImp,
    amImp, amNon, amNon, amImp, amImp, amNon, amImp, amImp, amImp, amImp, amImp, amNon, amImp, amImp, amNon, amImp,
    amImp, amNon, amNon, amImp, amImp, amNon, amImp, amImp, amImp, amImp, amImp, amNon, amImp, amImp, amNon, amImp,
    amInd, amNon, amNon, amInd, amInd, amNon, amInd, amInd, amInd, amInd, amInd, amNon, amInd, amInd, amInd, amInd,
    amExt, amNon, amNon, amExt, amExt, amNon, amExt, amExt, amExt, amExt, amExt, amNon, amExt, amExt, amExt, amExt,
    amImm, amImm, amImm, amNon, amImm, amImm, amImm, amNon, amImm, amImm, amImm, amImm, amImm, amRel, amImm, amNon,
    amDir, amDir, amDir, amNon, amDir, amDir, amDir, amDir, amDir, amDir, amDir, amDir, amDir, amNon, amDir, amDir,
    amInd, amInd, amInd, amNon, amInd, amInd, amInd, amInd, amInd, amInd, amInd, amInd, amInd, amInd, amInd, amInd,
    amExt, amExt, amExt, amNon, amExt, amExt, amExt, amExt, amExt, amExt, amExt, amExt, amExt, amExt, amExt, amExt,
    amImm, amImm, amImm, amNon, amImm, amImm, amImm, amNon, amImm, amImm, amImm, amImm, amNon, amNon, amImm, amNon,
    amDir, amDir, amDir, amNon, amDir, amDir, amDir, amDir, amDir, amDir, amDir, amDir, amNon, amNon, amDir, amDir,
    amInd, amInd, amInd, amNon, amInd, amInd, amInd, amInd, amInd, amInd, amInd, amInd, amNon, amNon, amInd, amInd,
    amExt, amExt, amExt, amNon, amExt, amExt, amExt, amExt, amExt, amExt, amExt, amExt, amNon, amNon, amExt, amExt,
};

#define SZ_CODELEN 10   // A diseassembled code is this much bytes (0x32 stuffing is used)
#define INVALID_CODE -1 // returned by Dasm function

/* class for disassembling given byte array
 */
class Disassembler
{
private:
    HexDumper hexer;

public:
    int Dasm(BYTE *pbByteArray, int szByteArray, char *pcStr);
};

#endif