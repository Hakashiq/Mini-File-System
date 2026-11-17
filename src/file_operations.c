#include "../include/file_manager.h"
#include "../include/file_operations.h"

/* ─────────────────────────────────────────────────────────────
   find_file()  — internal helper
   Walks the linked list of the current directory and returns
   a pointer to the FileNode whose name matches, or NULL.
   ───────────────────────────────────────────────────────────── */
static FileNode *find_file(const char *name) {
    FileNode *cur = current_directory->files;
    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

/* ─────────────────────────────────────────────────────────────
   create_file()
   1. Reads a name from the user.
   2. Checks for duplicates in the current directory.
   3. Allocates a new FileNode.
   4. Prepends it to the current directory's linked list.
   ───────────────────────────────────────────────────────────── */
void create_file(void) {
    char name[MAX_NAME];
    printf("  Enter file name: ");
    scanf("%63s", name);

    if (find_file(name) != NULL) {
        printf("  ERROR: File '%s' already exists in this directory.\n", name);
        return;
    }

    FileNode *new_file = (FileNode *) malloc(sizeof(FileNode));
    if (!new_file) { fprintf(stderr, "Out of memory.\n"); return; }

    strncpy(new_file->name, name, MAX_NAME);
    new_file->size    = 0;
    new_file->content = NULL;
    new_file->next    = NULL;
    get_timestamp(new_file->created_at);
    get_timestamp(new_file->modified_at);

    /* Prepend to the linked list (O(1)) */
    new_file->next            = current_directory->files;
    current_directory->files  = new_file;

    printf("  File '%s' created successfully.\n", name);
}

/* ─────────────────────────────────────────────────────────────
   delete_file()
   1. Reads a name from the user.
   2. Walks the list with prev + cur pointers.
   3. Patches links around the deleted node.
   4. Frees content and the node.
   ───────────────────────────────────────────────────────────── */
void delete_file(void) {
    char name[MAX_NAME];
    printf("  Enter file name to delete: ");
    scanf("%63s", name);

    FileNode *cur  = current_directory->files;
    FileNode *prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0) {
            /* Bypass the node */
            if (prev == NULL)
                current_directory->files = cur->next;  /* was head */
            else
                prev->next = cur->next;                /* was middle / tail */

            free(cur->content);
            free(cur);
            printf("  File '%s' deleted.\n", name);
            return;
        }
        prev = cur;
        cur  = cur->next;
    }

    printf("  ERROR: File '%s' not found.\n", name);
}

/* ─────────────────────────────────────────────────────────────
   write_file()
   Reads multi-line input until the user types "END" alone.
   Uses realloc to grow the buffer line by line.
   Overwrites any existing content.
   ───────────────────────────────────────────────────────────── */
void write_file(void) {
    char name[MAX_NAME];
    printf("  Enter file name to write to: ");
    scanf("%63s", name);
    getchar();   /* consume newline left by scanf */

    FileNode *f = find_file(name);
    if (!f) { printf("  ERROR: File '%s' not found.\n", name); return; }

    printf("  Enter content line by line. Type END on a new line to finish:\n");

    char   line[256];
    char  *buffer    = NULL;
    size_t total_len = 0;

    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) break;
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;

        size_t line_len = strlen(line);

        /* Grow the buffer */
        char *temp = (char *) realloc(buffer, total_len + line_len + 1);
        if (!temp) { fprintf(stderr, "Out of memory.\n"); free(buffer); return; }
        buffer = temp;

        memcpy(buffer + total_len, line, line_len);
        total_len += line_len;
        buffer[total_len] = '\0';
    }

    free(f->content);
    f->content = buffer;
    f->size    = total_len;
    get_timestamp(f->modified_at);

    printf("  Wrote %zu byte(s) to '%s'.\n", total_len, name);
}

/* ─────────────────────────────────────────────────────────────
   read_file()
   Finds the file and prints its content to stdout.
   ───────────────────────────────────────────────────────────── */
void read_file(void) {
    char name[MAX_NAME];
    printf("  Enter file name to read: ");
    scanf("%63s", name);

    FileNode *f = find_file(name);
    if (!f) { printf("  ERROR: File '%s' not found.\n", name); return; }

    printf("\n  --- Content of '%s' ---\n", name);
    if (f->content == NULL || f->size == 0)
        printf("  (empty file)\n");
    else
        printf("%s", f->content);
    printf("  --- End ---\n");
}

/* ─────────────────────────────────────────────────────────────
   search_file_recursive()  — internal DFS helper
   Recursively searches all directories for a file by name.
   Returns 1 if found and fills found_path; 0 otherwise.
   ───────────────────────────────────────────────────────────── */
static int search_recursive(DirectoryNode *dir,
                             const char    *target,
                             char          *found_path) {
    if (dir == NULL) return 0;

    /* Check files in this directory */
    FileNode *f = dir->files;
    while (f != NULL) {
        if (strcmp(f->name, target) == 0) {
            char dir_path[MAX_PATH] = {0};
            get_current_path(dir, dir_path);
            snprintf(found_path, MAX_PATH, "%s/%s", dir_path, f->name);
            return 1;
        }
        f = f->next;
    }

    /* Recurse into children */
    DirectoryNode *child = dir->children;
    while (child != NULL) {
        if (search_recursive(child, target, found_path)) return 1;
        child = child->sibling;
    }

    return 0;
}

void search_file(void) {
    char name[MAX_NAME];
    printf("  Enter file name to search: ");
    scanf("%63s", name);

    char found_path[MAX_PATH] = {0};
    if (search_recursive(filesystem_root, name, found_path))
        printf("  Found: %s\n", found_path);
    else
        printf("  File '%s' not found anywhere in the filesystem.\n", name);
}

/* ─────────────────────────────────────────────────────────────
   list_files()
   Walks the linked list of the current directory and prints
   each file's name and size.
   ───────────────────────────────────────────────────────────── */
void list_files(void) {
    FileNode *cur = current_directory->files;

    if (cur == NULL) {
        printf("  (no files in this directory)\n");
        return;
    }

    printf("\n  %-30s %12s\n", "File Name", "Size (bytes)");
    printf("  %-30s %12s\n", "------------------------------", "------------");
    while (cur != NULL) {
        printf("  %-30s %12zu\n", cur->name, cur->size);
        cur = cur->next;
    }
    printf("\n");
}

/* ─────────────────────────────────────────────────────────────
   display_file_info()
   Prints all metadata stored in a FileNode.
   ───────────────────────────────────────────────────────────── */
void display_file_info(void) {
    char name[MAX_NAME];
    printf("  Enter file name: ");
    scanf("%63s", name);

    FileNode *f = find_file(name);
    if (!f) { printf("  ERROR: File '%s' not found.\n", name); return; }

    printf("\n  ╔══════════════════════════════╗\n");
    printf(  "  ║       File Information       ║\n");
    printf(  "  ╠══════════════════════════════╣\n");
    printf(  "  ║ Name     : %-18s║\n", f->name);
    printf(  "  ║ Size     : %-14zu bytes ║\n", f->size);
    printf(  "  ║ Created  : %-18s║\n", f->created_at);
    printf(  "  ║ Modified : %-18s║\n", f->modified_at);
    printf(  "  ║ Content  : %-18s║\n", f->content ? "Yes" : "No (empty)");
    printf(  "  ╚══════════════════════════════╝\n");
}
