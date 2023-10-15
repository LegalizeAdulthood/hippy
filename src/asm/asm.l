%option nounistd
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

#include "lexer.h"
#include "sym_table.h"
#include "y.tab.hpp"

static int isatty(int fileno)
{
    return (yyin == stdin) ? 1 : 0;
}

%}

identifier      ("_"|[a-z])([0-9]|[a-z]|"_")*
decimal         [0-9]+
err_num         [0-9]+{identifier}
hex_digit       [0-9]|[a-f]
hex_num_t1      "0x"{hex_digit}+
hex_num_t2      "$"{hex_digit}+
hex_num_t3      [0-9]+{hex_digit}*"h"
err_hex         ("0x"|"$"){identifier}
bin_num         "%"(0|1)+"%"
err_bin         "%"([0-9]|[a-z])+
octal_num       "@"[0-7]+
err_oct         "@"({decimal}|{identifier})
text            \"(.)*\"
comment         ";"(.)*\n
newline         \n
delim           "#"|","|":"
arith           "*"|"+"|"-"|"/"
ignore          "."

%%
{identifier}    { return do_id(yytext); }
{decimal}       { yylval.ival = atoi(yytext); return NUMBER; }
{hex_num_t1}    { yylval.ival = str2int(yytext+2, 16); return NUMBER; }
{hex_num_t2}    { yylval.ival = str2int(yytext+1, 16); return NUMBER; }
{hex_num_t3}    { yytext[strlen(yytext)-1] = 0; yylval.ival = str2int(yytext, 16); return NUMBER; }
{bin_num}       { yylval.ival = str2int(yytext, 2); return NUMBER; }
{octal_num}     { yylval.ival = str2int(yytext, 8); return NUMBER; }
{comment}       { my_linenum++; return '\n'; }
{text}          { yylval.pse = insert_id(yytext); return TEXT; }
{newline}       { my_linenum++; return '\n';}
{delim}         { return yytext[0]; }
{arith}         { return yytext[0]; }
[ \t]+ 
{ignore}        ;
{err_hex}       { yylval.ival = 0; yyerror("Bad hexadecimal."); return NUMBER; }
{err_num}       { yylval.ival = 0; yyerror("Bad decimal."); return NUMBER; }
{err_oct}       { yylval.ival = 0; yyerror("Bad octal"); return NUMBER; }
{err_bin}       { yylval.ival = 0; yyerror("Bad binary"); return NUMBER; }
%%
