%option never-interactive
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
#include <stdarg.h>
#include <stdio.h>

int my_linenum = 1;
int num_errors = 0;
// int num_warnings=0;
int  do_id(char *);
void yyerror(char *msg);
int  str2int(char *str, int base);

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
{identifier}    return do_id(yytext);
{decimal}       { yylval.ival = atoi(yytext); return NUMBER; }
{hex_num_t1}    { yylval.ival = str2int(yytext+2, 16); return NUMBER; }
{hex_num_t2}    { yylval.ival = str2int(yytext+1, 16); return NUMBER; }
{hex_num_t3}    { yytext[strlen(yytext)-1] = 0; yylval.ival = str2int(yytext, 16); return NUMBER; }
{bin_num}       { yylval.ival = str2int(yytext, 2); return NUMBER; }
{octal_num}     { yylval.ival = str2int(yytext, 8); return NUMBER; }
{comment}       { my_linenum++; return '\n'; }
{text}          { yylval.pse = insert_id(yytext); return TEXT; }
{newline}       { my_linenum++; return '\n';}
{delim}         return yytext[0];
{arith}         return yytext[0];
[ \t]+ 
{ignore}        ;
{err_hex}       yylval.ival = 0; yyerror("Bad hexadecimal."); return NUMBER;
{err_num}       yylval.ival = 0; yyerror("Bad decimal."); return NUMBER;
{err_oct}       yylval.ival = 0; yyerror("Bad octal"); return NUMBER;
{err_bin}       yylval.ival = 0; yyerror("Bad binary"); return NUMBER;
%%

void err_msg(const char *fmt, ...)
{
    char buf[512];
    num_errors++;
    va_list ap;
    va_start(ap, fmt);
    fflush(stdout); /* in case stdout and stderr are the same */
    vsprintf(buf, fmt, ap);
    fputs(buf, stderr);
    fputs("\n", stderr);
    fflush(NULL); /* flushes all stdio output streams */
    va_end(ap);
    return;
}

void yyerror(char *msg)
{
    num_errors++;
    printf("error: line %3d: %s %s.\n", my_linenum, msg, yytext);
}

unsigned int hash_func(char *pstr)
{
    int          len = strlen(pstr);
    unsigned int hash = 0;
    memcpy(&hash, pstr, len);
    return hash;
}

int str2int(char *str, int base)
{
    int  rval = 0;
    char c;
    strupr(str);
    char *p = str;
    while (*p)
    {
        c = *p;
        if (c > '9')
            c = (c - 'A') + 10;
        else
            c -= '0';
        if (c >= base)
            yyerror("weird number");
        rval = rval * base + c;
        p++;
    }
    return rval;
}

int bin_search(unsigned int needle, unsigned int *haysack, int min, int max)
{
    int mid = (min + max) / 2;
    if (haysack[max] == needle)
        return max;
    if (max <= min)
        return -1;
    if (haysack[mid] > needle)
        if (mid == max)
            return -1;
        else
            return bin_search(needle, haysack, min, mid - 1);
    if (haysack[mid] < needle)
        if (mid == min)
            return -1;
        else
            return bin_search(needle, haysack, mid + 1, max);
    return mid;
}

int do_id(char *yytext)
{
    int          rval = -1;
    char         buf[256];
    unsigned int hash;
    int          ind;
    strcpy(buf, yytext);
    int len = strlen(buf);
    strupr(buf);
    if (len < 3 || len > 4)
    {
        if (!strcmp(buf, "X"))
            return INDEX;
        // printf("[identifier]");
        goto retid;
    }
    hash = hash_func(buf);
    ind = bin_search(hash, hashList, 0, 106);
    if (ind == -1)
    {
        ind = bin_search(hash, dirHashList, 0, NUM_DIRECTIVES);
        if (ind == -1)
        {
            // printf("[identifier]");
            goto retid;
        }
        return ind + FCB;
    }
    else
    {
        // printf("[inst:%4s]", instList[hashInd[ind]]);
        yylval.ival = hashInd[ind];
        return INST;
    }
retid:
    yylval.pse = insert_id(buf);
    return ID;
}

/*int main( int  argc , char**  argv  ){
    while(1) lexer->yylex();
    return 0;
}*/

