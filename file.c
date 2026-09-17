#include <stdio.h>
#include <string.h>

#include "file.h"

int save_document(Document *doc, const char *filename)
{
    if (doc == NULL || filename == NULL)
    {
        return 0;
    }

    FILE *fp = fopen(filename, "w");

    if (fp == NULL)
    {
        return 0;
    }

    for (int i = 0; i < doc->line_count; i++)
    {
        fprintf(fp, "%s\n", doc->lines[i]);
    }

    fclose(fp);

    return 1;
}


int load_document(Document *doc, const char *filename)
{
    if (doc == NULL || filename == NULL)
    {
        return 0;
    }

    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        return 0;
    }


    
    

    /* Remove existing document */
    free_document(doc);
    init_document(doc);

    char buffer[1024];

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        buffer[strcspn(buffer, "\n")] = '\0';

        insert_line(doc, doc->line_count + 1, buffer);
    }

    fclose(fp);

    return 1;
}