/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Chinmayi R                                                 */
/*--------------------------------------------------------------------*/
#include "symtable.h"

/* A Node_T object represents a single key-value binding within a symbol table.
   Each node contains a unique string key, an associated value pointer, 
   and a pointer to the next binding in a linked list. */
typedef struct Node Node_T;
struct Node {const char *key; const void *value; struct Node *next;};

/* What is the significance of typedef struct SymTable *SymTable_T; instead of typedef struct SymTable SymTable_T;?*/

struct SymTable {struct Node *first; size_t len;};

/* Free the memory associated with the Node structure p, including
   its key but not the value it points to. */
static void Node_free(Node_T *p){
    assert(p != NULL);
    free((char *)p->key);
    free(p);
}

/* Why are we making a list with just pointer first, cant we directly refer to list using a pointer: SymTable *root = ...*/

/* Create a new symbol table and return a pointer to it. 
   The table is initially empty and uses the first entry of BUCKET_COUNT 
   as its bucket count. */
SymTable_T SymTable_new()
{
 struct SymTable *p; /* So can I replace this with SymTable_T p;? or is it SymTable_T *p;? */
 p = calloc(1, sizeof(*p));
 if(p == NULL) { printf("Mem alloc Error"); return NULL;}
 p->len = 0;
 return p;
}

/* Free all memory associated with the symbol table oSymTable, 
   including all bindings and the table structure itself. */
void SymTable_free(SymTable_T oSymTable)
{
    Node_T *p = oSymTable->first;
    Node_T *next;

    assert(oSymTable != NULL);

    for ( ; p != NULL; p = next) {
        next = p->next;
        Node_free(p);
    }
    /* free(oSymTable->first);  Is this needed? Is double freeing happening?*/
    free(oSymTable);
}

/* Return the number of key-value bindings stored in the symbol table oSymTable. */
size_t SymTable_getLength(SymTable_T oSymTable){assert(oSymTable != NULL); return oSymTable->len;}

/* Insert a new binding with key pcKey and value pvValue into the symbol table oSymTable. 
   If pcKey already exists in oSymTable, the function does nothing and returns 0.
   Returns 1 on successful insertion. */
int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    Node_T *newNode;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /*assert(pvValue != NULL);*/

    if(SymTable_contains(oSymTable, pcKey)){return 0;}

    newNode = (Node_T *) calloc(1, sizeof(Node_T));
    if(newNode == NULL) {printf("Mem alloc Error"); return 0;}
    newNode->key = (const char*)malloc(strlen(pcKey) + 1);
    strcpy((char*)newNode->key, pcKey);
    newNode->value = pvValue;
    newNode->next = oSymTable->first;
    oSymTable->first = newNode;
    ++(oSymTable->len);
    return 1;
}

/* Replace the value associated with pcKey in the symbol table oSymTable with pvValue.
   Returns the old value associated with pcKey if it exists, otherwise returns NULL. */
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    Node_T *p = oSymTable->first;
    const void *temp;
    
    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /*assert(pvValue != NULL);*/

    if (p == NULL) {return NULL;}

    for (; p != NULL; p = p->next)
    {
        if (strcmp(p->key, pcKey) == 0) {
            temp = p->value;
            p->value = pvValue;
            return (void *) temp;}
    }
    return NULL;
}

/* Check if the symbol table oSymTable contains a binding for pcKey. 
   Returns 1 if pcKey is found, 0 otherwise. */
int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
    Node_T *p = oSymTable->first;
    
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

/* Retrieve the value associated with pcKey in the symbol table oSymTable.
   Returns NULL if pcKey is not found. */
void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
    Node_T *p = oSymTable->first;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (p == NULL) {return NULL;}

    for (; p != NULL; p = p->next) 
    {
        if (strcmp(p->key, pcKey) == 0) 
            return (void *) p->value;
    }
    return NULL;
}

/* Remove the binding for pcKey from the symbol table oSymTable, freeing its memory.
   Returns the value associated with pcKey, or NULL if pcKey is not found. */
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
    Node_T *p = oSymTable->first;
    Node_T *prev;
    const void *temp;
    
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
            return (void *) temp;
        }
        prev = p;
    }
    return NULL;
}

/* Apply the function pfApply to each binding in the symbol table oSymTable,
   passing pcKey, pvValue, and pvExtra as arguments. */
void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra)
{
    Node_T *p = oSymTable->first;
    
    assert(oSymTable != NULL);
    /*assert(pvExtra != NULL); Can It be null?? Is assert needed* - NO. See Ed https://edstem.org/us/courses/74019/discussion/6328027/ */

    /*assert(pfApply != NULL);Can It be null?? Is assert needed*/

    for ( ; p != NULL; p = p->next)
    (*pfApply)(p->key, (void *) p->value, (void *) pvExtra); /* Am I supposed to cast here with (void *) like this?*/
}