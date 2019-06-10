#ifndef SYMBOLS_H
#define SYMBOLS_H

typedef enum {intNode=1, strNode, boolNode} symEnum;

typedef struct {
    int val;
} intVar;

typedef struct {
    char *val;
} strVar;

typedef struct {
    int val;
} boolVar;

typedef struct varNodeType {
    symEnum type;
    char *id;
    union {
        intVar i;
        strVar s;
        boolVar b;
    };
    struct varNodeType *next;
} varNode;

int addSymbol(varNode *node);
int updateIntSymbol(char *id, int val);
int updateStrSymbol(char *id, char *val);
int updateBoolSymbol(char *id, int val);
varNode *getSymbol(char *id);

#endif