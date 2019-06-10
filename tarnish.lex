%{
    #include <stdlib.h>
    #include <string.h>
    #include "tarnish.h"
    #include "y.tab.h"
    void yyerror (char *);
%}

%%

0                   {
                        yylval.iValue = atoi(yytext);
                        return INTEGER;
                    }

[1-9][0-9]*         {
                        yylval.iValue = atoi(yytext);
                        return INTEGER;
                    }

\"[^"\n]*["\n]      { 
                        yylval.strValue = strdup(yytext+1); 
                        if (yylval.strValue[yyleng-2] != '"') 
                            yyerror("improperly terminated string"); 
                        else 
                            yylval.strValue[yyleng-2] = 0; 
                        return STRING;
                    }

">="                return GE;
"<="                return LE;
"!="                return NE;
"put"               return PUT;
"in"                return IN;
"while"             return WHILE;
"if"                return IF;
"else"              return ELSE;
"print"             return PRINT;
"int"               return INT;
"string"            return STR;
"true"              {
                        yylval.boolValue = 1;
                        return BOOLEAN;    
                    }
"false"              {
                        yylval.boolValue = 0;
                        return BOOLEAN;    
                    }                    
"bool"              return BOOL;

[a-z][a-zA-Z0-9_]*  {
                        yylval.sId = strdup(yytext);
                        return ID;
                    }

[-()<>=+*/;{}]      {
                        return *yytext;
                    }

\/\/.*              /* Ignore comments */

[ \t\n]+            /* Ignore Whitespace */

.                   yyerror("Unknown Character");

%%

int yywrap(void) {
    return 1;
}