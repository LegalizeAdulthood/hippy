%{
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

#include "actions.h"

%}

%union{ int ival; void* pse; }
%token INDEX FCB FDB DFB RMB ZMB FCC END ORG DFL DWL NAM DWM DFS OPT EQU BSZ PAGE FILL
%token <ival> INST NUMBER 
%token <pse> ID TEXT
%type <pse> simple_expr expr
%left '+' '-'
%left '*' '/'
%%

program:    asm_stmts
            ;

asm_stmts:
            asm_stmt asm_stmts 
            | /*e*/
            ;

asm_stmt:
            label_def '\n'
            | label_def pure_asm_stmt
            | label_def dir_stmt
            | equ_stmt
            | nam_stmt
            | page_stmt
            | opt_stmt
            | label_def NUMBER '\n'     { err_msg("error: line %04d: instruction or directive expected, found \"%d\"", my_linenum-1,$2);}
            | ID ':' ID '\n'            { err_msg("error: line %04d: instruction or directive expected, found \"%s\"", my_linenum-1,((SymbolEntry*)$3)->str);}
            | ID ID '\n'                { err_msg("error: line %04d: instruction or directive expected, found \"%s\"", my_linenum-1,((SymbolEntry*)$2)->str);}
            | ID ID NUMBER '\n'         { err_msg("error: line %04d: instruction or directive expected, found \"%s\"", my_linenum-1,((SymbolEntry*)$2)->str);}
            | ID ':' ID NUMBER '\n'     { err_msg("error: line %04d: instruction or directive expected, found \"%s\"", my_linenum-1,((SymbolEntry*)$3)->str);}
            ;

pure_asm_stmt:
            INST '\n'                   { do_inher($1); }
            | INST '#' expr '\n' {
                unsigned char opcode = $1;
                if (get_tktype($3) == INTEGER)
                {
                    do_immediate(opcode, get_tkval($3));
                }
                else
                {
                    if (get_numbytes(opcode) == 3)
                    {
                        store_eqn($3, pc + 1, EXTENDED);
                        do_immediate(opcode, 0x0000);
                    }
                    else
                    {
                        store_eqn($3, pc + 1, ONEBYTE);
                        do_immediate(opcode, 0x00);
                    }
                }
            }
            | INST expr '\n' {
                unsigned short sval = get_tkval($2);
                unsigned char  opcode = $1;
                if (get_tktype($2) == INTEGER)
                {
                    if (g_instOpCodes[opcode].icRel)
                    {
                        do_relative(opcode, (unsigned char) (sval - pc - 2));
                    }
                    else if (sval < 0x100 && g_instOpCodes[opcode].icDirect)
                        do_direct(opcode, (unsigned char) sval);
                    else
                        do_extended(opcode, sval);
                }
                else
                {
                    if (g_instOpCodes[opcode].icRel)
                    {
                        store_eqn($2, pc + 1, RELATIVE);
                        do_relative(opcode, 0x00);
                    }
                    else
                    {
                        store_eqn($2, pc + 1, EXTENDED);
                        do_extended(opcode, 0x0000);
                    }
                }
            }
            | INST NUMBER ',' INDEX '\n'    { do_indexed($1, $2); }
            ;

dir_stmt:
            ORG NUMBER '\n'                 { do_org($2); }
            | fcb byte_list                 { /*do_fcb();*/ }
            | fcb TEXT '\n'                 { do_fcc(((SymbolEntry *) $2)->str); }
            | fdb word_list                 { /*do_fdb();*/ }
            //| FDB ID '\n'                 { add_reference(0, $2, pc); } 
            | rmb NUMBER '\n'               { do_rmb($2); }
            | FILL NUMBER ',' NUMBER '\n'   { do_fill($2, $4); }
            | BSZ NUMBER '\n'               { do_zmb($2); /*bsz=zmb*/ }
            | ZMB NUMBER '\n'               { do_zmb($2); }
            | FCC TEXT '\n'                 { do_fcc(((SymbolEntry*) $2)->str); }
            | END '\n'
            ;

equ_stmt:
            ID ':' EQU expr '\n'{ do_equ($1, get_tkval($4)); }
            | ID EQU expr '\n' { do_equ($1, get_tkval($3)); }
            | EQU expr '\n'  { err_msg("error: line %04d: EQU must define a label", my_linenum-1);}
            ;

nam_stmt:   NAM TEXT '\n' ;

page_stmt:  PAGE '\n' {} ;

opt_stmt:   OPT id_list { } ;

byte_list:
            expr {
                if (get_tktype($1) == INTEGER)
                {
                    if (get_tkval($1) < 0x100)
                    {
                        memory[pc] = (unsigned char) get_tkval($1);
                        pc++;
                    }
                    else
                        err_msg("error: line %04d: byte value overflows.", my_linenum);
                }
                else
                {
                    store_eqn($1, pc, ONEBYTE);
                    pc++;
                }
            } ',' byte_list
            | expr {
                if (get_tktype($1) == INTEGER)
                {
                    if (get_tkval($1) < 0x100)
                    {
                        memory[pc] = (unsigned char) get_tkval($1);
                        pc++;
                    }
                    else
                        err_msg("error: line %04d: byte value overflows.", my_linenum);
                }
                else
                {
                    store_eqn($1, pc, ONEBYTE);
                    pc++;
                }
	    }
            ;

word_list:  expr {
                if (get_tktype($1) == INTEGER)
                {
                    memory[pc] = HI_BYTE(get_tkval($1));
                    pc++;
                    memory[pc] = LO_BYTE(get_tkval($1));
                    pc++;
                }
                else
                {
                    store_eqn($1, pc, EXTENDED);
                    pc += 2;
                }
            }
            | expr {
                if (get_tktype($1) == INTEGER)
                {
                    memory[pc] = HI_BYTE(get_tkval($1));
                    pc++;
                    memory[pc] = LO_BYTE(get_tkval($1));
                    pc++;
                }
                else
                {
                    store_eqn($1, pc, EXTENDED);
                    pc += 2;
                }
            } ',' word_list ;

id_list:
            ID ',' id_list
            | ID '\n'
            ;

label_def:
            ID          { add_label($1, pc); }
            | ID ':'    { add_label($1, pc); }
            | /*e*/
            ;

fdb:        FDB
            | DWM
            ;

fcb:        FCB
            | DFB
            ;

rmb:        RMB
            | DFS
            ;

expr:       expr '+' expr       { $$ = make_eqn('+',$1, $3); }
            | expr '-' expr     { $$ = make_eqn('-',$1, $3); }
            | expr '*' expr     { $$ = make_eqn('*',$1, $3); }
            | expr '/' expr     { $$ = make_eqn('/',$1, $3); }
            | simple_expr
            ;

simple_expr:
            ID          { $$ = make_identifier($1); }
            | NUMBER    { $$ = make_number($1); }
            ;
%%
