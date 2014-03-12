#ifndef _LIGER_TOKENS_H_
#define _LIGER_TOKENS_H_

enum {
      ID = 256,
      NUM,
      STR,
      VAR,
/* More tokens go here. */
};

extern char* yytext;

extern int yylex(void);

#endif
