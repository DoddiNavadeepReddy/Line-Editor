#ifndef FILE_H
#define FILE_H

#include "document.h"

/*
 * Writes every line in doc to filename.
 * Returns 1 on success, 0 if the file couldn't be opened.
 */
int save_document(Document *doc, const char *filename);

/*
 * Reads filename into doc, replacing whatever was previously in memory.
 * Returns 1 on success, 0 if the file couldn't be opened.
 */
int load_document(Document *doc, const char *filename);

#endif /* FILE_H */