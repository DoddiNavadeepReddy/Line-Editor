# Simple Line Editor in C

A command-line line editor built for the "Build a Simple Line Editor in C" coding competition.

## Team

- **G V Raghuveer** — Document struct, core data structure (`init_document`, `insert_line`, `free_document`), search, find & replace, line/word count, help command
- **D Navadeep Reddy** — `delete_line`, `display`, command parsing / user interaction, invalid line-number handling
- **Eshan S K** — `save_document`, `load_document`, final `main.c` integration, testing

## Data structure

We used a **dynamic array of strings** (`char **lines`, growing via `realloc`), since our documents are expected to be small — the O(n) cost of shifting lines on insert/delete is negligible, and array indexing keeps line lookup and display simple.

```c
typedef struct {
    char **lines;
    int line_count;
    int capacity;
} Document;
```

## Features implemented

**Core (required 2–3):**
- Insert a line
- Delete a line
- Display the document

**Bonus:**
- Search (find line numbers containing a word/phrase)
- Find & replace (single line or whole document)
- Line count / word count

## Commands

See `HELP.md` for the full command list with examples, or type `help` inside the editor once it's running.

## Compiling

```bash
gcc -Wall -Wextra -std=c11 -o line_editor main.c document.c
```

(Once `editor.c`/`file.c` are added, they'll be included in this same command.)

## Running

```bash
./line_editor
```

Type `help` at any point to see available commands, `q` to quit.