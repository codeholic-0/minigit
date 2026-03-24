#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "objects.h"

int print_log(void) {
    FILE *ref_file = fopen(".minigit/refs/heads/master", "r");
    if (!ref_file) {
        fprintf(stderr, "fatal: your current branch 'master' does not have any commits yet\n");
        return -1;
    }

    char current_hash[64];
    if (fscanf(ref_file, "%s", current_hash) != 1) {
        fclose(ref_file);
        return -1;
    }
    fclose(ref_file);

    while (strlen(current_hash) > 0) {
        size_t obj_size;
        char *commit_data = read_object(current_hash, &obj_size);
        if (!commit_data) {
            fprintf(stderr, "fatal: missing object %s\n", current_hash);
            break;
        }

        // The object format is: "commit <size>\0<content>"
        // strchr finds the first null byte, and + 1 moves us to the actual content!
        char *content = strchr(commit_data, '\0') + 1;

        char parent_hash[64] = {0};
        char author[128] = {0};
        char message[512] = {0};

        // Parse the commit content line by line
        char *line = strtok(content, "\n");
        while (line != NULL) {
            if (strncmp(line, "parent ", 7) == 0) {
                strcpy(parent_hash, line + 7);
            } else if (strncmp(line, "author ", 7) == 0) {
                strcpy(author, line + 7);
            } else if (strlen(line) == 0) {
                // In Git, an empty line means the message starts next
                line = strtok(NULL, ""); // Grab the entire rest of the string
                if (line) strcpy(message, line);
                break;
            }
            line = strtok(NULL, "\n"); // Get next line
        }

        printf("commit %s\n", current_hash);
        printf("Author: %s\n\n", author);
        printf("    %s\n\n", message);

        // Move backwards to the parent commit. 
        // If there is no parent, parent_hash is empty and the loop breaks.
        strcpy(current_hash, parent_hash);

        free(commit_data);
    }

    return 0;
}
