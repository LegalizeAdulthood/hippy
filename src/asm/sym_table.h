#ifndef HIPPY_ASM_SYM_TABLE_H
#define HIPPY_ASM_SYM_TABLE_H

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#define SYMTBL_LENGTH 23

enum type_t
{
    OPERATOR,
    IDENTIFIER,
    INTEGER
};

enum mode_t
{
    RELATIVE,
    EXTENDED,
    ONEBYTE
};

struct token
{
    type_t type;
    int    opr;
    void  *data1;
    void  *data2;
};

struct equation
{
    unsigned short addr;
    token         *eqn;
    mode_t         mode;
};

struct sym_entry
{
    char           *str;      // actual string of the identifier
    char            hash_val; // hash val for this str
    unsigned short *refs;     // references to this identifier
    unsigned char  *ref_mode; // relative or not?
    int             num_refs; // number of elements in the refs array
    unsigned short  addr;     // value of the identifier (generally an address)
    char            defined;  // undefined refrence ?
    sym_entry      *next;     // next item
};

void       init_sym_table();
void       clear_sym_table();
sym_entry *insert_id(char *str);
int        insert_ref(sym_entry *pse, unsigned short mem_adr, char relative);
int        insert_def(sym_entry *pse, unsigned short mem_adr);
sym_entry *get_first(int ind);

#endif
