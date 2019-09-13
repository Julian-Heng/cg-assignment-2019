#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "hashtable.h"


static int hash(const char*, const int, const int);
static int getHashTableHash(const char*, const int, const int);

static HashTableItem* newHashTableItem(const char*, void*, bool);
static HashTable* newHashTableSized(const int);

static void resizeHashTable(HashTable*, const int);
static void resizeHashTableUp(HashTable*);
static void resizeHashTableDown(HashTable*);

static void deleteHashTableItem(HashTableItem**);

static int isPrime(const int);
static int nextPrime(int);

static HashTableItem DELETED = {NULL, NULL, false};

static int hash(const char* str, const int prime, const int bucket)
{
    long hash = 0;
    int len = strlen(str);
    int i;

    for (i = 0; i < len; i++)
    {
        hash += (long)pow(prime, len - (i + 1)) * str[i];
        hash %= bucket;
    }

    return (int)hash;
}


static int getHashTableHash(const char* str, const int size, const int attempt)
{
    const int hashA = hash(str, PRIME_1, size);
    const int hashB = hash(str, PRIME_2, size);
    int result = (hashA + (attempt * (hashB + 1))) % size;
    return result;
}


static HashTableItem* newHashTableItem(const char* key, void* value,
                                       bool isMalloc)
{
    HashTableItem* item;
    int len;

    if (! (item = (HashTableItem*)malloc(sizeof(HashTableItem))))
    {
        fprintf(stderr, ERR_ITEM_MALLOC);
        return NULL;
    }

    memset(item, 0, sizeof(HashTableItem));
    len = strlen(key) + 1;

    if (! (item->key = (char*)malloc(len * sizeof(char))))
    {
        fprintf(stderr, ERR_ITEM_KEY_MALLOC);
        SAFE_FREE(item);
        return NULL;
    }

    strncpy(item->key, key, len);
    item->value = value;
    item->isMalloc = isMalloc;

    return item;
}


HashTable* newHashTable()
{
    /*
    HashTable* table;

    if (! (table = (HashTable*)malloc(sizeof(HashTable))))
    {
        fprintf(stderr, ERR_TABLE_MALLOC);
        return NULL;
    }

    memset(table, 0, sizeof(HashTable));

    table->size = 53;
    table->count = 0;

    if (! (table->items = (HashTableItem**)malloc(table->size * sizeof(HashTableItem*))))
    {
        fprintf(stderr, ERR_ITEMS_MALLOC);
        SAFE_FREE(table);
        return NULL;
    }

    memset(table->items, 0, table->size * sizeof(HashTableItem*));

    return table;
    */
    return newHashTableSized(INITIAL_SIZE);
}


static HashTable* newHashTableSized(const int size)
{
    HashTable* table;

    if (! (table = (HashTable*)malloc(sizeof(HashTable))))
    {
        fprintf(stderr, ERR_TABLE_MALLOC);
        return NULL;
    }

    memset(table, 0, sizeof(HashTable));

    table->baseSize = size;
    table->size = nextPrime(table->baseSize);
    table->count = 0;

    if (! (table->items = (HashTableItem**)malloc(table->size * sizeof(HashTableItem*))))
    {
        fprintf(stderr, ERR_ITEMS_MALLOC);
        SAFE_FREE(table);
        return NULL;
    }

    memset(table->items, 0, table->size * sizeof(HashTableItem*));

    return table;
}

static void resizeHashTable(HashTable* table, const int size)
{
    HashTableItem* item;
    HashTableItem** temp;
    int i;

    if (size >= INITIAL_SIZE)
    {
        HashTable* newTable = newHashTableSized(size);

        for (i = 0; i < table->size; i++)
        {
            item = table->items[i];

            if (item && item != &DELETED)
            {
                insertHashTableItem(newTable, item->key, item->value,
                                    item->isMalloc);
            }
        }

        table->baseSize = newTable->baseSize;
        table->count = newTable->count;

        temp = table->items;
        table->items = newTable->items;
        newTable->items = temp;

        deleteHashTable(&newTable);
    }
}


static void resizeHashTableUp(HashTable* table)
{
    const int newSize = table->baseSize * 2;
    fprintf(stderr, "Resizing up:\n    Size: %d\n    newSize: %d\n", table->baseSize, newSize);
    resizeHashTable(table, newSize);
}


static void resizeHashTableDown(HashTable* table)
{
    const int newSize = table->baseSize / 2;
    fprintf(stderr, "Resizing down:\n    Size: %d\n    newSize: %d\n", table->baseSize, newSize);
    resizeHashTable(table, newSize);
}


void insertHashTableItem(HashTable* table, const char* key, void* value,
                         bool isMalloc)
{
    fprintf(stderr, "Insert:\n    Key: %s\n    Value: %s\n", key, (char*)value);

    HashTableItem* item;
    HashTableItem* currItem;

    int index;
    int i = 1;

    const int load = table->count * 100 / table->size;

    if (load > 70)
    {
        resizeHashTableUp(table);
    }

    if ((item = newHashTableItem(key, value, isMalloc)))
    {
        index = getHashTableHash(item->key, table->size, 0);
        currItem = table->items[index];

        while (currItem)
        {
            if (currItem != &DELETED)
            {
                if (! strcmp(currItem->key, key))
                {
                    deleteHashTableItem(&currItem);
                    table->items[index] = item;
                    return;
                }
            }

            index = getHashTableHash(item->key, table->size, i++);
            currItem = table->items[index];
        }

        table->items[index] = item;
        table->count++;
    }
}


void* searchHashTableItem(HashTable* table, const char* key)
{
    HashTableItem* item;

    int index;
    int i = 1;
    bool found = false;

    index = getHashTableHash(key, table->size, i++);
    item = table->items[index];

    while (item && ! found)
    {
        found = item != &DELETED && ! strcmp(item->key, key);
        index = getHashTableHash(item->key, table->size, i++);
        item = table->items[index];
    }

    return item->value ? item->value : NULL;
}


void removeHashTableItem(HashTable* table, const char* key)
{
    HashTableItem* item;
    int index;
    int i = 1;

    const int load = table->count * 100 / table->size;

    if (load < 10)
    {
        resizeHashTableDown(table);
    }

    index = getHashTableHash(key, table->size, i++);
    item = table->items[index];

    while (item)
    {
        if (item != &DELETED &&
            ! strcmp(item->key, key))
        {
            deleteHashTableItem(&item);
            table->items[index] = &DELETED;
        }

        index = getHashTableHash(key, table->size, i++);
        item = table->items[index];
    }

    table->count--;
}


static void deleteHashTableItem(HashTableItem** item)
{
    if (*item)
    {
        SAFE_FREE((*item)->key);

        if ((*item)->isMalloc)
        {
            SAFE_FREE((*item)->value);
        }

        SAFE_FREE(*item);
    }
}


void deleteHashTable(HashTable** table)
{
    int i;

    if (*table)
    {
        for (i = 0; i < (*table)->size; i++)
        {
            deleteHashTableItem(&((*table)->items[i]));
        }
    }

    SAFE_FREE((*table)->items);
    SAFE_FREE(*table);
}


static int isPrime(const int x)
{
    int i;

    if (x < 2)
    {
        return -1;
    }
    else if (x < 4)
    {
        return 1;
    }
    else if ((x % 2) == 0)
    {
        return 0;
    }

    for (i = 3; i <= floor(sqrt((double)x)); i += 2)
    {
        if (! (x % i))
        {
            return 0;
        }
    }

    return 1;
}


static int nextPrime(int x)
{
    while (! isPrime(x)) x++;
    return x;
}
