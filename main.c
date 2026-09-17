#include "document.h"
#include "editor.h"

int main(void)
{
    Document doc;

    init_document(&doc);

    run_editor(&doc);

    free_document(&doc);

    return 0;
}