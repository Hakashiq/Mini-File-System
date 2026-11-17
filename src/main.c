#include <stdio.h>
#include "../include/file_manager.h"
#include "../include/file_operations.h"
#include "../include/directory_operations.h"

/* Declared in storage.c */
int  load_filesystem(void);
void save_filesystem(void);

/* ─────────────────────────────────────────────────────────────
   list_directories()
   Lists all immediate child directories of current_directory.
   ───────────────────────────────────────────────────────────── */
static void list_directories(void) {
    DirectoryNode *c = current_directory->children;
    if (c == NULL) {
        printf("  (no subdirectories)\n");
        return;
    }
    printf("\n  Subdirectories:\n");
    while (c != NULL) {
        printf("    [DIR]  %s\n", c->name);
        c = c->sibling;
    }
    printf("\n");
}

/* ─────────────────────────────────────────────────────────────
   print_menu()
   Clears the screen and renders the main menu.
   ───────────────────────────────────────────────────────────── */
static void print_menu(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    printf("\n");
    printf("  ============================================\n");
    printf("        MINI FILE SYSTEM SIMULATOR\n");
    printf("  ============================================\n");
    printf("  Current Path: ");
    print_current_path();
    printf("\n  ============================================\n\n");

    printf("  --- FILE OPERATIONS ---\n");
    printf("   1.  Create File\n");
    printf("   2.  Delete File\n");
    printf("   3.  Write to File\n");
    printf("   4.  Read File\n");
    printf("   5.  Search File\n");
    printf("   6.  List Files\n");
    printf("   7.  Display File Information\n\n");

    printf("  --- DIRECTORY OPERATIONS ---\n");
    printf("   8.  Create Directory\n");
    printf("   9.  Delete Directory\n");
    printf("  10.  Change Directory\n");
    printf("  11.  List Subdirectories\n\n");

    printf("  12.  Exit\n");
    printf("\n  ============================================\n");
    printf("  Enter your choice: ");
}

/* ─────────────────────────────────────────────────────────────
   main()
   Entry point. Loads saved data or initialises fresh tree.
   Runs the menu loop until the user chooses Exit.
   Saves and frees everything on exit.
   ───────────────────────────────────────────────────────────── */
int main(void) {
    printf("\n  Starting Mini File System Simulator...\n");

    if (!load_filesystem()) {
        init_filesystem();
        printf("  No saved data found. Fresh filesystem created.\n");
    }

    int choice;

    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            /* Clear bad input */
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            choice = 0;
        }
        printf("\n");

        switch (choice) {
            case  1: create_file();        break;
            case  2: delete_file();        break;
            case  3: write_file();         break;
            case  4: read_file();          break;
            case  5: search_file();        break;
            case  6: list_files();         break;
            case  7: display_file_info();  break;
            case  8: create_directory();   break;
            case  9: delete_directory();   break;
            case 10: change_directory();   break;
            case 11: list_directories();   break;
            case 12: printf("  Goodbye!\n"); break;
            default: printf("  Invalid choice. Please enter 1-12.\n");
        }

        if (choice != 12) {
            printf("\n  Press Enter to return to menu...");
            getchar();
            getchar();
        }

    } while (choice != 12);

    save_filesystem();
    free_filesystem(filesystem_root);

    return 0;
}
