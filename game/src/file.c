#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"


char* fileRead(char* filename)
{
    char* file = NULL;
    int count = 0;
    int i;
    int ch;
    FILE *fp;

    if ((fp = fopen(filename, "r")))
    {
        while ((ch = fgetc(fp)) != EOF && ! ferror(fp))
        {
            count++;
        }

        fseek(fp, 0, SEEK_SET);
        file = (char*)malloc((count + 1) * sizeof(char));
        memset(file, 0, count + 1);
        i = -1;

        while ((ch = fgetc(fp)) != EOF && ! ferror(fp))
        {
            file[++i] = ch;
        }

        fclose(fp);
    }

    return file;
}
