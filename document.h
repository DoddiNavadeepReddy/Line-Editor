#ifndef DOCUMENT_H
#define DOCUMENT_H

/*
 * Document: holds the in-memory text document as a dynamic array
 * of line strings.
 *
 * lines       - array of char* pointers, one per line, heap-allocated
 * line_count  - number of lines currently stored (0-based count, but
 *               lines are addressed with 1-based line numbers by callers)
 * capacity    - number of slots currently allocated in `lines`
 */
typedef struct {
    char **lines;
    int line_count;
    int capacity;
} Document;

/* Initializes doc with an empty line array and starting capacity. */
void init_document(Document *doc);

/*
 * Inserts a copy of `text` as line `line_no` (1-based), shifting any
 * existing lines at or after that position down by one.
 *
 * Valid range for line_no: 1 .. doc->line_count + 1
 * (line_count + 1 means "append at the end").
 *
 * Returns 1 on success, 0 on failure (bad line_no, NULL args, or
 * allocation failure). Never crashes on invalid input.
 */
int insert_line(Document *doc, int line_no, const char *text);

/*
 * Frees every stored line string, frees the lines array itself, and
 * resets the struct fields to a safe empty state.
 * Safe to call on an already-freed or zeroed Document.
 */
void free_document(Document *doc);

/* Prints usage instructions for every supported command. */
void print_help(void);

/* --- Bonus features --- */

/*
 * Searches every line for `keyword` (case-sensitive substring match)
 * and prints each matching line number and its text.
 * Prints "No matches found." if nothing matches.
 */
void search_document(Document *doc, const char *keyword);

/*
 * Replaces every occurrence of `old_word` with `new_word` on a single
 * line (1-based line_no). Returns 1 if at least one replacement was
 * made, 0 if the line had no match, invalid line_no, or on failure.
 */
int replace_in_line(Document *doc, int line_no, const char *old_word, const char *new_word);

/*
 * Replaces every occurrence of `old_word` with `new_word` across all
 * lines in the document. Returns the total number of lines changed.
 */
int replace_all(Document *doc, const char *old_word, const char *new_word);

/*
 * Prints simple statistics: total line count and total word count
 * (words are whitespace-separated tokens, summed across all lines).
 */
void print_document_stats(Document *doc);

/* --- Prototypes for teammates' functions (implemented elsewhere) --- */
int delete_line(Document *doc, int line_no);
void display(Document *doc);
int save_document(Document *doc, const char *filename);
int load_document(Document *doc, const char *filename);

#endif /* DOCUMENT_H */