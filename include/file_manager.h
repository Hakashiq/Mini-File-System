#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ───────────────────────────────────────────────────────────
   CONSTANTS
   ─────────────────────────────────────────────────────────── */
#define MAX_NAME   64
#define MAX_PATH  512
#define DATA_FILE "data/filesystem.dat"

/* ───────────────────────────────────────────────────────────
   FileNode — represents one file inside a directory.
   These are chained into a singly linked list.
   ─────────────────────────────────────────────────────────── */
typedef struct FileNode {
    char   name[MAX_NAME];     /* e.g. "notes.txt"              */
    size_t size;               /* number of bytes in content    */
    char   created_at[32];    /* creation timestamp string      */
    char   modified_at[32];   /* last-modified timestamp string */
    char  *content;            /* heap pointer to file content  */
    struct FileNode *next;     /* pointer to next file in list  */
} FileNode;

/* ───────────────────────────────────────────────────────────
   DirectoryNode — represents one directory.
   These form a tree via first-child / next-sibling links.
   ─────────────────────────────────────────────────────────── */
typedef struct DirectoryNode {
    char   name[MAX_NAME];              /* e.g. "docs"                     */
    FileNode         *files;            /* head of this dir's file list     */
    struct DirectoryNode *parent;       /* pointer to parent directory      */
    struct DirectoryNode *children;     /* pointer to first child directory */
    struct DirectoryNode *sibling;      /* pointer to next sibling dir      */
} DirectoryNode;

/* ───────────────────────────────────────────────────────────
   GLOBALS  (defined in file_manager.c)
   ─────────────────────────────────────────────────────────── */
extern DirectoryNode *filesystem_root;
extern DirectoryNode *current_directory;

/* ───────────────────────────────────────────────────────────
   FUNCTION DECLARATIONS
   ─────────────────────────────────────────────────────────── */
void init_filesystem(void);
void free_filesystem(DirectoryNode *dir);
void get_current_path(DirectoryNode *dir, char *buffer);
void get_timestamp(char *buffer);

#endif /* FILE_MANAGER_H */
