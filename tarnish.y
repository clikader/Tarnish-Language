%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include "tarnish.h"
    #include "expressions.h"
    #include "symbols.h"

    FILE *yyin;

    /* Prototypes */
    nodeType *op(int op, int nops, ...);
    nodeType *id(char *s, symEnum type);
    nodeType *con(int value);
    nodeType *str(char *s);
    nodeType *boolean(int value);
    void freeNode(nodeType *p);
    exprType *ex(nodeType *p);
    int yylex(void);
    void yyerror(char *s);
    int sym[26];
%}

%union {
    int iValue;
    char *sId;
    nodeType *nPtr;
    char *strValue;
    int boolValue;
}

%token <iValue> INTEGER;
%token <sId> ID;
%token <strValue> STRING;
%token <boolValue> BOOLEAN;
%token PUT IN WHILE IF PRINT INT STR BOOL;
%nonassoc IFX;
%nonassoc ELSE;

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%left '%'
%nonassoc NEGATIVE

%type <nPtr> stmt expr stmt_list

%%

program:
            function                { exit(0); }
        ;

function:
            function stmt           { ex($2); freeNode($2); }
        |   /* NULL */
        ;

stmt_list:
            stmt                    { $$ = $1; }
        |   stmt_list stmt          { $$ = op(';', 2, $1, $2); }
        ;

stmt:
            ';'                     { $$ = op(';', 2, NULL, NULL); }
        |   expr ';'                { $$ = $1; }
        |   PRINT expr ';'          { $$ = op(PRINT, 1, $2); }
        |   PUT expr IN INT ID      { symEnum varType = intNode; $$ = op(PUT, 2, id($5, varType), $2); }
        |   PUT expr IN STR ID      { symEnum varType = strNode; $$ = op(PUT, 2, id($5, varType), $2); }
        |   PUT expr IN BOOL ID     { symEnum varType = boolNode; $$ = op(PUT, 2, id($5, varType), $2); }
        |   PUT expr IN ID          { $$ = op(PUT, 2, id($4, (symEnum)NULL), $2); }
        |   WHILE '(' expr ')' stmt { $$ = op(WHILE, 2, $3, $5); }
        |   IF '(' expr ')' stmt %prec IFX 
                                    { $$ = op(IF, 2, $3, $5); }
        |   IF '(' expr ')' stmt ELSE stmt
                                    { $$ = op(IF, 3, $3, $5, $7); }
        |   '{' stmt_list '}'       { $$ = $2; }
        ;

expr:
            INTEGER                 { $$ = con($1); }
        |   ID                      { $$ = id($1, (symEnum)NULL); }
        |   STRING                  { $$ = str($1); }
        |   BOOLEAN                 { $$ = boolean($1); }
        |   '-' expr %prec NEGATIVE { $$ = op(NEGATIVE, 1, $2); }
        |   expr '+' expr           { $$ = op('+', 2, $1, $3); }
        |   expr '-' expr           { $$ = op('-', 2, $1, $3); }
        |   expr '*' expr           { $$ = op('*', 2, $1, $3); }
        |   expr '/' expr           { $$ = op('/', 2, $1, $3); }
        |   expr '%' expr           { $$ = op('%', 2, $1, $3); }
        |   expr '<' expr           { $$ = op('<', 2, $1, $3); }
        |   expr '>' expr           { $$ = op('>', 2, $1, $3); }
        |   expr GE expr           { $$ = op(GE, 2, $1, $3); }
        |   expr LE expr           { $$ = op(LE, 2, $1, $3); }
        |   expr '=' expr          { $$ = op('=', 2, $1, $3); }
        |   expr NE expr           { $$ = op(NE, 2, $1, $3); }
        |   '(' expr ')'           { $$ = $2; }
        ;

%%

#define SIZEOF_NODETYPE ((char *) &p->con - (char *)p)

nodeType *con(int value) {
    nodeType *p;

    /* Allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL) {
        yyerror("Out of memory");
    }

    /* Copy Information */
    p->type = typeCon;
    p->con.value = value;

    return p;
}

nodeType *id(char *s, symEnum type) {
    nodeType *p;

    /* Allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL) {
        yyerror("Out of memory");
    }

    /* Copy Information */
    p->type = typeId;

    if (!type) {
        p->id.node = getSymbol(s);
        if (!p->id.node) {
            yyerror("Undeclared variable");
        }
    } else {
        if ((p->id.node = malloc(sizeof(varNode))) == NULL) {
            yyerror("Out of memory");
        }
        p->id.node->type = type;
        p->id.node->id = strdup(s);
    }

    return p;
}

nodeType *op(int op, int nops, ...) {
    va_list ap;
    nodeType *p;
    int i;

    /* Allocate node, extending op array */
    if ((p = malloc(sizeof(nodeType) + (nops - 1) * sizeof(nodeType *))) == NULL) {
        yyerror("Out of memory");
    }

    /* Copy Information */
    p->type = typeOp;
    p->op.op = op;
    p->op.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++) {
        p->op.ops[i] = va_arg(ap, nodeType*);
    }
    va_end(ap);
    return p;
}

nodeType *str(char *s) {
    nodeType *p;

    /* Allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL) {
        yyerror("Out of memory");
    }

    p->type = typeStr;
    p->str.string = strdup(s);
    return p;
}

nodeType *boolean(int value) {
    nodeType *p;

    /* Allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL) {
        yyerror("Out of memory");
    }

    /* Copy Information */
    p->type = typeBool;
    p->boolean.value = value;
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOp) {
        for (i = 0; i < p->op.nops; i++) {
            freeNode(p->op.ops[i]);
        }
    }
    free (p);
}

void yyerror(char *s) { 
    fprintf(stdout, "%s\n", s); 
}

int main(int argc, char **argv) { 
    /* Process command line args*/
    yyin = fopen(argv[1], "r");
    yyparse();
    yylex();
    fclose(yyin);
    return 0; 
}