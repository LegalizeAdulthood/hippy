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

extern FILE *yyin;
extern char *yytext;

// clang-format off
const char *const dirList[NUM_DIRECTIVES] =
{
    "FCB", "FDB", "DFB", "RMB",
    "ZMB", "FCC", "END", "ORG",
    "DFL", "DWL", "NAM", "DWM",
    "DFS", "OPT", "EQU", "BSZ",
    "PAGE", "FILL"
};

const unsigned int dirHashList[NUM_DIRECTIVES] =
{
    4342598, 4342854, 4343364, 4345170,
    4345178, 4408134, 4476485, 4674127,
    4998724, 5003076, 5062990, 5068612,
    5457476, 5525583, 5591365, 5919554,
    1162297680, 1280067910
};

/*
 * instruction listing
 */
const char *const instList[NUM_INST + 1] =
{
    "ASL",  "BMI",  "ASR",  "BLS",
    "BNE",  "SEC",  "BLT",  "DAA",
    "SEI",  "EORA", "EORB", "NEG",
    "SEV",  "TAB",  "BPL",  "BRA",
    "ANDA", "ANDB", "LSR",  "TBA",
    "ROL",  "CLC",  "DEC",  "TAP",
    "ROR",  "CLI",  "ADCA", "ADCB",
    "LSRA", "LSRB", "ADDA", "ADDB",
    "BSR",  "CLR",  "DES",  "CLV",
    "BVC",  "DEX",  "COMA", "COMB",
    "RTI",  "COM",  "PSHA", "PSHB",
    "ASLA", "BVS",  "ASLB", "RTS",
    "STAA", "ROLA", "STAB", "ROLB",
    "CPX",  "INC",  "DECA", "NOP",
    "DECB", "INS",  "INX",  "ASRA",
    "ASRB", "RORA", "RORB", "ABA",
    "STS",  "TPA",  "STX",  "SUBA",
    "SUBB", "SWI",  "ORAA", "ORAB",
    "JMP",  "TST",  "TSX",  "WAI",
    "BCC",  "LDS",  "LDX",  "JSR",
    "BCS",  "PULA", "PULB", "TXS",
    "CLRA", "CLRB", "INCA", "INCB",
    "BEQ",  "BGE",  "LDAA", "LDAB",
    "CBA",  "BHI",  "BGT",  "NEGA",
    "NEGB", "SBCA", "SBCB", "SBA",
    "CMPA", "BITA", "CMPB", "BITB",
    "BLE",  "TSTA", "TSTB", 0
};

/*
 * unsigned int hash_func(char * pstr){
 *	int len=strlen(pstr);
 *	unsigned int hash=0;
 *
 *	memcpy(&hash, pstr, len);
 *	return hash;
 *}
 *
 */

const unsigned int hashList[NUM_INST] =
{
    4276548,    4276801,    4276803,    4276819,    4276820,    4280404,    4280898,    4342100,    4408130,
    4408644,    4408659,    4410435,    4410953,    4412994,    4540226,    4541506,    4542018,    4670798,
    4800855,    4801875,    4802626,    4803651,    4803906,    4805714,    4806483,    5001042,    5001282,
    5002049,    5066563,    5259604,    5262666,    5263182,    5326146,    5393475,    5394258,    5395265,
    5395266,    5395274,    5395276,    5456706,    5456972,    5457220,    5459010,    5459529,    5461074,
    5461075,    5461570,    5462100,    5523266,    5524546,    5526356,    5653843,    5655619,    5784652,
    5784900,    5787209,    5787715,    5788500,    5788755,    1094796364, 1094799951, 1094800467, 1094866259,
    1094926931, 1094927425, 1094927684, 1094929993, 1094992961, 1094995521, 1095189838, 1095258960, 1095520082,
    1095521089, 1095521616, 1095585603, 1095781699, 1095912515, 1095913285, 1095913298, 1095914305, 1095914316,
    1096042818, 1096045396, 1111573580, 1111577167, 1111577683, 1111643475, 1111704147, 1111704641, 1111704900,
    1111707209, 1111770177, 1111772737, 1111967054, 1112036176, 1112297298, 1112298305, 1112298832, 1112362819,
    1112558915, 1112689731, 1112690501, 1112690514, 1112691521, 1112691532, 1112820034, 1112822612
};

const unsigned int hashInd[NUM_INST] =
{
    7,   63,  92, 99, 19, 65, 15, 13, 76, 22, 5,  21, 53, 36, 89, 104, 4,  11, 75,  8,  93, 25,  1,  40, 69,  20, 14,
    0,   41,  23, 72, 55, 88, 33, 24, 2,  32, 79, 18, 80, 77, 34, 3,   57, 47, 64,  45, 83, 94,  6,  73, 12,  35, 78,
    37,  58,  52, 74, 66, 90, 70, 48, 67, 97, 26, 54, 86, 30, 16, 95,  42, 49, 44,  81, 38, 100, 84, 9,  61,  59, 28,
    101, 105, 91, 71, 50, 68, 98, 27, 56, 87, 31, 17, 96, 43, 51, 46,  82, 39, 102, 85, 10, 62,  60, 29, 103, 106
};
// clang-format on

int my_linenum = 1;
int num_errors = 0;
int  do_id(char *);
void yyerror(char *msg);
int  str2int(char *str, int base);
int  isatty(int fileno);


int isatty(int fileno)
{
    return (yyin == stdin) ? 1 : 0;
}

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

int bin_search(unsigned int needle, const unsigned *haysack, int min, int max)
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

#if 0
int main(int argc, char **argv)
{
    while (1)
        lexer->yylex();
    return 0;
}
#endif
