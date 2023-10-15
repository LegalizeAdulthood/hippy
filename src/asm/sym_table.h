#ifndef HIPPY_ASM_SYM_TABLE_H
#define HIPPY_ASM_SYM_TABLE_H

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define SYMTBL_LENGTH 23

enum Type
{
    OPERATOR,
    IDENTIFIER,
    INTEGER
};

enum Mode
{
    RELATIVE,
    EXTENDED,
    ONEBYTE
};

struct Token
{
    Type  type;
    int   opr;
    void *data1;
    void *data2;
};

struct Equate
{
    unsigned short addr;
    Token         *eqn;
    Mode           mode;
};

struct SymbolEntry
{
    char           *str;      // actual string of the identifier
    char            hash_val; // hash val for this str
    unsigned short *refs;     // references to this identifier
    unsigned char  *ref_mode; // relative or not?
    int             num_refs; // number of elements in the refs array
    unsigned short  addr;     // value of the identifier (generally an address)
    char            defined;  // undefined refrence ?
    SymbolEntry    *next;     // next item
};

void         init_sym_table();
void         clear_sym_table();
SymbolEntry *insert_id(char *str);
int          insert_ref(SymbolEntry *pse, unsigned short mem_adr, char relative);
int          insert_def(SymbolEntry *pse, unsigned short mem_adr);
SymbolEntry *get_first(int ind);

#endif
