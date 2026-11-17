#include "../include/file_manager.h"

/* ── Global definitions ──────────────────────────────────── */
DirectoryNode *filesystem_root   = NULL;
DirectoryNode *current_directory = NULL;

/* ─────────────────────────────────────────────────────────────
   get_timestamp()
   Writes the current date-time into buffer as a string.
   Example: "2024-11-17 08:30:00"
   ───────────────────────────────────────────────────────────── */
void get_timestamp(char *buffer) {
    time_t now    = time(NULL);
    struct tm *t  = localtime(&now);
    strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", t);
}

/* ─────────────────────────────────────────────────────────────
   init_filesystem()
   Allocates the root directory and sets current_directory.
   Called once at startup when no saved data exists.
   ───────────────────────────────────────────────────────────── */
void init_filesystem(void) {
    filesystem_root = (DirectoryNode *) malloc(sizeof(DirectoryNode));
    if (!filesystem_root) {
        fprintf(stderr, "FATAL: Out of memory.\n");
        exit(1);
    }
    strncpy(filesystem_root->name, "root", MAX_NAME);
    filesystem_root->files    = NULL;
    filesystem_root->parent   = NULL;
    filesystem_root->children = NULL;
    filesystem_root->sibling  = NULL;

    current_directory = filesystem_root;
}

/* ─────────────────────────────────────────────────────────────
   get_current_path()
   Recursively walks up via parent pointers and builds the
   full path string in buffer.
   Example: /root/docs/projects
   ───────────────────────────────────────────────────────────── */
void get_current_path(DirectoryNode *dir, char *buffer) {
    if (dir == NULL) return;

    if (dir->parent == NULL) {
        /* Base case: we are at root */
        snprintf(buffer, MAX_PATH, "/%s", dir->name);
        return;
    }

    /* Recursive step: build parent path first */
    char parent_path[MAX_PATH] = {0};
    get_current_path(dir->parent, parent_path);
    snprintf(buffer, MAX_PATH, "%s/%s", parent_path, dir->name);
}

/* ─────────────────────────────────────────────────────────────
   free_file_list()  — internal helper
   Frees every FileNode in a singly linked list.
   ───────────────────────────────────────────────────────────── */
static void free_file_list(FileNode *head) {
    while (head != NULL) {
        FileNode *temp = head;
        head = head->next;
        free(temp->content);   /* free heap content first */
        free(temp);            /* then free the node      */
    }
}

/* ─────────────────────────────────────────────────────────────
   free_filesystem()
   Post-order DFS: frees all children before the parent.
   Called once on program exit.
   ───────────────────────────────────────────────────────────── */
void free_filesystem(DirectoryNode *dir) {
    if (dir == NULL) return;

    DirectoryNode *child = dir->children;
    while (child != NULL) {
        DirectoryNode *next = child->sibling;
        free_filesystem(child);   /* recurse */
        child = next;
    }

    free_file_list(dir->files);
    free(dir);
}
