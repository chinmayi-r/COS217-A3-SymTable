#include "symtable.h"

typedef struct Node Node;
struct Node {const char *key; void *value; struct Node *next;};
/* What is the significance of typedef struct SymTable *SymTable_T; instead of typedef struct SymTable SymTable_T;?*/
struct SymTable {struct Node *first; size_t len;};

static void Node_free(Node *p){
    assert(p != NULL);
    free((char *)p->key);
    /*free(p->value);*/
    free(p);
}

/* Why are we making a list with just pointer first, cant we directly refer to list using a pointer: SymTable *root = ...*/
SymTable_T SymTable_new()
{
 struct SymTable *p; /* So can I replace this with SymTable_T p;? or is it SymTable_T *p;? */
 p = calloc(1, sizeof(*p));
 if(p == NULL) { printf("Mem alloc Error"); return NULL;}
 p->len = 0;
 return p;
}

void SymTable_free(SymTable_T oSymTable)
{
    Node *p = oSymTable->first;
    Node *next;

    assert(oSymTable != NULL);

    for ( ; p != NULL; p = next) {
        next = p->next;
        Node_free(p);
    }
    /* free(oSymTable->first);  Is this needed? Is double freeing happening?*/
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){assert(oSymTable != NULL); return oSymTable->len;}


int SymTable_put(SymTable_T oSymTable, const char *pcKey, void *pvValue)
{
    Node *newNode;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /*assert(pvValue != NULL);*/

    if(SymTable_contains(oSymTable, pcKey)){return 0;}

    newNode = (Node *) calloc(1, sizeof(Node));
    if(newNode == NULL) {printf("Mem alloc Error"); return 0;}
    newNode->key = (const char*)malloc(strlen(pcKey) + 1);
    strcpy((char*)newNode->key, pcKey);
    newNode->value = pvValue;
    newNode->next = oSymTable->first;
    oSymTable->first = newNode;
    ++(oSymTable->len);
    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    Node *p = oSymTable->first;
    void *temp;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /*assert(pvValue != NULL);*/

    if (p == NULL) {return NULL;}

    for (; p != NULL; p = p->next)
    {
        if (strcmp(p->key, pcKey) == 0) {
            temp = p->value;
            p->value = pvValue;
            return temp;}
    }
    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
    Node *p = oSymTable->first;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (p == NULL) {return 0;}

    for (; p != NULL; p = p->next) 
    {
        if (strcmp(p->key, pcKey) == 0) 
            return 1;
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
    Node *p = oSymTable->first;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (p == NULL) {return NULL;}

    for (; p != NULL; p = p->next) 
    {
        if (strcmp(p->key, pcKey) == 0) 
            return p->value;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
    Node *p = oSymTable->first;
    Node *prev;
    void *temp;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (p == NULL) {return NULL;}

    prev = NULL;
    for ( ; p != NULL; p = p->next) {
        if (strcmp(p->key, pcKey) == 0) { /* Diff from above!!!!! Note differences!!!*/
            if (prev == NULL) {oSymTable->first = p->next;}
            else {prev->next = p->next;}
            --(oSymTable->len);
            temp = p->value;
            Node_free(p);
            return temp;
        }
        prev = p;
    }
    return NULL;
}

void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra)
{
    Node *p = oSymTable->first;
    
    assert(oSymTable != NULL);
    /*assert(pvExtra != NULL); Can It be null?? Is assert needed* - NO. See Ed https://edstem.org/us/courses/74019/discussion/6328027/

    /*assert(pfApply != NULL);Can It be null?? Is assert needed*/

    for ( ; p != NULL; p = p->next)
    (*pfApply)(p->key, p->value, (void *) pvExtra); /* Am I supposed to cast here like this?*/
}