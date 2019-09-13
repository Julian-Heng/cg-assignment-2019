#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphics.h"
#include "hashtable.h"
#include "macros.h"

#include "game.h"

int main()
{
    bool ret = false;


    /*
    Backend* engine = init();

    if (engine->window)
    {
        loop(engine);
    }

    terminate(&engine);
    */

    char** strs;
    char key[BUFSIZ];

    HashTable* table = newHashTable();

    strs = (char**)malloc(100 * sizeof(char*));
    for (int i = 0; i < BUFSIZ; i++)
    {
        strs[i] = (char*)malloc(BUFSIZ * sizeof(char));
        //strncpy(strs[i], "testing hash table", BUFSIZ);
        snprintf(strs[i], BUFSIZ, "Testing hash table with string no. %d", i + 1);
        snprintf(key, BUFSIZ, "string %d", i + 1);
        insertHashTableItem(table, key, strs[i], true);
    }
    SAFE_FREE(strs);

    deleteHashTable(&table);

    return (int)ret;
}
