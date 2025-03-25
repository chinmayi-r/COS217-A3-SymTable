#include "symtable.h"

static size_t BUCKET_COUNT[] = {509, 1021, 2039, 4093, 8191, 16381, 32749, 65521};
/*enum { BUCKET_COUNT = 509 };*/

typedef struct Binding Binding_T;
struct Binding {
 const char *key;
 const void *value;
 struct Binding *next;
};

struct SymTable {
 struct Binding **buckets;
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

static void Binding_free(Binding_T *p){
    assert(p != NULL);
    free((char *)p->key);
    /*free(p->value);*/
    free(p);
}

SymTable_T SymTable_new()
{
 struct SymTable *p; /* So can I replace this with SymTable_T p;? or is it SymTable_T *p;? */
 struct Binding **q;
 p = (struct SymTable *) calloc(1, sizeof(*p));
 if(p == NULL) {printf("Mem alloc Error"); return NULL;}
 q = (struct Binding **) calloc(BUCKET_COUNT[0], sizeof(*q));
 if(q == NULL) {printf("Mem alloc Error"); return NULL;}
 p->buckets = q;
 p->size = BUCKET_COUNT[0];
 p->len = 0;
 return p;
}


void SymTable_free(SymTable_T oSymTable)
{
    Binding_T *p;
    Binding_T *next;
    size_t i;
    
    assert(oSymTable != NULL);

    for (i=0; i < oSymTable->size; i++)
    {
        p = oSymTable->buckets[i];
        while (p != NULL)
        {
            next = p->next;
            Binding_free(p);
            p = next;
        }
    }
    free(oSymTable->buckets); /* Is this needed?*/
    free(oSymTable);
}

size_t SymTable_getLength(SymTable_T oSymTable){assert(oSymTable != NULL); return oSymTable->len;}

static void SymTable_resize(SymTable_T oSymTable, size_t size)
{
    Binding_T **old_buckets;
    Binding_T **new_buckets;
    Binding_T *buckets_i;
    Binding_T *next; 
    size_t i;
    /*if (oSymTable->size > size) {
		printf("SymTable_resize(): requested size too small");
		return NULL;
	}*/

    size_t old_size = oSymTable->size;
    old_buckets = oSymTable->buckets;
    new_buckets = (struct Binding **) calloc(size, sizeof(*new_buckets));
    if(new_buckets == NULL) {printf("Mem alloc Error"); return;}

    oSymTable->buckets = new_buckets;
    oSymTable->size = size;
    oSymTable->len = 0; /* Since SymTable_put increments len when reinserting bindings*/

	for (i = 0; i < old_size; i++) {
        buckets_i = old_buckets[i];
        while (buckets_i != NULL)
        {
            next = buckets_i->next;
            SymTable_put(oSymTable, buckets_i->key, buckets_i->value);
            Binding_free(buckets_i);
            buckets_i = next;
        }
	}
	free(old_buckets);
    return;
}

int findIndex(size_t arr[], size_t size, size_t value) {
    size_t i;
    for (i = 0; i < size; i++) {
        if (arr[i] == value) {
            return i;
        }
    }
    return -1;
}

int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue)
{
    size_t hash_value;
    Binding_T *newBinding;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /*assert(pvValue != NULL);*/

    if(SymTable_contains(oSymTable, pcKey)){return 0;}
    assert(0);

    /* NOTE that >= does not mean no of elements >= BUCKETCOUNT!!! 
    Since ++(oSymTable->len); is happening later, the no of elements is actually (oSymTable->len+1) !*/
    if ((oSymTable->len >= oSymTable->size) && oSymTable->size != BUCKET_COUNT[7])
    {
        SymTable_resize(oSymTable, BUCKET_COUNT[findIndex(BUCKET_COUNT, 8, oSymTable->size)+1]);
        assert(0);
        printf("resized to");
        printf((char *)oSymTable->size);
    }
    assert(0);
    printf("asa");
    fflush(stdout);
    hash_value = SymTable_hash(pcKey, oSymTable->size);
    newBinding = (Binding_T *) calloc(1, sizeof(Binding_T));
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
    Binding_T *p = oSymTable->buckets[hash_value];
    const void *temp;

    assert(oSymTable != NULL);
    assert(pcKey != NULL);
    /*assert(pvValue != NULL);*/

    if (p == NULL) {return NULL;}

    while(p != NULL)
    {
        if (strcmp(p->key, pcKey) == 0){
            temp = p->value;
            p->value = pvValue;
            return (void *) temp;
        }
        p = p->next;
    }
    return NULL;
}

int SymTable_contains(SymTable_T oSymTable, const char *pcKey)
{
    size_t hash_value = SymTable_hash(pcKey, oSymTable->size);
    Binding_T *p = oSymTable->buckets[hash_value];

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
    Binding_T *p = oSymTable->buckets[hash_value];

    assert(oSymTable != NULL);
    assert(pcKey != NULL);

    if (p == NULL) {return NULL;}

    while(p != NULL)
    {
        if (strcmp(p->key, pcKey) == 0)
            return (void *) p->value;
        p = p->next;
    }
    return NULL;
}

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey)
{
    size_t hash_value = SymTable_hash(pcKey, oSymTable->size);
    Binding_T *p = oSymTable->buckets[hash_value];
    Binding_T *prev;
    const void *temp;

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
            return (void *) temp;
        }
        prev = p;
        p = p->next;
    }
    return NULL;
}

void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra)
{
    Binding_T *p;
    size_t i;

    assert(oSymTable != NULL);
    /*assert(pvExtra != NULL); Can It be null?? Is assert needed* - NO. See Ed https://edstem.org/us/courses/74019/discussion/6328027/ */

    /*assert(pfApply != NULL);Can It be null?? Is assert needed*/

    for (i=0; i < oSymTable->size; i++)
    {
        p = oSymTable->buckets[i];
        while (p != NULL)
        {
            (*pfApply)(p->key, (void *) p->value, (void *) pvExtra);
            p = p->next;
        }
    }
}