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

#include "sym_table.h"

static SymbolEntry sym_table[SYMTBL_LENGTH];

unsigned char hash_str(char *str)
{
    unsigned char hash = 251;
    while (*str)
    {
        hash = ((hash << 5) + hash) + *str;
        str++;
    }
    return (unsigned char) (hash % SYMTBL_LENGTH);
}

void init_sym_table()
{
    memset(sym_table, 0, sizeof(SymbolEntry) * SYMTBL_LENGTH);
}

void clear_sym_table()
{
    int          i;
    SymbolEntry *pse, *pse_tmp;
    for (i = 0; i < SYMTBL_LENGTH; i++)
    {
        pse = sym_table[i].next;
        while (pse)
        {
            pse_tmp = pse->next;
            if (pse->refs)
                free(pse->refs);
            if (pse->str)
                free(pse->str);
            if (pse->ref_mode)
                free(pse->ref_mode);
            free(pse);
            pse = pse_tmp;
        }
    }
}

SymbolEntry *insert_id(char *str)
{
    int          hash_val = hash_str(str);
    SymbolEntry *pse = &sym_table[hash_val];
    while (pse->next)
    {
        if (pse->next->hash_val == hash_val && !strcmp(str, pse->next->str))
        {
            return pse->next;
        }
        pse = pse->next;
    }
    // we need to insert this string
    pse->next = (SymbolEntry *) malloc(sizeof(SymbolEntry));
    pse = pse->next;
    memset(pse, 0, sizeof(SymbolEntry));
    pse->hash_val = hash_val;
    pse->str = (char *) malloc(strlen(str) + 1);
    memcpy(pse->str, str, strlen(str) + 1);
    return pse;
}

int insert_ref(SymbolEntry *pse, unsigned short mem_adr, char relative)
{
    pse->num_refs++;
    pse->refs = (unsigned short *) realloc(pse->refs, sizeof(unsigned short) * pse->num_refs);
    pse->refs[pse->num_refs - 1] = mem_adr;
    pse->ref_mode = (unsigned char *) realloc(pse->ref_mode, sizeof(unsigned char) * pse->num_refs);
    pse->ref_mode[pse->num_refs - 1] = relative;
    return 0;
}

int insert_def(SymbolEntry *pse, unsigned short mem_adr)
{
    if (pse->defined)
        return -1; // identifier re-declared
    pse->defined = 1;
    pse->addr = mem_adr;
    return 0;
}

SymbolEntry *get_first(int ind)
{
    return sym_table[ind].next;
}
