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

#include <cstring>

FILE                       *fout{};
int                         pc{}; // where to write next
std::vector<unsigned short> segments;
std::vector<unsigned short> segment_length;
unsigned short              byte_list[40]{};
int                         num_byte_list{};

Equate       *equations{};
int           num_eqns{};
unsigned char memory[0x10000]; // memory image

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

// utils

unsigned short get_tkval(void *vptk)
{
    return (unsigned short) ((Token *) vptk)->data1;
}

Type get_tktype(void *vptk)
{
    return ((Token *) vptk)->type;
}

int do_arith(char op, int v1, int v2)
{
    switch (op)
    {
    case '+':
        return v1 + v2;
    case '-':
        return v1 - v2;
    case '*':
        return v1 * v2;
    case '/':
        return v1 / v2;
    }
    return -1;
}

unsigned short calc_tk(Token *tk)
{
    if (tk->type == INTEGER)
    {
        return get_tkval((void *) tk);
    }
    if (tk->type == IDENTIFIER)
    {
        SymbolEntry *pse = (SymbolEntry *) tk->data1;
        if (pse->defined)
        {
            return pse->addr;
        }
        else
        {
            err_msg("error: fatal: undeclared identifier \"%s\".", pse->str);
            return 0;
        }
    }
    return (unsigned short) do_arith(tk->opr, calc_tk((Token *) tk->data1), calc_tk((Token *) tk->data2));
}

void store_eqn(void *eqn, unsigned short pc, Mode mode)
{
    equations = (Equate *) realloc(equations, sizeof(Equate) * (num_eqns + 1));
    equations[num_eqns].eqn = (Token *) eqn;
    equations[num_eqns].addr = pc;
    equations[num_eqns].mode = mode;
    num_eqns++;
}

void *make_eqn(char opr, void *vpl, void *vpr)
{
    Token *pt;
    Token *vl = (Token *) vpl;
    Token *vr = (Token *) vpr;
    if (vl->type == INTEGER && vr->type == INTEGER)
    {
        pt = (Token *) make_number(do_arith(opr, reinterpret_cast<int>(vl->data1), reinterpret_cast<int>(vr->data1)));
    }
    else
    {
        pt = (Token *) malloc(sizeof(Token));
        pt->type = OPERATOR;
        pt->data1 = (void *) vpl;
        pt->data2 = (void *) vpr;
        pt->opr = opr;
    }
    return pt;
}

void *make_number(unsigned short sval)
{
    Token *pt = (Token *) malloc(sizeof(Token));
    pt->type = INTEGER;
    pt->data1 = (void *) sval;
    return pt;
}

void *make_identifier(void *pvpse)
{
    Token       *pt;
    SymbolEntry *pse = (SymbolEntry *) pvpse;
    if (!pse->defined)
    {
        pt = (Token *) malloc(sizeof(Token));
        pt->type = IDENTIFIER;
        pt->data1 = pvpse;
    }
    else
    {
        pt = (Token *) make_number(pse->addr);
    }
    return pt;
}

void add_reference(unsigned char opcode, void *vppse, unsigned short addr)
{
    SymbolEntry *pse = (SymbolEntry *) vppse;
    insert_ref(pse, addr, instCodes[opcode].icRel);
}

void add_label(void *vppse, unsigned short addr)
{
    SymbolEntry *pse = (SymbolEntry *) vppse;
    if (insert_def(pse, addr) == -1)
    {
        err_msg("error: line %04d: identifier \"%s\" is already defined", my_linenum, pse->str);
    }
}

// directives

void do_fill(unsigned char val, unsigned short cnt)
{
    while (cnt--)
        memory[pc++] = val;
}

void do_fcb()
{
    int i;
    for (i = 0; i < num_byte_list; i++)
    {
        if (byte_list[i] > 0xff)
            err_msg("error: line %04d: defined value %04X overflows FCB", my_linenum - 1, byte_list[i]);
        memory[pc++] = (unsigned char) byte_list[i];
    }
    num_byte_list = 0;
}

