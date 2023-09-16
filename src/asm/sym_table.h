#ifndef _SYM_TBL__
#define _SYM_TBL__

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define SYMTBL_LENGTH 23

typedef enum {OPERATOR, IDENTIFIER, INTEGER} type_t;
typedef enum {RELATIVE, EXTENDED, ONEBYTE} mode_t;
typedef struct tag_token{
	type_t type;
	int	   opr;
	void * data1;
	void * data2;
} token;

typedef struct tag_equation{
	unsigned short addr;
	token * eqn;
	mode_t mode;
}equation;

typedef struct tag_sym_entry{
	char * str;						//actual string of the identifier
	char hash_val;					//hash val for this str
	unsigned short * refs;			//references to this identifier
	unsigned char *  ref_mode;		//relative or not?
	int	num_refs;					//number of elements in the refs array
	unsigned short addr;			//value of the identifier (generally an address)
	char defined;					//undefined refrence ?
	struct tag_sym_entry * next;	//next item

} sym_entry;

void init_sym_table();
void clear_sym_table();
sym_entry * insert_id(char * str);
int insert_ref(sym_entry * pse, unsigned short mem_adr, char relative);
int insert_def(sym_entry * pse, unsigned short mem_adr);
sym_entry * get_first(int ind);


#endif