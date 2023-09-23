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
#include "disassembler.h"

#include <stdio.h>
#include <string.h>

static constexpr char s_hexTable[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/*
 * Here are the mnemonics for the 6800 instructions.
 * Directly adapted from the table 4-1 from the book U.Mp. and MC by William C. Wray
 */

static constexpr const char *const s_codes[0x100] = {
    nullptr, "NOP",   nullptr, nullptr, nullptr, nullptr, "TAP",   "TPA",   "INX",   "DEX",   "CLV",   "SEV",   "CLC",
    "SEC",   "CLI",   "SEI",   "SBA",   "CBA",   nullptr, nullptr, nullptr, nullptr, "TAB",   "TBA",   nullptr, "DAA",
    nullptr, "ABA",   nullptr, nullptr, nullptr, nullptr, "BRA",   nullptr, "BHI",   "BLS",   "BCC",   "BCS",   "BNE",
    "BEQ",   "BVC",   "BVS",   "BPL",   "BMI",   "BGE",   "BLT",   "BGT",   "BLE",   "TSX",   "INS",   "PULA",  "PULB",
    "DES",   "TXS",   "PSHA",  "PSHB",  nullptr, "RTS",   nullptr, "RTI",   nullptr, nullptr, "WAI",   "SWI",   "NEGA",
    nullptr, nullptr, "COMA",  "LSRA",  nullptr, "RORA",  "ASRA",  "ASLA",  "ROLA",  "DECA",  nullptr, "INCA",  "TSTA",
    nullptr, "CLRA",  "NEGB",  nullptr, nullptr, "COMB",  "LSRB",  nullptr, "RORB",  "ASRB",  "ASLB",  "ROLB",  "DECB",
    nullptr, "INCB",  "TSTB",  nullptr, "CLRB",  "NEG",   nullptr, nullptr, "COM",   "LSR",   nullptr, "ROR",   "ASR",
    "ASL",   "ROL",   "DEC",   nullptr, "INC",   "TST",   "JMP",   "CLR",   "NEG",   nullptr, nullptr, "COM",   "LSR",
    nullptr, "ROR",   "ASR",   "ASL",   "ROL",   "DEC",   nullptr, "INC",   "TST",   "JMP",   "CLR",   "SUBA",  "CMPA",
    "SBCA",  nullptr, "ANDA",  "BITA",  "LDAA",  nullptr, "EORA",  "ADCA",  "ORAA",  "ADDA",  "CPX",   "BSR",   "LDS",
    nullptr, "SUBA",  "CMPA",  "SBCA",  nullptr, "ANDA",  "BITA",  "LDAA",  "STAA",  "EORA",  "ADCA",  "ORAA",  "ADDA",
    "CPXA",  nullptr, "LDS",   "STS",   "SUBA",  "CMPA",  "SBCA",  nullptr, "ANDA",  "BITA",  "LDAA",  "STAA",  "EORA",
    "ADCA",  "ORAA",  "ADDA",  "CPX",   "JSR",   "LDS",   "STS",   "SUBA",  "CMPA",  "SBCA",  nullptr, "ANDA",  "BITA",
    "LDAA",  "STAA",  "EORA",  "ADCA",  "ORAA",  "ADDA",  "CPX",   "JSR",   "LDS",   "STS",   "SUBB",  "CMPB",  "SBCB",
    nullptr, "ANDB",  "BITB",  "LDAB",  nullptr, "EORB",  "ADCB",  "ORAB",  "ADDB",  nullptr, nullptr, "LDX",   nullptr,
    "SUBB",  "CMPB",  "SBCB",  nullptr, "ANDB",  "BITB",  "LDAB",  "STAB",  "EORB",  "ADCB",  "ORAB",  "ADDB",  nullptr,
    nullptr, "LDX",   "STX",   "SUBB",  "CMPB",  "SBCB",  nullptr, "ANDB",  "BITB",  "LDAB",  "STAB",  "EORB",  "ADCB",
    "ORAB",  "ADDB",  nullptr, nullptr, "LDX",   "STX",   "SUBB",  "CMPB",  "SBCB",  nullptr, "ANDB",  "BITB",  "LDAB",
    "STAB",  "EORB",  "ADCB",  "ORAB",  "ADDB",  nullptr, nullptr, "LDX",   "STX"};

/*
 *  INSTRUCTION DESCRIPTION TABLE (IDT)
 *  HIGHER ORDER 4BITS --> CLOCK CYCLES
 *  LOWER  ORDER 4BITS --> NUMBER OF BYTES
 */

const BYTE g_instDescTbl[0x100] = {
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

const AddressingMode g_instModes[0x100] = {
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

/* Converts the given array of bytes into a string containing their hexadecimal
 * representations. assumes that the buffer, pcStr is twice as much as the requested
 * number of bytes to decode (szByteArray) returns the number of bytes written.
 */
int HexDumper::ByteArrayToHexArray(BYTE *pbByteArray, int szByteArray, char *pcStr)
{
    char *p = pcStr;
    while (szByteArray)
    {
        pcStr[0] = s_hexTable[*pbByteArray / 16];
        pcStr[1] = s_hexTable[*pbByteArray % 16];
        pcStr += 2;
        pbByteArray++;
        szByteArray--;
    }

    pcStr[0] = 0;
    return (int) (pcStr - p);
}

/* Converts the given array of bytes into a string containing their hexadecimal
 * representations. assumes that the buffer, pcStr is twice more than the requested
 * number of bytes to decode (szByteArray). This function also inserts blank characters
 * after each byte.
 */
void HexDumper::ByteArrayToHexArrayEx(BYTE *pbByteArray, int szByteArray, char *pcStr)
{
    while (szByteArray)
    {
        pcStr[0] = s_hexTable[*pbByteArray / 16];
        pcStr[1] = s_hexTable[*pbByteArray % 16];
        pcStr[2] = ' ';
        pcStr += 3;
        pbByteArray++;
        szByteArray--;
    }

    pcStr[0] = 0;
}

/* Simply outputs the hexadecimal representaion of "b" into pcStr. Which should be
 * 3 chars (minimum). Null character at the end is also put.
 */
void HexDumper::ByteToHex(BYTE b, char *pcStr)
{
    pcStr[0] = s_hexTable[b / 16];
    pcStr[1] = s_hexTable[b % 16];
    pcStr[2] = 0;
}

/*
 * Disassembles given byte array os size szByteArray. Writes the out put to
 * pcSTr. Only Disassembles one instruction and returns the length (in bytes)
 * of that instruction. if left most byte is not a val,d instruction or
 * the length of that instruction is larger than szByteArray then returns -1.
 * in that case INVALID is coppied to the pcStr.
 */
int Disassembler::Dasm(BYTE *pbByteArray, int szByteArray, char *pcStr)
{
    int  opcode = *pbByteArray;
    int  numread = g_instDescTbl[opcode] & 0x0F;
    char pcAddr[5];

    if (numread > szByteArray || !opcode)
    {
        strcpy(pcStr, "INVALID");
        return INVALID_CODE;
    }

    if (numread == 2)
    {
        HexDumper::ByteToHex(pbByteArray[1], pcAddr);
    }
    else if (numread == 3)
    {
        HexDumper::ByteArrayToHexArray(&pbByteArray[1], 2, pcAddr);
    }

    switch (g_instModes[opcode])
    {
    case amNon:
        strcpy(pcStr, "INVALID");
        return INVALID_CODE;
    case amRel:
    case amDir:
    case amExt:
        sprintf(pcStr, strlen(s_codes[opcode]) == 3 ? "%s      $%s" : "%s     $%s", s_codes[opcode], pcAddr);
        break;
    case amImm:
        sprintf(pcStr, strlen(s_codes[opcode]) == 3 ? "%s      #$%s" : "%s     #$%s", s_codes[opcode], pcAddr);
        break;
    case amInd:
        sprintf(pcStr, "%s     $%s, X", s_codes[opcode], pcAddr);
        break;
    case amImp:
        strcpy(pcStr, s_codes[opcode]);
        break;
    }

    return numread;
}
