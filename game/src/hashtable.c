// Hash Table adapted from:
// https://github.com/jamesroutley/write-a-hash-table
// Accessed 12/10/19

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

#include "hashtable.h"

static HashEntry DELETED = {NULL, NULL, false};

static HashEntry* newHashEntry(const char*, void*, bool);
static HashTable* newHashTableSized(const int);
static void linkMethods(HashTable*);

static bool valid(HashTable*, HashEntry*);
static void insert(HashTable*, const char*, void*, bool);
static void* search(HashTable*, const char*);
static void delete(HashTable*, const char*);
static void deleteShallow(HashTable*, const char*);

static int hash(const char*, const int, const int);
static void deleteHashEntry(HashEntry**);
static void deleteHashEntryShallow(HashEntry**);
static void deleteHashTable(HashTable**);
static void deleteHashTableShallow(HashTable**);

static void resize(HashTable*, const int);
static int _hash(const char*, const int, const int);
static int isPrime(const int);
static int nextPrime(int);


static HashEntry* newHashEntry(const char* key, void* value, bool isMalloc)
{
    HashEntry* entry;

    if (! (entry = (HashEntry*)malloc(sizeof(HashEntry))))
    {
        fprintf(stderr, ERR_HASHENTRY_MALLOC);
        return NULL;
    }

    memset(entry, 0, sizeof(HashEntry));

    if (! (entry->key = (char*)malloc(BUFSIZ * sizeof(char))))
    {
        SAFE_FREE(entry);
        return NULL;
    }

    strncpy(entry->key, key, BUFSIZ);
    entry->value = value;
    entry->isMalloc = isMalloc;

    return entry;
}


HashTable* newHashTable()
{
    return newHashTableSized(BASE_SIZE);
}


static HashTable* newHashTableSized(const int baseSize)
{
    HashTable* table;

    if (! (table = (HashTable*)malloc(sizeof(HashTable))))
        fprintf(stderr, ERR_HASHTABLE_MALLOC);

    table->baseSize = baseSize;
    table->size = nextPrime(table->baseSize);
    table->count = 0;

    if (! (table->items = (HashEntry**)malloc(table->size * sizeof(HashEntry*))))
        fprintf(stderr, ERR_HASHTABLE_MALLOC);

    memset(table->items, 0, table->size * sizeof(HashEntry*));
    linkMethods(table);

    return table;
}


static void linkMethods(HashTable* this)
{
    this->valid = valid;
    this->insert = insert;
    this->search = search;
    this->delete = delete;
    this->deleteShallow = deleteShallow;

    this->deleteHashTable = deleteHashTable;
    this->deleteHashTableShallow = deleteHashTableShallow;
}


static bool valid(HashTable* this, HashEntry* check)
{
    return check != NULL && check != &DELETED;
}


static void insert(HashTable* this, const char* key, void* value, bool isMalloc)
{
    HashEntry* item;
    HashEntry* current;
    int index;
    int i;

    if (((this->count * 100) / this->size) > 70)
        resizeUp(this);

    if (! (item = newHashEntry(key, value, isMalloc)))
        return;

    index = hash(item->key, this->size, 0);
    current = this->items[index];
    i = 0;

    while (current != NULL)
    {
        if (current != &DELETED)
        {
            if (! strncmp(current->key, key, BUFSIZ))
            {
                deleteHashEntry(&current);
                this->items[index] = item;
                return;
            }
        }

        index = hash(item->key, this->size, i++);
        current = this->items[index];
    }

    this->items[index] = item;
    this->count++;
}


static void* search(HashTable* this, const char* key)
{
    int index = hash(key, this->size, 0);
    HashEntry* item = this->items[index];
    int i = 1;

    while (item != NULL)
    {
        if (item != &DELETED)
            if (! strncmp(item->key, key, BUFSIZ))
                return item->value;

        index = hash(key, this->size, i++);
        item = this->items[index];
    }

    return NULL;
}


