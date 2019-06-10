#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symbols.h"

varNode *first = (varNode *)NULL;

// Add new symbol with type declaration
int addSymbol(varNode *node) {
    //printf("Adding Symbol '%s'\n", node->id);
    if (!first) {
        first = node;
        return 0;
    } else {
        varNode *current = first;

        while (current != NULL) {
            if (strcmp(current->id, node->id) == 0) {
                //printf("Symbol '%s' already exists\n", node->id);
                return 1;
            }
            current = current->next;
        }

        node->next = first;
        first = node;
        return 0;
    }
}

int updateIntSymbol(char *id, int val) {
    //printf("Updating INT Symbol '%s'\n", id);
    if (!first) {
        return 1;
    } else {
        varNode *current = first;

        while (current != NULL) {
            //printf("Checking Symbol '%s'\n", current->id);
            if (strcmp(current->id, id) == 0) {
                if (current->type == intNode) {
                    //printf("Current Value: %d\n", current->i.val);
                    //printf("New Value: %d\n", val);
                    current->i.val = val;
                } else {
                    //printf("Cannot assign string to int.\n");
                    return 1;
                }
                return 0;
            }
            current = current->next;
        }

        return 1;
    }
}

int updateStrSymbol(char *id, char *val) {
    //printf("Updating STR Symbol '%s'\n", id);
    if (!first) {
        return 1;
    } else {
        varNode *current = first;

        while (current != NULL) {
            if (strcmp(current->id, id) == 0) {
                if (current->type == strNode) {
                    current->s.val = strdup(val);
                } else {
                    //printf("Cannot assign int to string.\n");
                    return 1;
                }
                return 0;
            }
            current = current->next;
        }

        return 1;
    }
}

int updateBoolSymbol(char *id, int val) {
    if (!first) {
        return 1;
    } else {
        varNode *current = first;

        while (current != NULL) {
            //printf("Checking Symbol '%s'\n", current->id);
            if (strcmp(current->id, id) == 0) {
                if (current->type == boolNode) {
                    //printf("Current Value: %d\n", current->i.val);
                    //printf("New Value: %d\n", val);
                    current->b.val = val;
                } else {
                    //printf("Cannot assign string to int.\n");
                    return 1;
                }
                return 0;
            }
            current = current->next;
        }

        return 1;
    }
}

varNode *getSymbol(char *id) {
    //printf("Finding Symbol '%s'\n", id);
    if (!first) {
        return NULL;
    } else {
        varNode *current = first;

        while (current != NULL) {
            if (strcmp(current->id, id) == 0) {
                //printf("Found '%s'\n", current->id);
                return current;
            }
            current = current->next;
        }

        return NULL;
    }
}