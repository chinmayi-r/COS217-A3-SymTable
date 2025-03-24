#include "symtable.h"

enum { BUCKET_COUNT = 509 };

typedef struct Binding Binding;
struct Binding {
 const char *key;
 void *value;
 struct Binding *next;
};

typedef struct SymTable *SymTable_T;
struct SymTable {
 struct Binding *buckets[BUCKET_COUNT];
 size_t size;
 size_t len;
};

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

static void Binding_free(Binding *p){
    assert(p != NULL);
    free((char *)p->key);
    /*free(p->value);*/
    free(p);
}

SymTable_T SymTable_new()
{
 struct SymTable *p; /* So can I replace this with SymTable_T p;? or is it SymTable_T *p;? */
 p = calloc(1, sizeof(*p));
 if(p == NULL) {printf("Mem alloc Error"); return NULL;}
 p->size = BUCKET_COUNT;
 p->len = 0;
 return p;
}


void SymTable_free(SymTable_T oSymTable)
{
    Binding *p;
    int i;
    
    assert(oSymTable != NULL);

    for (i=0; i < oSymTable->size; i++)
    {
        p = oSymTable->buckets[i];
        while (p != NULL)
        {
            p = p->next;
            Binding_free(p);
        }
    }
    free(oSymTable->buckets); /* Is this needed?*/
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){assert(oSymTable != NULL); return oSymTable->len;}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    size_t hash_value = SymTable_hash(pcKey, oSymTable->size);
    Binding *newBinding;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /*assert(pvValue != NULL);*/

    if(SymTable_contains(oSymTable, pcKey)){return 0;}
    
    newBinding = (Binding *) calloc(1, sizeof(Binding));
    if(newBinding == NULL) {printf("Mem alloc Error"); return 0;}
    newBinding->key = (const char*)malloc(strlen(pcKey) + 1);
    strcpy((char*)newBinding->key, pcKey);
    newBinding->value = pvValue;
    newBinding->next = oSymTable->buckets[hash_value];
    oSymTable->buckets[hash_value] = newBinding;
    ++(oSymTable->len);
    return 1;
}

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    size_t hash_value = SymTable_hash(pcKey, oSymTable->size);
    Binding *p = oSymTable->buckets[hash_value];
    void *temp;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /*assert(pvValue != NULL);*/

    if (p == NULL) {return NULL;}

    while(p != NULL)
    {
        if (strcmp(p->key, pcKey) == 0){
            temp = p->value;
            p->value = pvValue;
            return temp;
        }
        p = p->next;
    }
    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
    size_t hash_value = SymTable_hash(pcKey, oSymTable->size);
    Binding *p = oSymTable->buckets[hash_value];

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (p == NULL) {return 0;}

    while(p != NULL)
    {
        if (strcmp(p->key, pcKey) == 0)
            return 1;
        p = p->next;
    }
    return 0;
}

void *SymTable_get(SymTable_T oSymTable, const char *pcKey)
{
    size_t hash_value = SymTable_hash(pcKey, oSymTable->size);
    Binding *p = oSymTable->buckets[hash_value];

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (p == NULL) {return NULL;}

    while(p != NULL)
    {
        if (strcmp(p->key, pcKey) == 0)
            return p->value;
        p = p->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
    size_t hash_value = SymTable_hash(pcKey, oSymTable->size);
    Binding *p = oSymTable->buckets[hash_value];
    Binding *prev;
    void *temp;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (p == NULL) {return NULL;}

    prev = NULL;
    while(p != NULL)
    {
        if (strcmp(p->key, pcKey) == 0) { /* Diff from above!!!!! Note differences!!!*/
            if (prev == NULL) {oSymTable->buckets[hash_value] = p->next;}
            else {prev->next = p->next;}
            --(oSymTable->len);
            temp = p->value;
            Binding_free(p);
            return temp;
        }
        prev = p;
        p = p->next;
    }
    return NULL;
}

void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra)
{
    Binding *p;
    int i;

    assert(oSymTable != NULL);
    /*assert(pvExtra != NULL); Can It be null?? Is assert needed* - NO. See Ed https://edstem.org/us/courses/74019/discussion/6328027/

    /*assert(pfApply != NULL);Can It be null?? Is assert needed*/

    for (i=0; i < oSymTable->size; i++)
    {
        p = oSymTable->buckets[i];
        while (p != NULL)
        {
            (*pfApply)(p->key, p->value, (void *) pvExtra);
            p = p->next;
        }
    }
}