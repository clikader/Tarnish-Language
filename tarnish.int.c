#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tarnish.h"
#include "symbols.h"
#include "expressions.h"
#include "y.tab.h"

void yyerror (char *);

exprType *makeIntExpr(int val) {
    exprType *e;

    if ((e = malloc(sizeof(exprType))) == NULL) {
        yyerror("Out of memory");
    }

    e->type = intExpr;
    e->integer.val = val;
    return e;
}

exprType *makeStrExpr(char *val) {
    exprType *e;

    if ((e = malloc(sizeof(exprType))) == NULL) {
        yyerror("Out of memory");
    }

    e->type = strExpr;
    e->string.val = strdup(val);
    return e;
}

exprType *makeBoolExpr(int val) {
    exprType *e;

    if ((e = malloc(sizeof(exprType))) == NULL) {
        yyerror("Out of memory");
    }

    e->type = boolExpr;
    e->integer.val = val;
    return e;
}

char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      return str;
}

exprType *ex(nodeType *p) {
    if (!p) return 0;
    switch(p->type) { 
        case typeCon:       return makeIntExpr(p->con.value); 
        case typeId:        switch(p->id.node->type) {
                                case intNode:   return makeIntExpr(p->id.node->i.val);
                                case strNode:   return makeStrExpr(p->id.node->s.val);
                                case boolNode:  return makeBoolExpr(p->id.node->b.val);
                            }
        case typeStr:       return makeStrExpr(p->str.string);
        case typeBool:      return makeBoolExpr(p->boolean.value);
        case typeOp: 
            switch(p->op.op) { 
                case WHILE:     {
                                    exprType *e = ex(p->op.ops[0]);
                                    if (e->type == boolExpr)
                                        while(e->boolean.val) {
                                            ex(p->op.ops[1]);
                                            free(e);
                                            e = ex(p->op.ops[0]);
                                        }
                                    else
                                        printf("Error: Loop condition is not a boolean\n");
                                    free(e);
                                    return makeIntExpr(0); 
                                }
                case IF:        {
                                    exprType *e = ex(p->op.ops[0]);
                                    if (e->type == boolExpr) {
                                        if (e->boolean.val) 
                                            ex(p->op.ops[1]); 
                                        else if (p->op.nops > 2) 
                                            ex(p->op.ops[2]);
                                    }
                                    else
                                        printf("Error: If condition is not a boolean\n");
                                    free(e);
                                    return makeIntExpr(0);
                                }
                case PRINT:     {
                                    exprType *e = ex(p->op.ops[0]);
                                    if (!e) {
                                        printf("NULL\n");
                                        return makeIntExpr(0);
                                    }
                                    else {
                                        switch(e->type) {
                                            case intExpr:   printf("%d\n", ex(p->op.ops[0])->integer.val); 
                                                            return makeIntExpr(0);
                                            case strExpr:   printf("%s\n", ex(p->op.ops[0])->string.val);
                                                            return makeIntExpr(0);
                                            case boolExpr:  if(ex(p->op.ops[0])->boolean.val == 1) printf("True\n");
                                                            else printf("False\n");
                                                            return makeIntExpr(0);
                                            default:   return makeIntExpr(0);
                                        }
                                    }
                                }
                case ';':       ex(p->op.ops[0]); 
                                return ex(p->op.ops[1]); 
                case PUT:       { 
                                    exprType *e = ex(p->op.ops[1]);
                                    if (!p->op.ops[0]->id.node) {
                                        // Do nothing. Variable is not declared
                                    }
                                    else if (p->op.ops[0]->id.node->type == intNode && e->type == intExpr) {
                                        addSymbol(p->op.ops[0]->id.node);
                                        updateIntSymbol(p->op.ops[0]->id.node->id, e->integer.val);
                                    }
                                    else if (p->op.ops[0]->id.node->type == intNode) {
                                        printf("Error: Expression is not of type INT\n");
                                    }
                                    else if (p->op.ops[0]->id.node->type == strNode && e->type == strExpr) {
                                        addSymbol(p->op.ops[0]->id.node);
                                        updateStrSymbol(p->op.ops[0]->id.node->id, e->string.val);
                                    }
                                    else if (p->op.ops[0]->id.node->type == strNode) {
                                        printf("Error: Expression is not of type STR\n");
                                    }
                                    else if (p->op.ops[0]->id.node->type == boolNode && e->type == boolExpr) {
                                        addSymbol(p->op.ops[0]->id.node);
                                        updateBoolSymbol(p->op.ops[0]->id.node->id, e->boolean.val);
                                    }
                                    else if (p->op.ops[0]->id.node->type == boolNode) {
                                        printf("Error: Expression is not of type BOOLEAN\n");
                                    }
                                    free(e);
                                    return makeIntExpr(0);
                                }
                case NEGATIVE:  {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *e;
                                    if (left->type == intExpr)
                                        e = makeIntExpr(-(left->integer.val));
                                    else if (left->type == strExpr)
                                        e = makeStrExpr(strrev(left->string.val));
                                    else if (left->type ==boolExpr && left->boolean.val == 1)
                                        e = makeBoolExpr(0);
                                    else if (left->type ==boolExpr && left->boolean.val == 0)
                                        e = makeBoolExpr(1);
                                    free(left);
                                    return e; 
                                }
                case '+':       {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeIntExpr(left->integer.val + right->integer.val);
                                        else if (left->type == strExpr)
                                            e = makeStrExpr(strcat(left->string.val, right->string.val));
                                        else if (left->type == boolExpr)
                                            e = makeBoolExpr(left->boolean.val && right->boolean.val);
                                    }
                                    else {
                                        printf("Error: '+' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                }
                case '-':       {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeIntExpr(left->integer.val - right->integer.val);
                                        else if (left->type == strExpr)
                                            printf("Error: '-' is not defined for type STR\n");
                                        else if (left->type == boolExpr)
                                            e = makeBoolExpr(left->boolean.val || right->boolean.val);
                                    }
                                    else {
                                        printf("Error: '-' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                }
                case '*':       {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeIntExpr(left->integer.val * right->integer.val);
                                        else if (left->type == strExpr)
                                            printf("Error: '*' is undefined for type STR\n");
                                        else if (left->type == boolExpr)
                                            printf("Error: '*' is undefined for type BOOL\n");
                                    }
                                    else {
                                        printf("Error: '*' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                } 
                case '/':       {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeIntExpr(left->integer.val / right->integer.val);
                                        else if (left->type == strExpr)
                                            printf("Error: '/' is undefined for type STR\n");
                                        else if (left->type == boolExpr)
                                            printf("Error: '/' is undefined for type BOOL\n");
                                    }
                                    else {
                                        printf("Error: '/' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                } 
                case '<':       {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeBoolExpr(left->integer.val < right->integer.val);
                                        else if (left->type == strExpr)
                                            e = makeBoolExpr(strcmp(left->string.val, right->string.val) == -1);
                                        else if (left->type == boolExpr)
                                            printf("Error: '<' is undefined for type BOOL\n");
                                    }
                                    else {
                                        printf("Error: '<' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                } 
                case '>':       {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeBoolExpr(left->integer.val > right->integer.val);
                                        else if (left->type == strExpr)
                                            e = makeBoolExpr(strcmp(left->string.val, right->string.val) == 1);
                                        else if (left->type == boolExpr)
                                            printf("Error: '>' is undefined for type BOOL\n");
                                    }
                                    else {
                                        printf("Error: '>' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                } 
                case GE:        {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeBoolExpr(left->integer.val >= right->integer.val);
                                        else if (left->type == strExpr)
                                            e = makeBoolExpr(strcmp(left->string.val, right->string.val) >= 0);
                                        else if (left->type == boolExpr)
                                            printf("Error: '>=' is undefined for type BOOL\n");
                                    }
                                    else {
                                        printf("Error: '>=' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                } 
                case LE:        {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeBoolExpr(left->integer.val <= right->integer.val);
                                        else if (left->type == strExpr)
                                            e = makeBoolExpr(strcmp(left->string.val, right->string.val) <= 0);
                                        else if (left->type == boolExpr)
                                            printf("Error: '>=' is undefined for type BOOL\n");
                                    }
                                    else {
                                        printf("Error: '>=' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                } 
                case NE:        {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeBoolExpr(left->integer.val != right->integer.val);
                                        else if (left->type == strExpr)
                                            e = makeBoolExpr(strcmp(left->string.val, right->string.val) != 0);
                                        else if (left->type == boolExpr)
                                            e = makeBoolExpr(left->boolean.val != right->boolean.val);
                                    }
                                    else {
                                        printf("Error: '!=' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                }
                case '=':       {
                                    exprType *left = ex(p->op.ops[0]);
                                    exprType *right = ex(p->op.ops[1]);
                                    exprType *e;
                                    if (left->type == right->type) {
                                        if (left->type == intExpr)
                                            e = makeBoolExpr(left->integer.val == right->integer.val);
                                        else if (left->type == strExpr)
                                            e = makeBoolExpr(strcmp(left->string.val, right->string.val) == 0);
                                        else if (left->type == boolExpr)
                                            e = makeBoolExpr(left->boolean.val == right->boolean.val);
                                    }
                                    else {
                                        printf("Error: '==' type mismatch\n");
                                    }
                                    free(left);
                                    free(right);
                                    return e;
                                }
    }
  }   
  return 0; 
}