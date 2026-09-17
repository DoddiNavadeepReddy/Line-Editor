#ifndef EDITOR_H
#define EDITOR_H

#include "document.h"

/*
 * Deletes the line at `line_no` (1-based), shifting the lines below
 * it up. Returns 1 on success, 0 on failure (invalid line_no or
 * NULL doc). Never crashes on invalid input.
 */
int delete_line(Document *doc, int line_no);

/*
 * Prints every line with its 1-based line number.
 * Prints "(empty document)" if there are no lines.
 */
void display(Document *doc);

/*
 * Runs the interactive command loop: reads a line of input, parses
 * the command, dispatches to the right function, and repeats until
 * the user types 'q'.
 */
void run_editor(Document *doc);

#endif /* EDITOR_H */