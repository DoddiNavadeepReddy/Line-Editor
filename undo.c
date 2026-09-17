#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "undo.h"

/*
 * Single-slot undo history: just the last snapshot, as a plain copy
 * of the line array. We deliberately don't use a Document here to
 * avoid any confusion with the live document.
 */
static char **snapshot_lines = NULL;
static int snapshot_count = 0;
static int has_snapshot = 0;

void free_undo_snapshot(void) {
    if (snapshot_lines == NULL) {
        return;
    }

    for (int i = 0; i < snapshot_count; i++) {
        free(snapshot_lines[i]);
    }
    free(snapshot_lines);

    snapshot_lines = NULL;
    snapshot_count = 0;
    has_snapshot = 0;
}

void save_undo_snapshot(Document *doc) {
    if (doc == NULL) {
        return;
    }

    /* Drop whatever was saved before — we only support one level
     * of undo, so the newest snapshot replaces the old one. */
    free_undo_snapshot();

    if (doc->line_count == 0) {
        /* Nothing to copy, but we still mark that a (empty) snapshot
         * exists so undoing after e.g. an insert into an empty doc
         * correctly restores back to empty. */
        snapshot_lines = NULL;
        snapshot_count = 0;
        has_snapshot = 1;
        return;
    }

    snapshot_lines = malloc(doc->line_count * sizeof(char *));
    if (snapshot_lines == NULL) {
        /* Allocation failed: skip taking a snapshot rather than crash.
         * The next undo attempt will just report "nothing to undo". */
        has_snapshot = 0;
        return;
    }

    for (int i = 0; i < doc->line_count; i++) {
        size_t len = strlen(doc->lines[i]);
        snapshot_lines[i] = malloc(len + 1);
        if (snapshot_lines[i] == NULL) {
            /* Partial failure mid-copy: undo what we've built so far
             * and bail out cleanly rather than leaving a half-copied,
             * misleading snapshot around. */
            for (int j = 0; j < i; j++) {
                free(snapshot_lines[j]);
            }
            free(snapshot_lines);
            snapshot_lines = NULL;
            snapshot_count = 0;
            has_snapshot = 0;
            return;
        }
        strcpy(snapshot_lines[i], doc->lines[i]);
    }

    snapshot_count = doc->line_count;
    has_snapshot = 1;
}

int undo_last_action(Document *doc) {
    if (doc == NULL || !has_snapshot) {
        return 0;
    }

    /* Free the document's current lines before replacing them. */
    for (int i = 0; i < doc->line_count; i++) {
        free(doc->lines[i]);
    }
    free(doc->lines);

    int new_capacity = snapshot_count > 0 ? snapshot_count : 10;
    doc->lines = malloc(new_capacity * sizeof(char *));
    if (doc->lines == NULL) {
        /* Worst case: leave doc in a safe empty state rather than
         * crash or leave a dangling pointer. */
        doc->line_count = 0;
        doc->capacity = 0;
        free_undo_snapshot();
        return 0;
    }

    for (int i = 0; i < snapshot_count; i++) {
        size_t len = strlen(snapshot_lines[i]);
        doc->lines[i] = malloc(len + 1);
        if (doc->lines[i] == NULL) {
            /* Partial restore failure: keep what succeeded so far
             * rather than crash; line_count reflects what's really there. */
            doc->line_count = i;
            doc->capacity = new_capacity;
            free_undo_snapshot();
            return 1;
        }
        strcpy(doc->lines[i], snapshot_lines[i]);
    }

    doc->line_count = snapshot_count;
    doc->capacity = new_capacity;

    /* Single-level undo: once used, the snapshot is consumed. */
    free_undo_snapshot();
    return 1;
}