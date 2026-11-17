#include "../include/file_manager.h"
DirectoryNode *filesystem_root   = NULL;
DirectoryNode *current_directory = NULL;

void get_timestamp(char *buffer) {
    time_t now    = time(NULL);
    struct tm *t  = localtime(&now);
    strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", t);
}

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

void get_current_path(DirectoryNode *dir, char *buffer) {
    if (dir == NULL) return;
    if (dir->parent == NULL) {
        snprintf(buffer, MAX_PATH, "/%s", dir->name);
        return;
    }
    char parent_path[MAX_PATH] = {0};
    get_current_path(dir->parent, parent_path);
    snprintf(buffer, MAX_PATH, "%s/%s", parent_path, dir->name);
}

static void free_file_list(FileNode *head) {
    while (head != NULL) {
        FileNode *temp = head;
        head = head->next;
        free(temp->content);   
        free(temp);            
    }
}

void free_filesystem(DirectoryNode *dir) {
    if (dir == NULL) return;
    DirectoryNode *child = dir->children;
    while (child != NULL) {
        DirectoryNode *next = child->sibling;
        free_filesystem(child); 
        child = next;
    }

    free_file_list(dir->files);
    free(dir);
}
