#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "commit.h"
#include "objects.h"

int create_commit(const char *message) {
    // 1. Read the Index to build the Tree
    FILE *index_file = fopen(".minigit/index", "r");
    if (!index_file) {
        fprintf(stderr, "fatal: nothing to commit (index is missing)\n");
        return -1;
    }

    // We will dump the contents of the index straight into a buffer to act as our Tree
    char tree_buffer[4096] = {0};
    char line[512];
    while (fgets(line, sizeof(line), index_file)) {
        strcat(tree_buffer, line);
    }
    fclose(index_file);

    if (strlen(tree_buffer) == 0) {
        fprintf(stderr, "fatal: nothing to commit (index is empty)\n");
        return -1;
    }

    // Save the Tree object
    char *tree_hash = write_object("tree", tree_buffer, strlen(tree_buffer));
    if (!tree_hash) {
        fprintf(stderr, "fatal: failed to write tree object\n");
        return -1;
    }

    // 2. Find the Parent Commit (if it exists)
    char parent_hash[64] = {0};
    FILE *ref_file = fopen(".minigit/refs/heads/master", "r");
    if (ref_file) {
        if (fscanf(ref_file, "%s", parent_hash) != 1) {
            parent_hash[0] = '\0'; // File was empty
        }
        fclose(ref_file);
    }

    // 3. Build the Commit Object string
    char commit_data[4096];
    time_t now = time(NULL); // Get current timestamp
    
    // Format depends on whether this is the first commit (no parent) or not
    if (strlen(parent_hash) > 0) {
        snprintf(commit_data, sizeof(commit_data),
                 "tree %s\nparent %s\nauthor Student <%ld>\n\n%s\n",
                 tree_hash, parent_hash, now, message);
    } else {
        snprintf(commit_data, sizeof(commit_data),
                 "tree %s\nauthor Student <%ld>\n\n%s\n",
                 tree_hash, now, message);
    }

    // Save the Commit object
    char *commit_hash = write_object("commit", commit_data, strlen(commit_data));
    free(tree_hash); // Cleanup tree hash
    
    if (!commit_hash) {
        fprintf(stderr, "fatal: failed to write commit object\n");
        return -1;
    }

    // 4. Update the branch pointer (master)
    ref_file = fopen(".minigit/refs/heads/master", "w");
    if (!ref_file) {
        perror("fatal: failed to update master branch pointer");
        free(commit_hash);
        return -1;
    }
    fprintf(ref_file, "%s\n", commit_hash);
    fclose(ref_file);

    // Success Output
    printf("[master (root-commit) %s] %s\n", commit_hash, message);
    
    free(commit_hash); // Cleanup commit hash
    return 0;
}
