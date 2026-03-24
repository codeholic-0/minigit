#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "index.h"
#include "objects.h"

#define INDEX_FILE ".minigit/index"
#define TEMP_INDEX_FILE ".minigit/index.tmp"

int add_file(const char *filepath) {
    // 1. Check if the file actually exists on the filesystem
    if (access(filepath, F_OK) == -1) {
        fprintf(stderr, "fatal: pathspec '%s' did not match any files\n", filepath);
        return -1;
    }

    // 2. Hash the file and create the blob in .minigit/objects
    char *hash = hash_object(filepath);
    if (!hash) {
        fprintf(stderr, "fatal: updating files failed\n");
        return -1;
    }

    // 3. Open current index for reading, and a temp index for writing
    FILE *index_file = fopen(INDEX_FILE, "r");
    FILE *temp_file = fopen(TEMP_INDEX_FILE, "w");
    if (!temp_file) {
        perror("fatal: could not create temp index file");
        free(hash);
        if (index_file) fclose(index_file);
        return -1;
    }

    // 4. Read existing entries and copy them over
    char line[512];
    if (index_file) {
        while (fgets(line, sizeof(line), index_file)) {
            char existing_mode[16], existing_hash[64], existing_path[256];
            // Parse the line. %[^\n] means "read everything until the newline"
            if (sscanf(line, "%s %s %[^\n]", existing_mode, existing_hash, existing_path) == 3) {
                if (strcmp(existing_path, filepath) == 0) {
                    // This is the file we are updating. Skip copying the old entry.
                    continue; 
                }
                // Not our file, copy the existing entry to the temp index
                fprintf(temp_file, "%s %s %s\n", existing_mode, existing_hash, existing_path);
            }
        }
        fclose(index_file);
    }

    // 5. Write the new/updated entry to the temp file
    fprintf(temp_file, "100644 %s %s\n", hash, filepath);
    fclose(temp_file);

    // 6. Replace the old index with the new one
    if (rename(TEMP_INDEX_FILE, INDEX_FILE) != 0) {
        perror("fatal: failed to update index file");
        free(hash);
        return -1;
    }

    free(hash);
    return 0;
}
