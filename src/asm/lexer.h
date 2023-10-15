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
#ifndef HIPPY_LEXER_H
#define HIPPY_LEXER_H

#define NUM_INST 107

/*
 * assebler directives listing
 */
#define NUM_DIRECTIVES 18

extern const char *const dirList[NUM_DIRECTIVES];

extern const unsigned int dirHashList[NUM_DIRECTIVES];

/*
 * instruction listing
 */
extern const char *const instList[NUM_INST + 1];

enum Instruction
{
    ASL,
    BMI,
    ASR,
    BLS,
    BNE,
    SEC,
    BLT,
    DAA,
    SEI,
    EORA,
    EORB,
    NEG,
    SEV,
    TAB,
    BPL,
    BRA,
    ANDA,
    ANDB,
    LSR,
    TBA,
    ROL,
    CLC,
    DEC,
    TAP,
    ROR,
    CLI,
    ADCA,
    ADCB,
    LSRA,
    LSRB,
    ADDA,
    ADDB,
    BSR,
    CLR,
    DES,
    CLV,
    BVC,
    DEX,
    COMA,
    COMB,
    RTI,
    COM,
    PSHA,
    PSHB,
    ASLA,
    BVS,
    ASLB,
    RTS,
    STAA,
    ROLA,
    STAB,
    ROLB,
    CPX,
    INC,
    DECA,
    NOP,
    DECB,
    INS,
    INX,
    ASRA,
    ASRB,
    RORA,
    RORB,
    ABA,
    STS,
    TPA,
    STX,
    SUBA,
    SUBB,
    SWI,
    ORAA,
    ORAB,
    JMP,
    TST,
    TSX,
    WAI,
    BCC,
    LDS,
    LDX,
    JSR,
    BCS,
    PULA,
    PULB,
    TXS,
    CLRA,
    CLRB,
    INCA,
    INCB,
    BEQ,
    BGE,
    LDAA,
    LDAB,
    CBA,
    BHI,
    BGT,
    NEGA,
    NEGB,
    SBCA,
    SBCB,
    SBA,
    CMPA,
    BITA,
    CMPB,
    BITB,
    BLE,
    TSTA,
    TSTB
};

extern const unsigned int hashList[NUM_INST];

extern const unsigned int hashInd[NUM_INST];

#define MAX_ID_LENGTH 256

extern int my_linenum;
extern int num_errors;
int  do_id(const char *text);
void yyerror(const char *msg);
int  str2int(const char *str, int base);

#endif
