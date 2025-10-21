//
//  main.c
//  PA01_FindDups
//
//  Created by Raul Martinez on 2/10/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <openssl/evp.h> // Replaces deprecated SHA256 functions
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h> // Fix PATH_MAX

#define HASH_SIZE 65 // SHA256 hash is 64 hex characters + null terminator

// Struct to store file information
typedef struct FileEntry {
    char path[PATH_MAX];
    size_t size;
    char hash[HASH_SIZE];
    int processed; // Flag to track if already processed
    struct FileEntry* next;
} FileEntry;

// Head of linked list storing file entries
FileEntry* file_list = NULL;

// Function prototypes
void scan_directory(const char* dir_path);
int hash_file(const char* filename, char* outputBuffer);
void add_file(const char* path, size_t size, const char* hash);
void find_duplicates();
int compare_paths(const void* a, const void* b);
void free_file_list();

int main(int argc, char* argv[]) {
    // If no arguments, scan current directory
    if (argc == 1) {
        scan_directory(".");
    } else {
        for (int i = 1; i < argc; i++) {
            struct stat sb;
            if (stat(argv[i], &sb) == 0) {
                if (S_ISDIR(sb.st_mode)) {
                    scan_directory(argv[i]);
                } else if (S_ISREG(sb.st_mode)) {
                    char hash[HASH_SIZE];
                    if (hash_file(argv[i], hash) == 0) {
                        add_file(argv[i], sb.st_size, hash);
                    }
                }
            } else {
                fprintf(stderr, "Error accessing %s: %s\n", argv[i], strerror(errno));
            }
        }
    }

    find_duplicates();
    free_file_list(); //Free memory before exiting
    return 0;
}

// Recursively scans directories
void scan_directory(const char* dir_path) {
    DIR* dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "Cannot open directory %s: %s\n", dir_path, strerror(errno));
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[PATH_MAX]; // Fix for PATH_MAX error
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat sb;
        if (stat(full_path, &sb) == 0) {
            if (S_ISDIR(sb.st_mode)) {
                scan_directory(full_path);
            } else if (S_ISREG(sb.st_mode)) {
                char hash[HASH_SIZE];
                if (hash_file(full_path, hash) == 0) {
                    add_file(full_path, sb.st_size, hash);
                }
            }
        } else {
            fprintf(stderr, "Error accessing %s: %s\n", full_path, strerror(errno));
        }
    }

    closedir(dir);
}

// Computes SHA-256 hash of a file using OpenSSL EVP API
int hash_file(const char* filename, char* outputBuffer) {
    unsigned char hash[EVP_MAX_MD_SIZE]; // Buffer to store hash output
    unsigned int hash_len;
    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Cannot open file %s: %s\n", filename, strerror(errno));
        return -1;
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        fprintf(stderr, "Failed to create EVP_MD_CTX\n");
        fclose(file);
        return -1;
    }

    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);

    char buffer[4096];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        EVP_DigestUpdate(mdctx, buffer, bytesRead);
    }

    EVP_DigestFinal_ex(mdctx, hash, &hash_len);
    EVP_MD_CTX_free(mdctx);
    fclose(file);

    // Convert hash bytes to hex string
    for (unsigned int i = 0; i < hash_len; i++) {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }
    outputBuffer[HASH_SIZE - 1] = '\0';
    return 0;
}

// Adds a file to linked list
void add_file(const char* path, size_t size, const char* hash) {
    FileEntry* new_entry = malloc(sizeof(FileEntry));
    if (!new_entry) {
        fprintf(stderr, "Memory allocation failed for %s\n", path);
        return;
    }
    strncpy(new_entry->path, path, PATH_MAX);
    new_entry->size = size;
    strncpy(new_entry->hash, hash, HASH_SIZE);
    new_entry->processed = 0; // Set processed flag to 0
    new_entry->next = file_list;
    file_list = new_entry;
}

// Sort function for qsort
int compare_paths(const void* a, const void* b) {
    FileEntry* fileA = *(FileEntry**)a;
    FileEntry* fileB = *(FileEntry**)b;
    return strcmp(fileA->path, fileB->path);
}

// Finds duplicates and prints results
void find_duplicates() {
    int total_files = 0;
    for (FileEntry* temp = file_list; temp != NULL; temp = temp->next) {
        total_files++;
    }

    for (FileEntry* current = file_list; current != NULL; current = current->next) {
        if (current->processed) continue; // Skip already processed entries

        int count = 0;
        FileEntry** duplicates = malloc(sizeof(FileEntry*) * total_files); // Scales dynamically
        if (!duplicates) {
            fprintf(stderr, "Memory allocation failed for duplicate tracking\n");
            return;
        }

        FileEntry* compare = file_list;
        while (compare != NULL) {
            if (!compare->processed && strcmp(current->hash, compare->hash) == 0) {
                duplicates[count] = compare;
                count++;
                compare->processed = 1; //Mark as processed
            }
            compare = compare->next;
        }

        // Sort duplicate paths before printing
        qsort(duplicates, count, sizeof(FileEntry*), compare_paths);

        // Print duplicates only if count > 1
        if (count > 1) {
            for (int i = 0; i < count; i++) {
                printf("%d %d %s\n", count, i + 1, duplicates[i]->path);
            }
        }

        free(duplicates);
    }
}

// Frees the file list to prevent memory leaks
void free_file_list() {
    FileEntry* temp;
    while (file_list != NULL) {
        temp = file_list;
        file_list = file_list->next;
        free(temp);
    }
}