void do_fdb()
{
    int i;
    for (i = 0; i < num_byte_list; i++)
    {
        memory[pc++] = HI_BYTE(byte_list[i]);
        memory[pc++] = LO_BYTE(byte_list[i]);
    }
    num_byte_list = 0;
}

void do_zmb(unsigned short cnt)
{
    while (cnt--)
        memory[pc++] = 0;
}

void do_rmb(unsigned short size)
{
    do_org(pc + size);
}

void do_fcc(char *str)
{
    char *p = str + 1; // skip first "
    while (*p != '"')
    {
        memory[pc] = (unsigned char) (*p);
        pc++;
        p++;
    }
}

void do_org(int addr)
{
    if (addr > 0xffff)
    {
        err_msg("error: line %d: parameter for ORG directive is out of address space.", my_linenum - 1);
        return;
    }
    segments.push_back(addr);
    segment_length.push_back(0);
    if (!segments.empty())
    {
        segment_length[segments.size() - 1] = pc - segments[segments.size() - 1];
    }
    pc = addr;
}

void do_equ(void *vppse, unsigned short addr)
{
    add_label(vppse, addr);
}

// instructions
/*
void do_idinst(unsigned char opcode, SymbolEntry *pse, char imm)
{
    // add_reference(opcode, pse, pc+1);
    if (instCodes[opcode].icRel)
        do_relative(opcode, 0x00);
    else
        do_extended(opcode, 0x0000);
}
*/

void do_inher(unsigned char opcode)
{
    if (instCodes[opcode].icInher)
        memory[pc++] = instCodes[opcode].icInher;
    else
        err_msg("error: line %04d: instruction \"%s\" does not support implied addressing\n", my_linenum - 1,
                instList[opcode]);
}

int get_numbytes(unsigned char opcode)
{
    return (s_instDescTbl[instCodes[opcode].icImmed] & 0x0f);
}

void do_immediate(unsigned char opcode, unsigned short sval)
{
    if (instCodes[opcode].icImmed)
    {
        if (get_numbytes(opcode) == 3)
        {
            memory[pc++] = instCodes[opcode].icImmed;
            memory[pc++] = HI_BYTE(sval);
            memory[pc++] = LO_BYTE(sval);
        }
        else
        {
            memory[pc++] = instCodes[opcode].icImmed;
            memory[pc++] = (unsigned char) sval;
        }
    }
    else
        err_msg("error: line %04d: instruction \"%s\" does not support immediate addressing\n", my_linenum - 1,
                instList[opcode]);
}

void do_direct(unsigned char opcode, unsigned char cval)
{
    if (instCodes[opcode].icDirect)
    {
        memory[pc++] = instCodes[opcode].icDirect;
        memory[pc++] = cval;
    }
    else
        err_msg("error: line %04d: instruction \"%s\" does not support direct addressing\n", my_linenum - 1,
                instList[opcode]);
}

void do_indexed(unsigned char opcode, unsigned char cval)
{
    if (instCodes[opcode].icIndex)
    {
        memory[pc++] = instCodes[opcode].icIndex;
        memory[pc++] = cval;
    }
    else
        err_msg("error: line %04d: instruction \"%s\" does not support indexed addressing\n", my_linenum - 1,
                instList[opcode]);
}

void do_extended(unsigned char opcode, unsigned short sval)
{
    if (instCodes[opcode].icExtend)
    {
        memory[pc++] = instCodes[opcode].icExtend;
        memory[pc++] = (unsigned char) ((sval & 0xff00) >> 8);
        memory[pc++] = (unsigned char) (sval & 0xff);
    }
    else
        err_msg("error: line %04d: instruction \"%s\" does not support extended addressing\n", my_linenum - 1,
                instList[opcode]);
}