static void delete(HashTable* this, const char* key)
{
    int index = hash(key, this->size, 0);
    HashEntry* item = this->items[index];
    int i = 1;

    // This produces memory errors :(
    // Table will never downsize
    /*
    if (((this->count * 100) / this->size) < 10)
        resizeDown(this);
    */

    while (item != NULL)
    {
        if (item != &DELETED && ! strncmp(item->key, key, BUFSIZ))
        {
            deleteHashEntry(&item);
            this->items[index] = &DELETED;
            this->count--;
            return;
        }

        index = hash(key, this->size, i++);
        item = this->items[index];
    }
}


static void deleteShallow(HashTable* this, const char* key)
{
    int index = hash(key, this->size, 0);
    HashEntry* item = this->items[index];
    int i = 1;

    // This produces memory errors :(
    // Table will never downsize
    /*
    if (((this->count * 100) / this->size) < 10)
        resizeDown(this);
    */

    while (item != NULL)
    {
        if (item != &DELETED && ! strncmp(item->key, key, BUFSIZ))
        {
            deleteHashEntryShallow(&item);
            this->items[index] = &DELETED;
            this->count--;
            return;
        }

        index = hash(key, this->size, i++);
        item = this->items[index];
    }
}


static int hash(const char* string, const int bucket, const int attempt)
{
    const int a = _hash(string, HASHTABLE_PRIME_1, bucket);
    const int b = _hash(string, HASHTABLE_PRIME_2, bucket);
    return (a + attempt * (! b ? 1 : b)) % bucket;
}


static void deleteHashEntry(HashEntry** entry)
{
    if (*entry && *entry != &DELETED)
    {
        if ((*entry)->isMalloc)
            SAFE_FREE((*entry)->value);

        deleteHashEntryShallow(entry);
    }
}


static void deleteHashEntryShallow(HashEntry** entry)
{
    if (*entry && *entry != &DELETED)
    {
        free((*entry)->key);
        (*entry)->key = NULL;

        free(*entry);
        *entry = NULL;
    }
}


static void deleteHashTable(HashTable** table)
{
    for (int i = 0; i < (*table)->size; i++)
        deleteHashEntry((*table)->items + i);

    SAFE_FREE((*table)->items);
    SAFE_FREE(*table);
}


static void deleteHashTableShallow(HashTable** table)
{
    for (int i = 0; i < (*table)->size; i++)
        deleteHashEntryShallow((*table)->items + i);

    SAFE_FREE((*table)->items);
    SAFE_FREE(*table);
}


static void resize(HashTable* table, const int baseSize)
{
    if (baseSize < BASE_SIZE)
        return;

    HashTable* newTable = newHashTableSized(baseSize);
    HashEntry* item;

    HashEntry** tempItems;
    int temp;

    HASHTABLE_FOR_EACH(table, item)
        newTable->insert(newTable, item->key, item->value, item->isMalloc);

    table->baseSize = newTable->baseSize;
    table->count = newTable->count;

    temp = table->size;
    table->size = newTable->size;
    newTable->size = temp;

    tempItems = table->items;
    table->items = newTable->items;
    newTable->items = tempItems;

    newTable->deleteHashTableShallow(&newTable);
}


static int _hash(const char* string, const int prime, const int bucket)
{
    long hash = 0;
    const int length = strlen(string);
    for (int i = 0; i < length; i++)
    {
        hash += ((long)pow(prime, length - (i + 1)) * string[i]);
        hash %= bucket;
    }

    return (int)hash;
}


static int isPrime(const int x)
{
    if (x < 2)
        return -1;
    else if (x < 4)
        return 1;
    else if (! (x & 1))
        return 0;

    for (int i = 3; i <= floor(sqrt((double)x)); i += 2)
        if (! (x % i))
            return 0;

    return 1;
}


static int nextPrime(int x)
{
    while (! isPrime(x++));
    return x - 1;
}
