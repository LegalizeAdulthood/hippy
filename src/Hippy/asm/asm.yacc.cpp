
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

#include <stdio.h>
#include "lexer.h"
#include "codes.h"
#include "sym_table.h"

#define HI_BYTE(y) (unsigned char)(y>>8)
#define LO_BYTE(y) (unsigned char)(y&0xff)

# pragma warning (disable: 4065) /* Switch statement contains default but no case. */
int yylex();
extern int num_errors;
extern int my_linenum;
extern void err_msg(const char *fmt, ...);
extern void yyerror(char*);

FILE * fout=NULL;
int pc=0;//where to write next
unsigned short *segments=NULL;
unsigned short *segment_length=NULL;
unsigned short byte_list[40];
int num_byte_list=0;
int num_segments=0;

equation* equations;
int num_eqns=0;
unsigned char memory[0x10000]; //memory image

void store_eqn(void *eqn, unsigned short pc,  mode_t mode);
int get_numbytes(unsigned char opcode);
unsigned short get_tkval(void *vptk);
type_t get_tktype(void *vptk);
void * make_number(unsigned short sval);
void * make_identifier(void *pvpse);
void * make_eqn(char opr, void * vpl, void * vpr);
void add_label(void *vppse, unsigned short addr);
void add_reference(unsigned char opcode, void *vppse, unsigned short addr);
void do_fill(unsigned char val, unsigned short cnt);
void do_fcb();
void do_fdb();
void do_fcc(char * str);
void do_zmb(unsigned short cnt);
void do_rmb(unsigned short size);
void do_org(int addr);
void do_equ(void * vppse, unsigned short addr);
void do_idinst(unsigned char opcode, sym_entry *pse,  char imm);
void do_inher(unsigned char opcode);
void do_immediate(unsigned char opcode, unsigned short sval);
void do_direct(unsigned char opcode, unsigned char cval);
void do_indexed(unsigned char opcode, unsigned char cval);
void do_extended(unsigned char opcode, unsigned short sval);
void do_relative(unsigned char opcode, unsigned char cval);
void generate_hex();
%}

%union{	int ival; void* pse;}
%token INDEX FCB FDB DFB RMB ZMB FCC END ORG DFL DWL NAM DWM DFS OPT EQU BSZ PAGE FILL
%token <ival> INST NUMBER 
%token <pse> ID TEXT
%type <pse> simple_expr expr
%left '+' '-'
%left '*' '/'
%%

program:		  asm_stmts
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
				| label_def NUMBER '\n'		{ err_msg("error: line %04d: instruction or directive expected, found \"%d\"", my_linenum-1,$2);}
				| ID ':' ID '\n'			{ err_msg("error: line %04d: instruction or directive expected, found \"%s\"", my_linenum-1,((sym_entry*)$3)->str);}
				| ID ID '\n'				{ err_msg("error: line %04d: instruction or directive expected, found \"%s\"", my_linenum-1,((sym_entry*)$2)->str);}
				| ID ID NUMBER '\n'			{ err_msg("error: line %04d: instruction or directive expected, found \"%s\"", my_linenum-1,((sym_entry*)$2)->str);}
				| ID ':' ID NUMBER '\n'		{ err_msg("error: line %04d: instruction or directive expected, found \"%s\"", my_linenum-1,((sym_entry*)$3)->str);}
				;
pure_asm_stmt:	 INST '\n'						{ do_inher($1); }
				| INST '#' expr '\n' {
					unsigned char opcode=$1;
					if( get_tktype($3)==INTEGER ){
						do_immediate(opcode, get_tkval($3));
					}
					else{
						if(get_numbytes(opcode)==3){
							store_eqn($3, pc+1, EXTENDED); 
							do_immediate(opcode, 0x0000);
						}
						else{
							store_eqn($3, pc+1, ONEBYTE);
							do_immediate(opcode, 0x00);
						}
					}
				}
				| INST expr	'\n' {
					unsigned short sval=get_tkval($2);
					unsigned char opcode=$1;
					if( get_tktype($2)==INTEGER ){
						if(instCodes[opcode].icRel){
							do_relative(opcode, (unsigned char)(sval-pc-2));
						}
						else if(sval<0x100 && instCodes[opcode].icDirect)
							do_direct(opcode, (unsigned char)sval);
						else
							do_extended(opcode, sval);
					}
					else{
						if(instCodes[opcode].icRel){
							store_eqn($2,pc+1, RELATIVE);
							do_relative(opcode, 0x00);
						}
						else{
							store_eqn($2, pc+1, EXTENDED);
							do_extended(opcode, 0x0000);
						}
					}
				}
				| INST NUMBER ',' INDEX '\n'	{ do_indexed($1, $2); }
				;
