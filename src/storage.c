#include "../include/file_manager.h"

/* ════════════════════════════════════════════════════════════
   SAVE  (in-memory tree  →  binary file)
   ════════════════════════════════════════════════════════════ */

static void save_files(FILE *fp, FileNode *head) {
    /* Count files */
    int count = 0;
    FileNode *f = head;
    while (f) { count++; f = f->next; }
    fwrite(&count, sizeof(int), 1, fp);

    f = head;
    while (f != NULL) {
        fwrite(f->name,        sizeof(char),   MAX_NAME, fp);
        fwrite(&f->size,       sizeof(size_t), 1,        fp);
        fwrite(f->created_at,  sizeof(char),   32,       fp);
        fwrite(f->modified_at, sizeof(char),   32,       fp);

        /* Write content length, then content bytes */
        size_t clen = (f->content) ? f->size : 0;
        fwrite(&clen, sizeof(size_t), 1, fp);
        if (clen > 0) fwrite(f->content, sizeof(char), clen, fp);

        f = f->next;
    }
}

static void save_directory(FILE *fp, DirectoryNode *dir) {
    if (!dir) return;

    fwrite(dir->name, sizeof(char), MAX_NAME, fp);
    save_files(fp, dir->files);

    /* Count and write children count */
    int child_count = 0;
    DirectoryNode *c = dir->children;
    while (c) { child_count++; c = c->sibling; }
    fwrite(&child_count, sizeof(int), 1, fp);

    c = dir->children;
    while (c != NULL) {
        save_directory(fp, c);
        c = c->sibling;
    }
}

void save_filesystem(void) {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) { printf("  WARNING: Could not save filesystem.\n"); return; }
    save_directory(fp, filesystem_root);
    fclose(fp);
    printf("  Filesystem saved to '%s'.\n", DATA_FILE);
}

/* ════════════════════════════════════════════════════════════
   LOAD  (binary file  →  in-memory tree)
   ════════════════════════════════════════════════════════════ */

static FileNode *load_files(FILE *fp) {
    int count;
    fread(&count, sizeof(int), 1, fp);

    FileNode *head = NULL;

    for (int i = 0; i < count; i++) {
        FileNode *f = (FileNode *) malloc(sizeof(FileNode));
        if (!f) { fprintf(stderr, "Out of memory.\n"); exit(1); }

        fread(f->name,        sizeof(char),   MAX_NAME, fp);
        fread(&f->size,       sizeof(size_t), 1,        fp);
        fread(f->created_at,  sizeof(char),   32,       fp);
        fread(f->modified_at, sizeof(char),   32,       fp);

        size_t clen;
        fread(&clen, sizeof(size_t), 1, fp);
        if (clen > 0) {
            f->content = (char *) malloc(clen + 1);
            if (!f->content) { fprintf(stderr, "Out of memory.\n"); exit(1); }
            fread(f->content, sizeof(char), clen, fp);
            f->content[clen] = '\0';
        } else {
            f->content = NULL;
        }

        /* Prepend to list */
        f->next = head;
        head    = f;
    }

    return head;
}

static DirectoryNode *load_directory(FILE *fp, DirectoryNode *parent) {
    DirectoryNode *dir = (DirectoryNode *) malloc(sizeof(DirectoryNode));
    if (!dir) { fprintf(stderr, "Out of memory.\n"); exit(1); }

    fread(dir->name, sizeof(char), MAX_NAME, fp);
    dir->parent   = parent;
    dir->files    = load_files(fp);
    dir->children = NULL;
    dir->sibling  = NULL;

    int child_count;
    fread(&child_count, sizeof(int), 1, fp);

    for (int i = 0; i < child_count; i++) {
        DirectoryNode *child = load_directory(fp, dir);
        child->sibling = dir->children;
        dir->children  = child;
    }

    return dir;
}

int load_filesystem(void) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return 0;   /* no saved data yet */

    filesystem_root   = load_directory(fp, NULL);
    current_directory = filesystem_root;
    fclose(fp);

    printf("  Filesystem loaded from '%s'.\n", DATA_FILE);
    return 1;
}
