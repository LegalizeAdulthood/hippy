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
#include "tables.h"

/*
 * STRING TABLE
 */

CStringTable::CStringTable()
{
    memset(listSize, 0, sizeof(int) * 256);
    memset(tree, 0, sizeof(tree));
}

CStringTable::~CStringTable()
{
    for (int i = 0; i < 256; i++)
        for (int k = 0; k < listSize[i]; k++)
            free(tree[i][k]);
}

LPSTR CStringTable::AddString(BYTE hash, LPCSTR lpcStr)
{
    bool found = false;
    ;
    LPSTR retval;

    retval = Find(hash, lpcStr);

    if (!retval)
    {
        tree[hash] = (char **) realloc(tree[hash], sizeof(LPSTR) * (listSize[hash] + 1));
        tree[hash][listSize[hash]] = (char *) malloc(strlen(lpcStr) + 1);
        strcpy(tree[hash][listSize[hash]], lpcStr);
        retval = tree[hash][listSize[hash]];
        listSize[hash]++;
    }
    return retval;
}

LPSTR CStringTable::Find(BYTE hash, LPCSTR lpcStr)
{
    LPSTR retval = NULL;
    for (int i = 0; i < listSize[hash]; i++)
        if (!strcmp(tree[hash][i], lpcStr))
        {
            retval = tree[hash][i];
            break;
        }
    return retval;
}

/*
 * SymbolTable
 */

CSymbolTable::CSymbolTable()
{
    memset(listSize, 0, sizeof(int) * 256);
    memset(tree, 0, sizeof(tree));
}

CSymbolTable::~CSymbolTable()
{
    for (int i = 0; i < 256; i++)
        for (int k = 0; k < listSize[i]; k++)
            free(tree[i][k]);
}

bool CSymbolTable::Insert(BYTE hash, PSymbol pSym)
{
    bool found = false;
    ;
    bool retval;

    retval = Find(hash, pSym);

    if (!retval)
    {
        tree[hash] = (PSymbol *) realloc(tree[hash], sizeof(LPSTR) * (listSize[hash] + 1));
        tree[hash][listSize[hash]] = (PSymbol) malloc(sizeof(Symbol));
        memcpy(tree[hash][listSize[hash]], pSym, sizeof(Symbol));
        listSize[hash]++;
        retval = true;
    }
    return retval;
}

bool CSymbolTable::Find(BYTE hash, PSymbol pSym)
{
    bool retval = false;
    for (int i = 0; i < listSize[hash]; i++)
        if (!strcmp(tree[hash][i]->name, pSym->name))
        {
            pSym->addr = tree[hash][i]->addr;
            pSym->type = tree[hash][i]->type;
            retval = true;
            break;
        }
    return retval;
}