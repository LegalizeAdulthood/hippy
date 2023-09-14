%{
#include <stdio.h>
#include "lexer.h"

FlexLexer* lexer;
int my_linenum=1;
int do_id();
void error(char *msg);

%}

identifier		("_"|[a-z])([0-9]|[a-z]|"_")*
decimal			[0-9]+
err_num			[0-9]+{identifier}
hex_digit		[0-9]|[a-f]
hex_num_t1		"0x"{hex_digit}+
hex_num_t2		"$"{hex_digit}+
hex_num_t3		[0-9]+{hex_digit}*"h"
hex_num			{hex_num_t1}|{hex_num_t2}|{hex_num_t3}
err_hex			("0x"|"$"){identifier}
bin_num			"%"(0|1)+
err_bin			"%"([0-9]|[a-z])+
octal_num		"@"[0-7]+
err_oct			"@"({decimal}|{identifier})
text			\"(.)*\"
comment			";"(.)*\n
newline			\n
delim			"#"|","|":"
arith			"*"|"+"|"-"

%%
{identifier}	do_id();
{decimal}		printf("[decimal]");
{hex_num}		printf("[hexnum]");
{bin_num}		printf("[binary]");
{octal_num}		printf("[octal]");
{comment}		printf("[comment]");
{text}			printf("[text]");
{newline}		my_linenum++;
{delim}			printf("[delim:%c]", lexer->YYText()[0]);
{arith}			printf("[arith:%c]", lexer->YYText()[0]);
[ \t]+ 
{err_hex}		error("{e:hex}");
{err_num}		error("{e:number}");
{err_oct}		error("{e:octal}");
{err_bin}		error("{e:bin}");
%%

void error(char *msg){
	printf("error: line %3d: %s.\n", my_linenum, msg);
}

unsigned int hash_func(char * pstr){
	int len=strlen(pstr);
	unsigned int hash=0;

	memcpy(&hash, pstr, len);
	return hash;
}

int bin_search(unsigned int needle, unsigned int *haysack, int min, int max){
	int mid=(min+max)/2;
	if(haysack[max]==needle) return max;
	if(max<=min) return -1;
	if(haysack[mid]>needle)
		if(mid==max) return -1;
		else return bin_search(needle, haysack, min, mid-1);
	if(haysack[mid]<needle)
		if(mid==min) return -1;
		else return bin_search(needle, haysack, mid+1, max);
	return mid;
}

int do_id(){
	int rval=-1;
	char buf[256];
	strcpy(buf, lexer->YYText());
	int len=strlen(buf);
	strupr(buf);
	if(len<3||len>4){
		printf("[identifier]");
		return -1;
	}
	unsigned int hash=hash_func(buf);
	int ind=bin_search(hash, hashList, 0, 106);
	if(ind==-1){
		ind=bin_search(hash, dirHashList, 0, 12);
		if(ind==-1){
			printf("[identifier]");
			return -1;
		}
		//printf("[dir:%4s]", dirList[dirHashInd[ind]]);
		yylval.ival=ind;
		return DIRECTIVE;
	}
	else{
		//printf("[inst:%4s]", instList[hashInd[ind]]);
		yylval.ival=ind;
		return INSTR;
	}
}

int main( int /* argc */, char** /* argv */ ){
    lexer = new yyFlexLexer;
	while(lexer->yylex() != 0);
    return 0;
}

