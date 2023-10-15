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
#ifndef HIPPY_ASM_ACTIONS_H
#define HIPPY_ASM_ACTIONS_H

#include "codes.h"
#include "lexer.h"
#include "sym_table.h"

#include <stdio.h>
#include <vector>

#define HI_BYTE(y) (unsigned char) (y >> 8)
#define LO_BYTE(y) (unsigned char) (y & 0xff)

#pragma warning(disable : 4065) /* Switch statement contains default but no case. */
int yylex();
int yyparse();

extern int  num_errors;
extern int  my_linenum;
extern void err_msg(const char *fmt, ...);
extern void yyerror(const char *);

extern FILE                       *fout;
extern int                         pc; // where to write next
extern std::vector<unsigned short> segments;
extern std::vector<unsigned short> segment_length;
extern unsigned short              byte_list[40];
extern int                         num_byte_list;

extern Equate       *equations;
extern int           num_eqns;
extern unsigned char memory[0x10000]; // memory image

void           store_eqn(void *eqn, unsigned short pc, Mode mode);
int            get_numbytes(unsigned char opcode);
unsigned short get_tkval(void *vptk);
Type           get_tktype(void *vptk);
void          *make_number(unsigned short sval);
void          *make_identifier(void *pvpse);
void          *make_eqn(char opr, void *vpl, void *vpr);
void           add_label(void *vppse, unsigned short addr);
void           add_reference(unsigned char opcode, void *vppse, unsigned short addr);
void           do_fill(unsigned char val, unsigned short cnt);
void           do_fcb();
void           do_fdb();
void           do_fcc(char *str);
void           do_zmb(unsigned short cnt);
void           do_rmb(unsigned short size);
void           do_org(int addr);
void           do_equ(void *vppse, unsigned short addr);
void           do_idinst(unsigned char opcode, SymbolEntry *pse, char imm);
void           do_inher(unsigned char opcode);
void           do_immediate(unsigned char opcode, unsigned short sval);
void           do_direct(unsigned char opcode, unsigned char cval);
void           do_indexed(unsigned char opcode, unsigned char cval);
void           do_extended(unsigned char opcode, unsigned short sval);
void           do_relative(unsigned char opcode, unsigned char cval);
void           generate_hex();

#endif
