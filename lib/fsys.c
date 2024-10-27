#include <stdint.h>
#include "print.h"

#define MAX_FILES 128          // Maximum number of files
#define MAX_FILENAME_LENGTH 32 // Maximum length of a file name
#define MAX_FILE_SIZE 512      // Maximum size of file content

// Structure to represent a file
typedef struct {
    char name[MAX_FILENAME_LENGTH]; // File name
    char content[MAX_FILE_SIZE];    // File content in memory
    int size;                       // Size of the file
} File;

// File system structure to hold files
typedef struct {
    File files[MAX_FILES]; // Array of files
    int file_count;        // Current number of files
} FileSystem;

// Create the file system
FileSystem fs = { .file_count = 0 };

// Function to create a file in the file system
int create_file(const char *name) {
    if (fs.file_count >= MAX_FILES) {
        return -1; // File system is full
    }

    File *new_file = &fs.files[fs.file_count];

    // Copy the name into the file structure
    int i;
    for (i = 0; i < MAX_FILENAME_LENGTH - 1 && name[i] != '\0'; i++) {
        new_file->name[i] = name[i];
    }
    new_file->name[i] = '\0'; // Null-terminate the string

    // Initialize content and size
    new_file->size = 0; // Start with size 0
    fs.file_count++;     // Increment the file count
    return 0; // Success
}

// Function to compare two strings
int compare_strings(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return (*str1 == *str2) ? 0 : (*str1 - *str2);
}

// Function to list all files in the file system
void list_files() {
    for (int i = 0; i < fs.file_count; i++) {
        // Print the file name
        print(WHITE, fs.files[i].name);
        print(WHITE, "\n");
    }
}

// Function to read a file's content
void read_file(const char *name) {
    for (int i = 0; i < fs.file_count; i++) {
        if (compare_strings(fs.files[i].name, name) == 0) { // Check if the name matches
            print(WHITE, fs.files[i].content); // Assuming the content is a string
            print(WHITE, "\n");
            return;
        }
    }
    print(WHITE, "File not found.\n");
}

// Function to put a string into a file
void put(const char *name, const char *content) {
    for (int i = 0; i < fs.file_count; i++) {
        // Check if the file already exists
        if (compare_strings(fs.files[i].name, name) == 0) {
            // File exists, append content
            int content_length = 0;

            // Calculate the length of the new content
            while (content[content_length] != '\0') {
                content_length++;
            }

            // Check for available space and append
            if (fs.files[i].size + content_length < MAX_FILE_SIZE) {
                for (int j = 0; j < content_length; j++) {
                    fs.files[i].content[fs.files[i].size + j] = content[j]; // Append content
                }
                fs.files[i].size += content_length; // Update size
                fs.files[i].content[fs.files[i].size] = '\0'; // Null-terminate
                print(WHITE, "Appended ");
                print(WHITE, fs.files[i].name);
                print(WHITE, "\n");
                return;
            } else {
                print(WHITE, "Error: Not enough space in file ");
                print(WHITE, fs.files[i].name);
                print(WHITE, "\n");
                return;
            }
        }
    }

    // If the file does not exist, create it
    if (create_file(name) == 0) {
        // Now put the content into the new file
        int content_length = 0;
        while (content[content_length] != '\0') {
            content_length++;
        }

        // Check for space in the newly created file
        if (content_length < MAX_FILE_SIZE) {
            for (int j = 0; j < content_length; j++) {
                fs.files[fs.file_count - 1].content[j] = content[j]; // Copy content
            }
            fs.files[fs.file_count - 1].size = content_length; // Set size
            fs.files[fs.file_count - 1].content[content_length] = '\0'; // Null-terminate
            print(WHITE, "Created file: ");
            print(WHITE, fs.files[fs.file_count - 1].name);
            print(WHITE, "\n");
        } else {
            print(WHITE, "Error: Content too large for new file ");
            print(WHITE, fs.files[fs.file_count - 1].name);
            print(WHITE, "\n");
        }
    } else {
        print(WHITE, "Error: Maximum file limit reached.\n");
        print(WHITE, "\n");
    }
}
