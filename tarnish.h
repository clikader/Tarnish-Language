#include "symbols.h"

typedef enum {typeCon, typeId, typeOp, typeStr, typeBool} nodeEnum;

/* Constants */
typedef struct {
    int value;
} conNodeType;

/* Identifiers */
typedef struct {
    varNode *node;
} idNodeType;

/* Operators */
typedef struct {
    int op;
    int nops;
    struct nodeTypeTag *ops[1];
} opNodeType;

/* Strings */
typedef struct {
    char *string;
} strNodeType;

/* Booleans */
typedef struct {
    int value;
} boolNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;

    union {
        conNodeType con;
        idNodeType id;
        opNodeType op;
        strNodeType str;
        boolNodeType boolean;
    };
} nodeType;

extern int sym[26];