dir_stmt:
				  ORG NUMBER '\n'{ do_org($2); }
				| fcb byte_list	{/*do_fcb();*/}
				| fcb TEXT '\n' { do_fcc( ((sym_entry*)$2)->str );}
				| fdb word_list {/*do_fdb();*/}
				//| FDB ID	'\n' { add_reference(0, $2, pc); } 
				| rmb NUMBER '\n'	{ do_rmb($2); }
				| FILL NUMBER ',' NUMBER '\n' {do_fill($2, $4);}
				| BSZ NUMBER '\n' { do_zmb($2); /*bsz=zmb*/ }
				| ZMB NUMBER '\n' { do_zmb($2);}
				| FCC TEXT '\n'	  { do_fcc( ((sym_entry*)$2)->str ); }
				| END '\n'
				;
equ_stmt:
				  ID ':' EQU expr '\n'{ do_equ($1, get_tkval($4)); }
				| ID EQU expr '\n' { do_equ($1, get_tkval($3)); }
				| EQU expr '\n'  {	err_msg("error: line %04d: EQU must define a label", my_linenum-1);}
				;
nam_stmt:
				  NAM TEXT '\n'
				;
page_stmt:
				  PAGE '\n'	{ }	
				;
opt_stmt:		  
				  OPT id_list { }
				;
byte_list:
				expr {
					if(get_tktype($1)==INTEGER){
						if(get_tkval($1)<0x100){
							memory[pc]=(unsigned char)get_tkval($1);
							pc++;
						}
						else err_msg("error: line %04d: byte value overflows.", my_linenum);
					}
					else{
						store_eqn($1, pc, ONEBYTE);
						pc++;
					}
				} ',' byte_list

				| expr {
					if(get_tktype($1)==INTEGER){
						if(get_tkval($1)<0x100){
							memory[pc]=(unsigned char)get_tkval($1);
							pc++;
						}
						else err_msg("error: line %04d: byte value overflows.", my_linenum);
					}
					else{
						store_eqn($1, pc, ONEBYTE);
						pc++;
					}
				}
				;
word_list:		expr 
				{
					if(get_tktype($1)==INTEGER){
						memory[pc]=HI_BYTE(get_tkval($1));
						pc++;
						memory[pc]=LO_BYTE(get_tkval($1));
						pc++;
					}
					else{
						store_eqn($1, pc, EXTENDED);
						pc+=2;
					}
				}
				| expr
				{
					if(get_tktype($1)==INTEGER){
						memory[pc]=HI_BYTE(get_tkval($1));
						pc++;
						memory[pc]=LO_BYTE(get_tkval($1));
						pc++;
					}
					else{
						store_eqn($1, pc, EXTENDED);
						pc+=2;
					}
				} ',' word_list
id_list:
				  ID ',' id_list
				| ID '\n'
				;

label_def:
				  ID		{ add_label($1, pc); }
				| ID ':'	{ add_label($1, pc); }
				| /*e*/
				;

fdb:			
				  FDB
				| DWM
				;

fcb:			  FCB
				| DFB
				;

rmb:			  RMB
				| DFS
				;

expr:			  expr '+' expr	{ $$ = make_eqn('+',$1, $3); }
				| expr '-' expr	{ $$ = make_eqn('-',$1, $3); }
				| expr '*' expr	{ $$ = make_eqn('*',$1, $3); }
				| expr '/' expr	{ $$ = make_eqn('/',$1, $3); }
				| simple_expr
				;
simple_expr:	
				  ID	 { $$ = make_identifier($1); }
				| NUMBER { $$ = make_number($1); }
				;
%%

// utils

unsigned short get_tkval(void *vptk){
	return (unsigned short)((token*)vptk)->data1;
}

type_t get_tktype(void *vptk){
	return ((token*)vptk)->type;
}

int do_arith(char op, int v1, int v2){
	switch(op){
		case '+': return v1 + v2;
		case '-': return v1 - v2;
		case '*': return v1 * v2;
		case '/': return v1 / v2;
	}
	return -1;
}

unsigned short calc_tk(token *tk){
	if(tk->type==INTEGER){
		return get_tkval((void*)tk);
	}
	if(tk->type==IDENTIFIER){
		sym_entry* pse=(sym_entry*)tk->data1;
		if(pse->defined){
			return pse->addr;
		}
		else{
			err_msg("error: fatal: undeclared identifier \"%s\".", pse->str);
			return 0;
		}
	}
	return (unsigned short)do_arith(tk->opr, calc_tk((token*)tk->data1), calc_tk((token*)tk->data2));
}

void store_eqn(void *eqn, unsigned short pc, mode_t mode){
	equations=(equation*)realloc(equations, sizeof(equation)*(num_eqns+1));
	equations[num_eqns].eqn=(token*)eqn;
	equations[num_eqns].addr=pc;
	equations[num_eqns].mode=mode;
	num_eqns++;
}

