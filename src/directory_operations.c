#include "../include/file_manager.h"
#include "../include/directory_operations.h"

/* ─────────────────────────────────────────────────────────────
   print_current_path()
   Wrapper: builds and prints the full path of current_directory.
   ───────────────────────────────────────────────────────────── */
void print_current_path(void) {
    char path[MAX_PATH] = {0};
    get_current_path(current_directory, path);
    printf("%s", path);
}

/* ─────────────────────────────────────────────────────────────
   create_directory()
   1. Reads a name from the user.
   2. Checks for duplicates among the current dir's children.
   3. Allocates a new DirectoryNode.
   4. Prepends it to the children sibling chain.
   ───────────────────────────────────────────────────────────── */
void create_directory(void) {
    char name[MAX_NAME];
    printf("  Enter new directory name: ");
    scanf("%63s", name);

    /* Duplicate check among siblings */
    DirectoryNode *c = current_directory->children;
    while (c != NULL) {
        if (strcmp(c->name, name) == 0) {
            printf("  ERROR: Directory '%s' already exists here.\n", name);
            return;
        }
        c = c->sibling;
    }

    DirectoryNode *new_dir = (DirectoryNode *) malloc(sizeof(DirectoryNode));
    if (!new_dir) { fprintf(stderr, "Out of memory.\n"); return; }

    strncpy(new_dir->name, name, MAX_NAME);
    new_dir->files    = NULL;
    new_dir->parent   = current_directory;
    new_dir->children = NULL;
    new_dir->sibling  = NULL;

    /* Prepend to sibling chain (O(1)) */
    new_dir->sibling              = current_directory->children;
    current_directory->children   = new_dir;

    printf("  Directory '%s' created.\n", name);
}

/* ─────────────────────────────────────────────────────────────
   delete_directory()
   Only deletes a directory that has no files and no children.
   Patches the sibling chain of the parent after removal.
   ───────────────────────────────────────────────────────────── */
void delete_directory(void) {
    char name[MAX_NAME];
    printf("  Enter directory name to delete: ");
    scanf("%63s", name);

    if (strcmp(name, "root") == 0) {
        printf("  ERROR: Cannot delete the root directory.\n");
        return;
    }

    DirectoryNode *cur  = current_directory->children;
    DirectoryNode *prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0) {
            if (cur->files != NULL || cur->children != NULL) {
                printf("  ERROR: Directory '%s' is not empty.\n", name);
                return;
            }
            /* Bypass the node in the sibling chain */
            if (prev == NULL)
                current_directory->children = cur->sibling;
            else
                prev->sibling = cur->sibling;

            free(cur);
            printf("  Directory '%s' deleted.\n", name);
            return;
        }
        prev = cur;
        cur  = cur->sibling;
    }

    printf("  ERROR: Directory '%s' not found.\n", name);
}

/* ─────────────────────────────────────────────────────────────
   change_directory()
   Supports:
     ..       → go to parent directory
     /root    → jump directly to filesystem root
     <name>   → move into a child directory by name
   ───────────────────────────────────────────────────────────── */
void change_directory(void) {
    char name[MAX_NAME];
    printf("  Enter directory ('..' = parent, '/root' = root): ");
    scanf("%63s", name);

    if (strcmp(name, "..") == 0) {
        if (current_directory->parent == NULL)
            printf("  Already at /root.\n");
        else {
            current_directory = current_directory->parent;
            printf("  Moved to parent directory.\n");
        }
        return;
    }

    if (strcmp(name, "/root") == 0) {
        current_directory = filesystem_root;
        printf("  Moved to /root.\n");
        return;
    }

    /* Search children */
    DirectoryNode *c = current_directory->children;
    while (c != NULL) {
        if (strcmp(c->name, name) == 0) {
            current_directory = c;
            printf("  Moved into '%s'.\n", name);
            return;
        }
        c = c->sibling;
    }

    printf("  ERROR: Directory '%s' not found under current directory.\n", name);
}
