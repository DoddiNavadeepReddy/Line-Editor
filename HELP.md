# Help — Line Editor Commands

| Command | Syntax | Description |
|---|---|---|
| Insert | `i <line#> <text>` | Insert `text` at line `<line#>`, shifting existing lines down. |
| Delete | `d <line#>` | Remove the line at `<line#>`, shifting the lines below it up. |
| Display | `p` | Print all lines with their line numbers. |
| Save | `w <filename>` | Write the in-memory document to `<filename>`. |
| Load | `r <filename>` | Read `<filename>` into the document. |
| Search | `s <keyword>` | Print every line number and line containing `<keyword>`. |
| Find & replace | `f <old> <new>` | Replace every occurrence of `<old>` with `<new>` across the document. |
| Stats | `stats` | Print total line count and word count. |
| Undo | `u` | Undo the last insert, delete, find & replace, or load. Single-level only. |
| Help | `help` | Show this list of commands. |
| Quit | `q` | Exit the program. |

## Examples

```
i 1 Hello world
i 2 This is the second line
p
1: Hello world
2: This is the second line

d 1
p
1: This is the second line

s second
1: This is the second line

f second first
1: This is the first line

stats
Lines: 1
Words: 5

w notes.txt
r notes.txt

q
```