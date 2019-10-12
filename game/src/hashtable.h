#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdio.h>

#define HASHTABLE_PRIME_1 661
#define HASHTABLE_PRIME_2 811
#define BASE_SIZE 53

#define ERR_HASHENTRY_MALLOC "Error: unable to allocate memory for hash entry\n"
#define ERR_HASHTABLE_MALLOC "Error: unable to allocate memory for hash table\n"

#define resizeUp(ht)    resize((ht), (ht)->size << 1)
#define resizeDown(ht)  resize((ht), (ht)->size >> 1)

/* !!! DANGEROUS !!! */
#define HASHTABLE_FOR_EACH(ht, iter) \
    for (int i = ((iter) = (ht)->items[0], 0); \
         i < (ht)->size; \
         i += ((iter) = (ht)->items[i + 1], 1)) \
        if ((ht)->valid((ht), (iter)))


typedef struct HashEntry
{
    char* key;
    void* value;
    bool isMalloc;
} HashEntry;


typedef struct HashTable
{
    int baseSize;
    int size;
    int count;
    HashEntry** items;

    bool (*valid)(struct HashTable*, struct HashEntry*);

    void (*insert)(struct HashTable*, const char*, void*, bool);
    void* (*search)(struct HashTable*, const char*);
    void (*delete)(struct HashTable*, const char*);
    void (*deleteShallow)(struct HashTable*, const char*);

    void (*deleteHashTable)(struct HashTable**);
    void (*deleteHashTableShallow)(struct HashTable**);
} HashTable;


HashTable* newHashTable(void);

#endif