void * make_eqn(char opr, void * vpl, void * vpr){
	token* pt;
	token *vl=(token*)vpl;
	token *vr=(token*)vpr;
	if(vl->type == INTEGER && vr->type==INTEGER){
		pt=(token*)make_number(do_arith(opr, (int)vl->data1, (int)vr->data1));
	}
	else{
		pt=(token*)malloc(sizeof(token));
		pt->type=OPERATOR;
		pt->data1=(void*)vpl;
		pt->data2=(void*)vpr;
		pt->opr=opr;
	}
	return pt;
}

void * make_number(unsigned short sval){
	token* pt=(token*)malloc(sizeof(token));
	pt->type=INTEGER;
	pt->data1=(void*)sval;
	return pt;
}

void * make_identifier(void *pvpse){
	token* pt;
	sym_entry* pse=(sym_entry*)pvpse;
	if(!pse->defined){
		pt=(token*)malloc(sizeof(token));
		pt->type=IDENTIFIER;
		pt->data1=pvpse;
	}
	else{
		pt=(token*)make_number(pse->addr);
	}
	return pt;
}

void add_reference(unsigned char opcode, void *vppse, unsigned short addr){
	sym_entry * pse = (sym_entry*)vppse;
	insert_ref(pse, addr, instCodes[opcode].icRel);
}

void add_label(void *vppse, unsigned short addr){
	sym_entry * pse = (sym_entry*)vppse;
	if(insert_def(pse, addr)==-1){
		err_msg("error: line %04d: identifier \"%s\" is already defined", my_linenum, pse->str);
	}
}

// directives

void do_fill(unsigned char val, unsigned short cnt){
	while(cnt--)
		memory[pc++]=val;
}

void do_fcb(){
	int i;
	for(i=0;i<num_byte_list;i++){
		if(byte_list[i]>0xff)
			err_msg("error: line %04d: defined value %04X overflows FCB", my_linenum-1, byte_list[i]);
		memory[pc++]=(unsigned char)byte_list[i];
	}
	num_byte_list=0;
}

void do_fdb(){
	int i;
	for(i=0;i<num_byte_list;i++){
		memory[pc++]=HI_BYTE(byte_list[i]);
		memory[pc++]=LO_BYTE(byte_list[i]);
	}
	num_byte_list=0;
}

void do_zmb(unsigned short cnt){
	while(cnt--)
		memory[pc++]=0;
}

void do_rmb(unsigned short size){
	do_org(pc+size);
}

void do_fcc(char * str){
	char *p=str+1; //skip first "
	while(*p!='"'){
		memory[pc] = (unsigned char)(*p);
		pc++;
		p++;
	}
}

void do_org(int addr){
	if(addr>0xffff){
		err_msg("error: line %d: parameter for ORG directive is out of address space.", my_linenum-1);
		return;
	}
	segments=(unsigned short*)realloc(segments, sizeof(unsigned short)*(num_segments+1));
	segments[num_segments]=addr;
	segment_length=(unsigned short*)realloc(segment_length, sizeof(unsigned short)*(num_segments+1));
	segment_length[num_segments]=0;
	if(num_segments){
		segment_length[num_segments-1]=pc-segments[num_segments-1];
	}
	num_segments++;
	pc=addr;
	
}

void do_equ(void * vppse, unsigned short addr){
	add_label(vppse, addr);
}

// instructions
/*
void do_idinst(unsigned char opcode, sym_entry *pse, char imm){
//	add_reference(opcode, pse, pc+1); 
	if(instCodes[opcode].icRel) 
		do_relative(opcode, 0x00); 
	else 
		do_extended(opcode, 0x0000);

}
*/
void do_inher(unsigned char opcode){
	if(instCodes[opcode].icInher)
	   memory[pc++]=instCodes[opcode].icInher;
	else
		err_msg("error: line %04d: instruction \"%s\" does not support implied addressing\n", 
		my_linenum-1, instList[opcode]);		
}

int get_numbytes(unsigned char opcode){
	return (InstDescTbl[instCodes[opcode].icImmed]&0x0f);
}

void do_immediate(unsigned char opcode, unsigned short sval){
	if(instCodes[opcode].icImmed){
		if(get_numbytes(opcode)==3){
			memory[pc++]=instCodes[opcode].icImmed;
			memory[pc++]=HI_BYTE(sval);
			memory[pc++]=LO_BYTE(sval);
		}
		else{
			memory[pc++]=instCodes[opcode].icImmed;
			memory[pc++]=(unsigned char)sval;
		}
	   }
	else		
		err_msg("error: line %04d: instruction \"%s\" does not support immediate addressing\n", 
		my_linenum-1, instList[opcode]);		

}

