#ifndef UNDO_H
#define UNDO_H

#include "document.h"

/*
 * Saves a deep copy of doc's current state as the "undo point".
 * Call this right before any operation that mutates the document
 * (insert, delete, replace, load). Overwrites any previous snapshot,
 * so only the single most recent action can be undone.
 * Safe to call even if a previous snapshot exists or allocation fails.
 */
void save_undo_snapshot(Document *doc);

/*
 * Restores doc to the state saved by the last save_undo_snapshot()
 * call. Returns 1 on success, 0 if there's nothing to undo (no
 * snapshot taken yet, or it was already consumed by a prior undo).
 * Frees doc's current lines before restoring — never leaks.
 */
int undo_last_action(Document *doc);

/*
 * Frees any stored snapshot. Call once at program exit for a clean
 * valgrind run; also called internally before taking a new snapshot.
 */
void free_undo_snapshot(void);

#endif /* UNDO_H */