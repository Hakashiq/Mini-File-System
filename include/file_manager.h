#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME   64
#define MAX_PATH  512
#define DATA_FILE "data/filesystem.dat"

typedef struct FileNode {
    char   name[MAX_NAME];     
    size_t size;               
    char   created_at[32];    
    char   modified_at[32];   
    char  *content;            
    struct FileNode *next;     
} FileNode;

typedef struct DirectoryNode {
    char   name[MAX_NAME];              
    FileNode         *files;            
    struct DirectoryNode *parent;       
    struct DirectoryNode *children;     
    struct DirectoryNode *sibling;      
} DirectoryNode;

extern DirectoryNode *filesystem_root;
extern DirectoryNode *current_directory;

void init_filesystem(void);
void free_filesystem(DirectoryNode *dir);
void get_current_path(DirectoryNode *dir, char *buffer);
void get_timestamp(char *buffer);

#endif /* FILE_MANAGER_H */
