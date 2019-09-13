#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stdbool.h>

#define ERR_ITEM_MALLOC "Cannot allocate memory for hash table item\n"
#define ERR_ITEM_KEY_MALLOC "Cannot allocate memory for item key\n"
#define ERR_ITEMS_MALLOC "Cannot allocate memory for hash table items\n"
#define ERR_TABLE_MALLOC "Cannot allocate memory for hash table\n"

#define INITIAL_SIZE 53
#define PRIME_1 67
#define PRIME_2 199


typedef struct HashTableItem
{
    char* key;
    void* value;
    bool isMalloc;
} HashTableItem;


typedef struct HashTable
{
    int baseSize;
    int size;
    int count;
    HashTableItem** items;
} HashTable;


HashTable* newHashTable();
void insertHashTableItem(HashTable*, const char*, void*, bool);
void* searchHashTableItem(HashTable*, const char*);
void removeHashTableItem(HashTable*, const char*);

void deleteHashTable(HashTable**);

#endif
