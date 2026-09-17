#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"

#define INITIAL_CAPACITY 10

void init_document(Document *doc) {
    if (doc == NULL) {
        return;
    }

    doc->lines = malloc(INITIAL_CAPACITY * sizeof(char *));
    if (doc->lines == NULL) {
        /* Allocation failed; leave doc in a safe, empty, non-crashing state. */
        doc->line_count = 0;
        doc->capacity = 0;
        return;
    }

    doc->line_count = 0;
    doc->capacity = INITIAL_CAPACITY;
}

/*
 * Doubles the pointer array's capacity using realloc().
 * Returns 1 on success, 0 on failure. On failure, doc->lines is left
 * untouched (still valid) because we never overwrite the original
 * pointer with realloc()'s return value until we know it succeeded.
 */
static int grow_capacity(Document *doc) {
    int new_capacity = doc->capacity * 2;
    char **new_lines = realloc(doc->lines, new_capacity * sizeof(char *));

    if (new_lines == NULL) {
        /* realloc failed: doc->lines still points to the original,
         * still-valid block. Nothing is lost. */
        return 0;
    }

    doc->lines = new_lines;
    doc->capacity = new_capacity;
    return 1;
}

int insert_line(Document *doc, int line_no, const char *text) {
    if (doc == NULL || text == NULL) {
        return 0;
    }

    /* Valid range is 1 .. line_count + 1 (append is allowed at the end). */
    if (line_no < 1 || line_no > doc->line_count + 1) {
        return 0;
    }

    if (doc->line_count == doc->capacity) {
        if (!grow_capacity(doc)) {
            return 0;
        }
    }

    /* Make our own copy of the string. The document owns this memory —
     * we never store a pointer into a caller's temporary buffer. */
    char *text_copy = malloc(strlen(text) + 1);
    if (text_copy == NULL) {
        return 0;
    }
    strcpy(text_copy, text);

    /* Shift existing pointers from the end backward to open a gap
     * at index (line_no - 1). Iterating from the end avoids
     * overwriting entries we still need to move. */
    for (int i = doc->line_count; i > line_no - 1; i--) {
        doc->lines[i] = doc->lines[i - 1];
    }

    doc->lines[line_no - 1] = text_copy;
    doc->line_count++;

    return 1;
}

void print_help(void) {
    printf("Available commands:\n");
    printf("  i <line#> <text>   Insert text at line# (shifts lines down)\n");
    printf("                     e.g. i 2 Hello world\n");
    printf("  d <line#>          Delete the line at line#\n");
    printf("                     e.g. d 3\n");
    printf("  p                  Print the document with line numbers\n");
    printf("                     e.g. p\n");
    printf("  w <filename>       Save the document to a file\n");
    printf("                     e.g. w notes.txt\n");
    printf("  r <filename>       Load a document from a file\n");
    printf("                     e.g. r notes.txt\n");
    printf("  s <keyword>        Search for lines containing keyword\n");
    printf("                     e.g. s hello\n");
    printf("  f <old> <new>      Replace old word with new word everywhere\n");
    printf("                     e.g. f hello hi\n");
    printf("  stats              Show line count and word count\n");
    printf("  u                  Undo the last insert/delete/replace/load\n");
    printf("  help               Show this help message\n");
    printf("  q                  Quit the program\n");
}

void search_document(Document *doc, const char *keyword) {
    if (doc == NULL || keyword == NULL) {
        return;
    }

    int found = 0;
    for (int i = 0; i < doc->line_count; i++) {
        if (strstr(doc->lines[i], keyword) != NULL) {
            printf("%d: %s\n", i + 1, doc->lines[i]);
            found = 1;
        }
    }

    if (!found) {
        printf("No matches found.\n");
    }
}

/*
 * Builds a new heap-allocated string with every occurrence of
 * `old_word` in `line` replaced by `new_word`.
 * Returns NULL if `old_word` isn't found in `line`, or on
 * allocation failure. Caller owns the returned string.
 */
static char *build_replaced_string(const char *line, const char *old_word, const char *new_word) {
    size_t old_len = strlen(old_word);
    size_t new_len = strlen(new_word);

    if (old_len == 0 || strstr(line, old_word) == NULL) {
        return NULL;
    }

    /* Count occurrences so we know exactly how much space to allocate. */
    int occurrences = 0;
    const char *scan = line;
    while ((scan = strstr(scan, old_word)) != NULL) {
        occurrences++;
        scan += old_len;
    }

    size_t result_len = strlen(line) - (size_t)occurrences * old_len + (size_t)occurrences * new_len;

    char *result = malloc(result_len + 1);
    if (result == NULL) {
        return NULL;
    }

    char *out = result;
    const char *cur = line;
    while ((scan = strstr(cur, old_word)) != NULL) {
        size_t chunk_len = (size_t)(scan - cur);
        memcpy(out, cur, chunk_len);
        out += chunk_len;
        memcpy(out, new_word, new_len);
        out += new_len;
        cur = scan + old_len;
    }
    strcpy(out, cur); /* copy the remainder, including the null terminator */

    return result;
}

int replace_in_line(Document *doc, int line_no, const char *old_word, const char *new_word) {
    if (doc == NULL || old_word == NULL || new_word == NULL) {
        return 0;
    }

    if (line_no < 1 || line_no > doc->line_count) {
        return 0;
    }

    char *replaced = build_replaced_string(doc->lines[line_no - 1], old_word, new_word);
    if (replaced == NULL) {
        return 0;
    }

    free(doc->lines[line_no - 1]);
    doc->lines[line_no - 1] = replaced;
    return 1;
}

int replace_all(Document *doc, const char *old_word, const char *new_word) {
    if (doc == NULL || old_word == NULL || new_word == NULL) {
        return 0;
    }

    int changed_count = 0;
    for (int i = 1; i <= doc->line_count; i++) {
        if (replace_in_line(doc, i, old_word, new_word)) {
            changed_count++;
        }
    }
    return changed_count;
}

void print_document_stats(Document *doc) {
    if (doc == NULL) {
        return;
    }

    int total_words = 0;
    for (int i = 0; i < doc->line_count; i++) {
        /* Work on a copy since strtok modifies the string it scans. */
        size_t len = strlen(doc->lines[i]);
        char *copy = malloc(len + 1);
        if (copy == NULL) {
            continue; /* skip word count for this line rather than crash */
        }
        strcpy(copy, doc->lines[i]);

        char *token = strtok(copy, " \t");
        while (token != NULL) {
            total_words++;
            token = strtok(NULL, " \t");
        }

        free(copy);
    }

    printf("Lines: %d\n", doc->line_count);
    printf("Words: %d\n", total_words);
}

void free_document(Document *doc) {
    if (doc == NULL || doc->lines == NULL) {
        return;
    }

    for (int i = 0; i < doc->line_count; i++) {
        free(doc->lines[i]);
    }

    free(doc->lines);

    doc->lines = NULL;
    doc->line_count = 0;
    doc->capacity = 0;
}