/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Chinmayi R                                                   */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

/* SymTable_T represents a symbol table, which is a collection 
   of key-value bindings. Each key is a unique string, and each value 
   is an associated void pointer to data of any type. */
typedef struct SymTable *SymTable_T;
struct SymTable;

/* Create a new symbol table and return a pointer to it. 
   The table is initially empty and uses the first entry of BUCKET_COUNT 
   as its bucket count. */
   SymTable_T SymTable_new(void);
   
   /* Return the number of key-value bindings stored in the symbol table oSymTable. */
   size_t SymTable_getLength(SymTable_T oSymTable);

   /* Free all memory associated with the symbol table oSymTable, 
      including all bindings and the table structure itself. */
   void SymTable_free(SymTable_T oSymTable);
   
   /* Insert a new binding with key pcKey and value pvValue into the symbol table oSymTable. 
      If pcKey already exists in oSymTable, the function does nothing and returns 0.
      Returns 1 on successful insertion. */
   int SymTable_put(SymTable_T oSymTable, const char *pcKey, const void *pvValue);
   
   /* Replace the value associated with pcKey in the symbol table oSymTable with pvValue.
      Returns the old value associated with pcKey if it exists, otherwise returns NULL. */
   void *SymTable_replace(SymTable_T oSymTable, const char *pcKey, const void *pvValue);
   
   /* Check if the symbol table oSymTable contains a binding for pcKey. 
      Returns 1 if pcKey is found, 0 otherwise. */
   int SymTable_contains(SymTable_T oSymTable, const char *pcKey);
   
   /* Retrieve the value associated with pcKey in the symbol table oSymTable.
      Returns NULL if pcKey is not found. */
   void *SymTable_get(SymTable_T oSymTable, const char *pcKey);
   
   /* Remove the binding for pcKey from the symbol table oSymTable, freeing its memory.
      Returns the value associated with pcKey, or NULL if pcKey is not found. */
   void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);
   
   /* Apply the function pfApply to each binding in the symbol table oSymTable,
      passing pcKey, pvValue, and pvExtra as arguments. */
   void SymTable_map(SymTable_T oSymTable, void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra), const void *pvExtra);
   

#endif