void do_direct(unsigned char opcode, unsigned char cval){
	if(instCodes[opcode].icDirect){
		memory[pc++]=instCodes[opcode].icDirect;
		memory[pc++]=cval;
	   }
	else		
		err_msg("error: line %04d: instruction \"%s\" does not support direct addressing\n", 
		my_linenum-1, instList[opcode]);		

}

void do_indexed(unsigned char opcode, unsigned char cval){
	if(instCodes[opcode].icIndex){
		memory[pc++]=instCodes[opcode].icIndex;
		memory[pc++]=cval;
	   }
	else		
		err_msg("error: line %04d: instruction \"%s\" does not support indexed addressing\n", 
		my_linenum-1, instList[opcode]);		

}

void do_extended(unsigned char opcode, unsigned short sval){
	if(instCodes[opcode].icExtend){
		memory[pc++]=instCodes[opcode].icExtend;
		memory[pc++]=(unsigned char)((sval&0xff00)>>8);
		memory[pc++]=(unsigned char)(sval&0xff);
	   }
	else		
		err_msg("error: line %04d: instruction \"%s\" does not support extended addressing\n", 
		my_linenum-1, instList[opcode]);		

}

void do_relative(unsigned char opcode, unsigned char cval){
	if(instCodes[opcode].icRel){
		memory[pc++]=instCodes[opcode].icRel;
		memory[pc++]=cval;
	}
	else
		err_msg("error: line %04d: instruction \"%s\" does not support relative addressing\n", 
		my_linenum-1, instList[opcode]);		

}

void resolve_refs(){
	int i;
	for(i=0;i<num_eqns;i++){
		unsigned short val=calc_tk(equations[i].eqn);
		switch(equations[i].mode){
		case RELATIVE:
				memory[equations[i].addr  ]=(unsigned char)(val-equations[i].addr-1);
			break;
		case EXTENDED:
				memory[equations[i].addr  ]=HI_BYTE(val);
				memory[equations[i].addr+1]=LO_BYTE(val);
			break;
		case ONEBYTE:
				memory[equations[i].addr  ]=(unsigned char)val;
			break;
		}
	}
}

void generate_hex(){
	int i;
	unsigned char cksum;
	int s_left, s_emit, addr;
	char buf[44];
	resolve_refs();
	for(i=0; i<num_segments; i++){
		addr=segments[i];
		s_left=segment_length[i];
		while(s_left){
			s_emit=(s_left>0x10)?0x10:s_left;
			cksum=s_emit+3;
			cksum+=HI_BYTE(addr)+LO_BYTE(addr);
			s_left-=s_emit;
			sprintf(buf, "S1%s%s%s", 
				hex_conv_tbl[s_emit+3], 
				hex_conv_tbl[HI_BYTE(addr)], 
				hex_conv_tbl[LO_BYTE(addr)]
			);
			while(s_emit){
				cksum+=memory[addr];
				strcat(buf, hex_conv_tbl[memory[addr]]);
				addr++;
				s_emit--;
			}
			strcat(buf, hex_conv_tbl[(unsigned char) (~cksum)]);
			strcat(buf, "\n");
			fprintf(fout, buf);
		}

	}
	cksum=~(3+HI_BYTE(addr)+LO_BYTE(addr));
	fprintf(fout, "S903%s%s%s\n", 
		hex_conv_tbl[HI_BYTE(addr)], 
		hex_conv_tbl[LO_BYTE(addr)], 
		hex_conv_tbl[cksum]
		);
}

extern "C"{
int yywrap(void){
	return 1;
}
}

void main(int argc,char **argv){
   extern FILE * yyin;
   char buf[512], *p;
   int out_is_stdout=0;
   buf[0]=0;
   --argc; ++argv;
	
   /*printf(
			"Hippy m6800 studio assembler\n"
			"contact: perreal@hotmail.com\n"
			"--------------------------------------\n"
	);*/
   init_sym_table();

   do_org(0x0000);

   if(argc>0)
   {
	   while(argc--){
		   strcat(buf, *argv);
		   strcat(buf, " ");
		   argv++;
	   }
	   yyin = fopen(buf,"r");
	   if(!yyin){
		   err_msg("error: fatal: input file \"%s\" open error.", buf);
		   exit(1);
	   }
	   p=strstr(buf, ".");
	   if(p) *p=0;
	   strcat(buf, ".hex");
       fout=fopen(buf, "w");
	   if(!fout){
		   err_msg("error: fatal: output file \"%s\" open error.", buf);
		   exit(1);
	   }
   }
   else
   {
	   yyin = stdin; 
	   fout=stdout;
	   out_is_stdout=1;
   }

   yyparse();
	
   segment_length[num_segments-1]=pc-segments[num_segments-1];
   generate_hex();
   printf("assembly complete with %d errors.\n", num_errors);

   clear_sym_table();
   if(!out_is_stdout){
	   fclose(yyin);
	   fclose(fout);
   }
   getchar();
}
