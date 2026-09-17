#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "editor.h"
#include "file.h"
#include "undo.h"

int delete_line(Document *doc, int line_no) {
    if (doc == NULL) {
        return 0;
    }

    if (line_no < 1 || line_no > doc->line_count) {
        return 0;
    }

    /* Free the string being removed, then shift everything after it
     * left by one slot to close the gap. */
    free(doc->lines[line_no - 1]);

    for (int i = line_no - 1; i < doc->line_count - 1; i++) {
        doc->lines[i] = doc->lines[i + 1];
    }

    doc->line_count--;
    return 1;
}

void display(Document *doc) {
    if (doc == NULL) {
        return;
    }

    if (doc->line_count == 0) {
        printf("(empty document)\n");
        return;
    }

    for (int i = 0; i < doc->line_count; i++) {
        printf("%d: %s\n", i + 1, doc->lines[i]);
    }
}

/*
 * Splits `input` into a command token and the remaining argument
 * string. `command` must be at least 16 bytes. `*rest_out` points
 * into `input` itself (no copy) at the first character after the
 * command and its following space, or at the trailing '\0' if there
 * were no arguments.
 */
static void parse_command(char *input, char *command, size_t command_size, char **rest_out) {
    char *space = strchr(input, ' ');
    size_t cmd_len = space ? (size_t)(space - input) : strlen(input);

    if (cmd_len >= command_size) {
        cmd_len = command_size - 1;
    }

    strncpy(command, input, cmd_len);
    command[cmd_len] = '\0';

    *rest_out = space ? space + 1 : input + strlen(input);
}

void run_editor(Document *doc) {
    char input[512];
    char command[16];
    char *rest;

    printf("Line editor. Type 'help' for a list of commands.\n");

    while (1) {
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break; /* EOF (e.g. Ctrl+D) — exit cleanly instead of looping forever */
        }

        input[strcspn(input, "\n")] = '\0'; /* strip trailing newline */

        if (strlen(input) == 0) {
            continue; /* ignore blank lines */
        }

        parse_command(input, command, sizeof(command), &rest);

        if (strcmp(command, "i") == 0) {
            char *space = strchr(rest, ' ');
            if (space == NULL) {
                printf("Usage: i <line#> <text>\n");
                continue;
            }
            int line_no = atoi(rest);
            char *text = space + 1;
            save_undo_snapshot(doc);
            if (insert_line(doc, line_no, text)) {
                printf("Inserted.\n");
            } else {
                printf("Insert failed: invalid line number.\n");
            }

        } else if (strcmp(command, "d") == 0) {
            int line_no = atoi(rest);
            save_undo_snapshot(doc);
            if (delete_line(doc, line_no)) {
                printf("Deleted.\n");
            } else {
                printf("Delete failed: invalid line number.\n");
            }

        } else if (strcmp(command, "p") == 0) {
            display(doc);

        } else if (strcmp(command, "w") == 0) {
            if (strlen(rest) == 0) {
                printf("Usage: w <filename>\n");
                continue;
            }
            if (save_document(doc, rest)) {
                printf("Saved to %s\n", rest);
            } else {
                printf("Save failed: could not open file.\n");
            }

        } else if (strcmp(command, "r") == 0) {
            if (strlen(rest) == 0) {
                printf("Usage: r <filename>\n");
                continue;
            }
            save_undo_snapshot(doc);
            if (load_document(doc, rest)) {
                printf("Loaded from %s\n", rest);
            } else {
                printf("Load failed: file not found.\n");
            }

        } else if (strcmp(command, "s") == 0) {
            if (strlen(rest) == 0) {
                printf("Usage: s <keyword>\n");
                continue;
            }
            search_document(doc, rest);

        } else if (strcmp(command, "f") == 0) {
            char old_word[256];
            char new_word[256];
            if (sscanf(rest, "%255s %255s", old_word, new_word) != 2) {
                printf("Usage: f <old> <new>\n");
                continue;
            }
            save_undo_snapshot(doc);
            int changed = replace_all(doc, old_word, new_word);
            printf("Replaced in %d line(s).\n", changed);

        } else if (strcmp(command, "stats") == 0) {
            print_document_stats(doc);

        } else if (strcmp(command, "u") == 0) {
            if (undo_last_action(doc)) {
                printf("Undo successful.\n");
            } else {
                printf("Nothing to undo.\n");
            }

        } else if (strcmp(command, "help") == 0) {
            print_help();

        } else if (strcmp(command, "q") == 0) {
            printf("Goodbye.\n");
            break;

        } else {
            printf("Unknown command. Type 'help' for a list of commands.\n");
        }
    }

    free_undo_snapshot();
}