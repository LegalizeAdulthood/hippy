#ifndef YYERRCODE
#define YYERRCODE 256
#endif

#define REAL 257
#define INT 258
#define TEXT 259
#define ID 260
#define CMT 261
#define OP 262
#define DL 263
#define NL 264
#define FCN 265
#define FCALL 266
#define IF 267
#define THEN 268
#define ELSE 269
#define WHILE 270
#define KINT 271
#define KREAL 272
#define PRINT 273
#define SCAN 274
#define LTE 275
#define GTE 276
#define EQ 277
#define NEQ 278
#define PRINT_T 279
#define PRINT_V 280
#define IFX 281
typedef union{
  double real;
  int integer;
  char str[1024];
  TOKEN *ptoken;
} YYSTYPE;
extern YYSTYPE yylval;