void do_relative(unsigned char opcode, unsigned char cval)
{
    if (instCodes[opcode].icRel)
    {
        memory[pc++] = instCodes[opcode].icRel;
        memory[pc++] = cval;
    }
    else
        err_msg("error: line %04d: instruction \"%s\" does not support relative addressing\n", my_linenum - 1,
                instList[opcode]);
}

void resolve_refs()
{
    int i;
    for (i = 0; i < num_eqns; i++)
    {
        unsigned short val = calc_tk(equations[i].eqn);
        switch (equations[i].mode)
        {
        case RELATIVE:
            memory[equations[i].addr] = (unsigned char) (val - equations[i].addr - 1);
            break;
        case EXTENDED:
            memory[equations[i].addr] = HI_BYTE(val);
            memory[equations[i].addr + 1] = LO_BYTE(val);
            break;
        case ONEBYTE:
            memory[equations[i].addr] = (unsigned char) val;
            break;
        }
    }
}

void generate_hex()
{
    int           i;
    unsigned char cksum;
    int           s_left, s_emit, addr;
    char          buf[44];
    resolve_refs();
    for (i = 0; i < segments.size(); i++)
    {
        addr = segments[i];
        s_left = segment_length[i];
        while (s_left)
        {
            s_emit = (s_left > 0x10) ? 0x10 : s_left;
            cksum = s_emit + 3;
            cksum += HI_BYTE(addr) + LO_BYTE(addr);
            s_left -= s_emit;
            sprintf(buf, "S1%s%s%s", hex_conv_tbl[s_emit + 3], hex_conv_tbl[HI_BYTE(addr)],
                    hex_conv_tbl[LO_BYTE(addr)]);
            while (s_emit)
            {
                cksum += memory[addr];
                strcat(buf, hex_conv_tbl[memory[addr]]);
                addr++;
                s_emit--;
            }
            strcat(buf, hex_conv_tbl[(unsigned char) (~cksum)]);
            strcat(buf, "\n");
            fprintf(fout, buf);
        }
    }
    cksum = ~(3 + HI_BYTE(addr) + LO_BYTE(addr));
    fprintf(fout, "S903%s%s%s\n", hex_conv_tbl[HI_BYTE(addr)], hex_conv_tbl[LO_BYTE(addr)], hex_conv_tbl[cksum]);
}

extern "C" int yywrap(void)
{
    return 1;
}

[[noreturn]] void usage(const char *program)
{
    fprintf(stderr,
            "Usage:\n"
            "%s [<input> [<output>]]\n",
            program);
    exit(1);
}

int asm_main(int argc, char **argv)
{
    extern FILE *yyin;
    char         buf[512], *p;
    int          out_is_stdout = 0;
    buf[0] = 0;

    // printf("Hippy m6800 studio assembler\n"
    //        "contact: perreal@hotmail.com\n"
    //        "--------------------------------------\n");
    init_sym_table();

    do_org(0x0000);

    if (argc > 3)
    {
        usage(argv[0]);
    }
    else if (argc >= 2)
    {
        strcpy(buf, argv[1]);
        yyin = fopen(buf, "r");
        if (!yyin)
        {
            err_msg("error: fatal: input file \"%s\" open error.", buf);
            exit(1);
        }
        if (argc == 3)
            strcpy(buf, argv[2]);
        else
        {
            p = strstr(buf, ".");
            if (p)
                *p = 0;
            strcat(buf, ".hex");
        }
        fout = fopen(buf, "w");
        if (!fout)
        {
            err_msg("error: fatal: output file \"%s\" open error.", buf);
            exit(1);
        }
    }
    else
    {
        yyin = stdin;
        fout = stdout;
        out_is_stdout = 1;
    }

    yyparse();

    segment_length[segments.size() - 1] = pc - segments[segments.size() - 1];
    generate_hex();
    printf("assembly complete with %d errors.\n", num_errors);

    clear_sym_table();
    if (!out_is_stdout)
    {
        fclose(yyin);
        fclose(fout);
    }

    return 0;
}
