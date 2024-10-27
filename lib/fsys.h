#ifndef FSYS_H
#define FSYS_H

#include <stdint.h>

#define MAX_FILES 128              // Maximum number of files
#define MAX_FILENAME_LENGTH 32     // Maximum length of a file name
#define MAX_FILE_SIZE 1024         // Maximum size of a file content

// Structure to represent a file
typedef struct {
    char name[MAX_FILENAME_LENGTH]; // File name
    char content[MAX_FILE_SIZE];    // Buffer to hold file content
    int size;                       // Size of the file content
} File;

// File system structure to hold files
typedef struct {
    File files[MAX_FILES]; // Array of files
    int file_count;        // Current number of files
} FileSystem;

// Function prototypes
int create_file(const char *name); // Create a file
int put(const char *name, const char *content); // Put content in a file
void list_files();                  // List all files
void read_file(const char *name);   // Read file content

#endif // FSYS_H
