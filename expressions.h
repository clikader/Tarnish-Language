#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

typedef enum {intExpr, strExpr, boolExpr} exprEnum;

typedef struct {
    int val;
} intExprType;

typedef struct {
    char *val;
} strExprType;

typedef struct {
    int val;
} boolExprType;

typedef struct exprType {
    exprEnum type;

    union {
        intExprType integer;
        strExprType string;
        boolExprType boolean;
    };
} exprType;

#endif